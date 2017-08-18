#include "stdafx.h"
#include "PointEncoder.h"


namespace CommonLib
{
	namespace Private
	{
		CPointEncoder::CPointEncoder(CommonLib::alloc_t *pAlloc)
		{
			clear();
		}
		CPointEncoder::~CPointEncoder()
		{

		}

		void CPointEncoder::clear()
		{ 
			m_SignX.clear();
			m_SignY.clear();
			m_Points.clear();

			m_X = 0;
			m_Y = 0;

			m_XPrev = 0;
			m_YPrev = 0;
		}
		void CPointEncoder::Reset()
		{
			m_SignX.Reset();
			m_SignY.Reset();
			m_Points.Reset();

			m_XPrev = m_X;
			m_YPrev = m_Y;
		}
		void CPointEncoder::encode(const GisXYPoint* pPts, uint32 nCnts, shape_compress_params *pParams, CommonLib::IWriteStream *pStream)
		{
			clear();
			calc(pPts, nCnts, pParams);
						
			uint32 nPos = pStream->pos();

			pStream->write((uint32)0);
			pStream->write(nCnts);						
			compressImpl(pPts, nCnts, pParams, pStream);
			uint32 nEndPos = pStream->pos();

			pStream->seek(nPos, soFromBegin);
			pStream->write(nEndPos - nPos);
			pStream->seek(nEndPos, soFromBegin);

		}

		void CPointEncoder::calc(const GisXYPoint* pPoints, uint32 nCnts, shape_compress_params *pParams)
		{

			double dScaleX = 1 / pow(10., pParams->m_nScaleX);
			double dScaleY = 1 / pow(10., pParams->m_nScaleY);

			uint64 X = (uint64)((pPoints[0].x + pParams->m_dOffsetX) / dScaleX);
			uint64 Y = (uint64)((pPoints[0].y + pParams->m_dOffsetY) / dScaleY);

			uint64 xPrev = X;
			uint64 yPrev = Y;

			uint64 xDiff = 0;
			uint64 yDiff = 0;
			for (uint32 i = 1; i < nCnts; ++i)
			{

				X = (uint64)((pPoints[i].x + pParams->m_dOffsetX) / dScaleX);
				Y = (uint64)((pPoints[i].y + pParams->m_dOffsetY) / dScaleY);

				PreAddCoord(i - 1, xPrev, X, m_SignX);
				PreAddCoord(i - 1, yPrev, Y, m_SignY);

				xPrev = X;
				yPrev = Y;
			}
		}
		void CPointEncoder::compressImpl(const GisXYPoint* pPoints, uint32 nCount, shape_compress_params *pParams, CommonLib::IWriteStream *pStream)
		{

			double dScaleX = 1 / pow(10., pParams->m_nScaleX);
			double dScaleY = 1 / pow(10., pParams->m_nScaleY);

			uint64 X = (uint64)((pPoints[0].x + pParams->m_dOffsetX) / dScaleX);
			uint64 Y = (uint64)((pPoints[0].y + pParams->m_dOffsetY) / dScaleY);

			m_SignX.BeginEncoding(pStream);
			m_SignY.BeginEncoding(pStream);
			pStream->write(X);
			pStream->write(Y);

			m_Points.BeginEncoding(pStream);
			
			uint64 xPrev = X;
			uint64 yPrev = Y;

			for (uint32 i = 1; i < nCount; ++i)
			{
				X = (uint64)((pPoints[i].x + pParams->m_dOffsetX) / dScaleX);
				Y = (uint64)((pPoints[i].y + pParams->m_dOffsetY) / dScaleY);


				CompreessCoord(i - 1, xPrev, X, m_SignX);
				CompreessCoord(i - 1, yPrev, Y,m_SignY);

				xPrev = X;
				yPrev = Y;
			}

			m_Points.FinishEncoding(pStream);
		}


		void CPointEncoder::PreAddCoord(uint32 nPos, uint64 prev, uint64 next, TSignEncoder& signCpmrpessor)
		{
			uint64 nDiff = 0;

			if (prev < next)
			{
				nDiff = next - prev;
				signCpmrpessor.AddSymbol(false);
			}
			else
			{
				nDiff = prev - next;
				signCpmrpessor.AddSymbol(true);
			}
			m_Points.AddSymbol(nDiff);
		}
		void CPointEncoder::CompreessCoord(uint32 nPos, uint64 prev, uint64 next, 
			TSignEncoder &signCpmrpessor)
		{
			uint32 nDiff = 0;
			if (prev < next)
			{
				nDiff = next - prev;
				signCpmrpessor.EncodeSign(false, nPos);
			}
			else
			{
				nDiff = prev - next;
				signCpmrpessor.EncodeSign(true, nPos);
			}

			m_Points.encodeSymbol(nDiff);
		}


		void CPointEncoder::InitDecode(CommonLib::IReadStream *pStream, shape_compress_params *pParams)
		{
			uint32 nBeginPos = pStream->pos();

			uint32 nSize = pStream->readIntu32();
			uint32 nCount = pStream->readIntu32();
			m_SignX.BeginDecoding(pStream, nCount);
			m_SignY.BeginDecoding(pStream, nCount);

			m_X = pStream->readIntu64();
			m_Y = pStream->readIntu64();

			uint32 nEncodeSize = pStream->pos() - nBeginPos;
			m_Points.BeginDecoding(pStream, nSize - nEncodeSize);
		}

		
	

		uint32 CPointEncoder::getPointCnts() const
		{
			return m_Points.count()/2 + 1;
		}
		GisXYPoint CPointEncoder::GetNextPoint(uint32 nPos, shape_compress_params *pParams) const
		{
			GisXYPoint pt;

			double dScaleX = 1 / pow(10., pParams->m_nScaleX); //TO DO
			double dScaleY = 1 / pow(10., pParams->m_nScaleY);

			if (nPos == 0)
			{

				pt.x = ((double)m_X *dScaleX) - pParams->m_dOffsetX;
				pt.y = ((double)m_Y *dScaleX) - pParams->m_dOffsetY;
			}
			else
			{
				uint64 xDiff = m_Points.decodeSymbol();
				uint64 yDiff = m_Points.decodeSymbol();

				bool bSignX = m_SignX.DecodeSign(nPos - 1);
				bool bSignY = m_SignY.DecodeSign(nPos - 1);

				if (bSignX)
					m_XPrev = m_XPrev - xDiff;
				else
					m_XPrev = m_XPrev + xDiff;

				if (bSignY)
					m_YPrev = m_YPrev - yDiff;
				else
					m_YPrev = m_YPrev + yDiff;

				pt.x = ((double)m_XPrev *dScaleX) - pParams->m_dOffsetX;
				pt.y = ((double)m_YPrev *dScaleX) - pParams->m_dOffsetY;
			}
			return pt;
		}
	}
}