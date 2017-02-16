#include "stdafx.h"
#include "PointCompressor.h"
namespace CommonLib
{
	CPointCompressor::CPointCompressor(CommonLib::alloc_t *pAlloc) : 
		m_pCacheStreamX(pAlloc), m_pCacheStreamY(pAlloc), m_pCacheSignX(pAlloc),
		m_pCacheSignY(pAlloc), m_nFlag(0)
	{

	}
	CPointCompressor::~CPointCompressor()
	{

	}

	void CPointCompressor::clear()
	{
		m_SignX.clear();
		m_SignY.clear();

		m_Point.clear();
	}
	bool CPointCompressor::compress(const CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IWriteStream *pStream)
	{
		clear();
		uint32 nCount = pShp->getPointCnt();
		if (nCount < 10)
		{
			m_nFlag = 0;
			pStream->write(m_nFlag);
			WriteRawPoint(pShp, pParams, pStream);
			return true;
		}
		m_nFlag = compress_point;
		pStream->write(m_nFlag);
		calc(pShp, pParams);

		compressImpl(pShp, pParams, pStream);


		return true;
	}

	void  CPointCompressor::calc(const CGeoShape *pShp, CGeoShape::compress_params *pParams)
	{
		uint32 nCount = pShp->getPointCnt();

		double dScaleX = 1 / pow(10., pParams->m_nScaleX);
		double dScaleY = 1 / pow(10., pParams->m_nScaleY);

		m_SignX.InitCompress(nCount - 1);
		m_SignX.InitCompress(nCount - 1);

		const GisXYPoint* pPoints = pShp->getPoints();

		uint64 X = (uint64)((pPoints[0].x + pParams->m_dOffsetX) / dScaleX);
		uint64 Y = (uint64)((pPoints[0].y + pParams->m_dOffsetY) / dScaleY);

		uint64 xPrev = X;
		uint64 yPrev = Y;

		uint64 xDiff = 0;
		uint64 yDiff = 0;
		for (uint32 i = 1; i < nCount; ++i)
		{

			X = (uint64)((pPoints[i].x + pParams->m_dOffsetX) / dScaleX);
			Y = (uint64)((pPoints[i].y + pParams->m_dOffsetY) / dScaleY);

			 

			PreAddCoord(i - 1, xPrev, X, m_SignX);
			PreAddCoord(i - 1, yPrev, Y, m_SignY);

			xPrev = X;
			yPrev = Y;
		}

	}

	void CPointCompressor::PreAddCoord(uint32 nPos, uint64 prev, uint64 next,  TSignCompressor2& signCpmrpessor)
	{
		uint64 nDiff = 0;

		if (prev < next)
		{
			nDiff = next - prev;
			signCpmrpessor.AddSymbol(false, nPos);
		}
		else
		{
			nDiff = prev - next;
			signCpmrpessor.AddSymbol(true, nPos);
		}
		m_Point.PreAddSympol(nDiff);
	}


	void CPointCompressor::CompreessCoord(uint32 nPos, uint64 prev, uint64 next, FxBitWriteStream& bitStream,
		TSignCompressor2 &signCpmrpessor)
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

		m_Point.EncodeSymbol(nDiff, &bitStream);
	}


	bool CPointCompressor::compressImpl(const CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IWriteStream *pStream)
	{
		uint32 nCount = pShp->getPointCnt();
		if (nCount == 0)
			return true;

		m_pCacheSignX.seek(0, soFromBegin);
		m_pCacheSignY.seek(0, soFromBegin);

		m_pCacheStreamX.seek(0, soFromBegin);
		m_pCacheStreamY.seek(0, soFromBegin);

		double dScaleX = 1 / pow(10., pParams->m_nScaleX);
		double dScaleY = 1 / pow(10., pParams->m_nScaleY);

		const GisXYPoint* pPoints = pShp->getPoints();

		uint64 X = (uint64)((pPoints[0].x + pParams->m_dOffsetX) / dScaleX);
		uint64 Y = (uint64)((pPoints[0].y + pParams->m_dOffsetY) / dScaleY);


		m_Point.BeginEncode(pStream);
		m_Point.WriteHeader(pStream);

		m_SignX.BeginCompress(pStream);
		m_SignY.BeginCompress(pStream);





		pStream->write(X);
		pStream->write(Y);

		uint32 nBitSize = m_Point.GetBitsLen();
		uint32 nByteSize = (nBitSize + 7) / 8;

		CommonLib::FxBitWriteStream bitStream;

		bitStream.attach(pStream, pStream->pos(), nByteSize);
		pStream->seek(nByteSize, soFromCurrent);



		uint64 xPrev = X;
		uint64 yPrev = Y;

		for (uint32 i = 1; i < nCount; ++i)
		{
			X = (uint64)((pPoints[i].x + pParams->m_dOffsetX) / dScaleX);
			Y = (uint64)((pPoints[i].y + pParams->m_dOffsetY) / dScaleY);


			CompreessCoord(i - 1, xPrev, X, bitStream, m_SignX);
			CompreessCoord(i - 1, yPrev, Y, bitStream, m_SignY);

			xPrev = X;
			yPrev = Y;
		}

		m_Point.EncodeFinish();

		return true;
	}

	bool CPointCompressor::decompress(CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IReadStream *pStream)
	{
		clear();

		byte nFlag = pStream->readByte();
		if (nFlag == 0)
		{
			ReadRawPoint(pShp, pParams, pStream);
			return true;
		}
		m_Point.Init(pStream);
		//m_Point.ReadHeader(pStream);

		uint32 nCount = m_Point.GetCount()/2;


		m_SignX.BeginDecompress(pStream, nCount);
		m_SignY.BeginDecompress(pStream, nCount);


		pShp->createPoints(nCount + 1);
 

		uint64 X = pStream->readIntu64();
		uint64 Y = pStream->readIntu64();

		uint32 nBitSize = m_Point.GetBitsLen();
		uint32 nByteSize = (nBitSize + 7) / 8;

		CommonLib::FxBitReadStream bitStream;

		bitStream.attach(pStream, pStream->pos(), nByteSize);
		pStream->seek(nByteSize, soFromCurrent);

		double dScaleX = 1 / pow(10., pParams->m_nScaleX);
		double dScaleY = 1 / pow(10., pParams->m_nScaleY);

		GisXYPoint* pPoints = pShp->getPoints();

		pPoints[0].x = ((double)X *dScaleX) - pParams->m_dOffsetX;
		pPoints[0].y = ((double)Y *dScaleY) - pParams->m_dOffsetY;

		uint64 xNext = X;
		uint64 yNext = Y;
		
		uint64 xDiff = 0;
		uint64 yDiff = 0;

		uint32 nBitX = 0;
		uint32 nBitY = 0;

		m_Point.StartDecode();

		for (uint32 i = 1; i < nCount + 1; ++i)
		{
			m_Point.DecodeSymbol(nBitX);
			m_Point.DecodeSymbol(nBitY);


			bool bSignX = m_SignX.DecodeSign(i - 1);
			bool bSignY = m_SignY.DecodeSign(i - 1);

			xDiff = nBitX;
			yDiff = nBitY;
			if (xDiff > 1)
			{
				xDiff = 0;
				bitStream.readBits(xDiff, nBitX - 1);
				xDiff |= ((uint64)1 << (nBitX - 1));
			}

			if (yDiff > 1)
			{
				yDiff = 0;
				bitStream.readBits(yDiff, nBitY - 1);
				yDiff |= ((uint64)1 << (nBitY - 1));
			}

			if (bSignX)
				xNext = xNext - xDiff;
			else
				xNext = xNext + xDiff;

			if (bSignY)
				yNext = yNext - yDiff;
			else
				yNext = yNext + yDiff;

			pPoints[i].x = ((double)xNext *dScaleX) - pParams->m_dOffsetX;
			pPoints[i].y = ((double)yNext *dScaleY) - pParams->m_dOffsetY;
		}
		

		return true;
	}


	void CPointCompressor::WriteRawPoint(const CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IWriteStream *pStream)
	{
		uint32 nCount = pShp->getPointCnt();
		const GisXYPoint* pPoints = pShp->getPoints();

		pStream->write(nCount);

		if (pParams->m_PointType != dtType64)
		{
			double dScaleX = 1 / pow(10., pParams->m_nScaleX);
			double dScaleY = 1 / pow(10., pParams->m_nScaleY);

			for (uint32 i = 0; i < nCount; ++i)
			{
				uint64 X = (uint64)((pPoints[i].x + pParams->m_dOffsetX) / dScaleX);
				uint64 Y = (uint64)((pPoints[i].y + pParams->m_dOffsetY) / dScaleY);

				WriteValue(X, pParams->m_PointType, pStream);
				WriteValue(Y, pParams->m_PointType, pStream);
			}
		}
		else
		{
			for (uint32 i = 0; i < nCount; ++i)
			{
				pStream->write(pPoints[i].x);
				pStream->write(pPoints[i].y);
			}
		}
	}
	void CPointCompressor::ReadRawPoint(CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IReadStream *pStream)
	{
		uint32 nCount = pStream->readIntu32();
		if (nCount == 0)
			return;

		pShp->createPoints(nCount);

		if (pParams->m_PointType != dtType64)
		{
			double dScaleX = 1 / pow(10., pParams->m_nScaleX);
			double dScaleY = 1 / pow(10., pParams->m_nScaleY);

			for (uint32 i = 0, sz = nCount; i < sz; ++i)
			{
		

				uint64 X = ReadValue<uint64>(pParams->m_PointType, pStream);
				uint64 Y = ReadValue<uint64>(pParams->m_PointType, pStream);

				pShp->getPoints()[i].x = ((double)X *dScaleX) - pParams->m_dOffsetX;
				pShp->getPoints()[i].y = ((double)Y *dScaleY) - pParams->m_dOffsetY;
			}
		}
		else
		{
			for (uint32 i = 0; i < nCount; ++i)
			{
				pShp->getPoints()[i].x = pStream->readDouble();
				pShp->getPoints()[i].y = pStream->readDouble();
			}
		}

	}
}