#ifndef _LIB_COMMON_GEO_SHAPE_XY_COMPRESSOR_2_H_
#define _LIB_COMMON_GEO_SHAPE_XY_COMPRESSOR_2_H_
#include "NumLenCompressor.h"
#include "BitStream.h"
#include "GeoShape.h"
#include "PointZOrder.h"
#include <set>
#include "ByteCompressor.h"
#include "SignCompressor.h"
#include "XYCompressDiff.h"
namespace CommonLib
{

 
//#define _CALC_STATISTIC_




	template <class _TValue, class _TPointLenCompressor>
	class TXYCompressorDiff2 : public IXYComressor
	{
	public:
		typedef _TPointLenCompressor TPointLenCompressor;
		typedef _TValue TValue;


		TXYCompressorDiff2(const CGeoShape::compress_params& params) : m_params(params)
		{
			m_nXBitLen = 0;
			m_nYBitLen = 0;
		}
		~TXYCompressorDiff2(){}

		virtual eCompressDataType GeteCompressDataType() const
		{
			return m_params.m_PointType;
		}

		uint32 PreAddCoord(TValue prev, TValue next, TSignCompressor &signCpmrpessor)
		{
			TValue nDiff = 0;

			if(prev <  next)
			{
				nDiff = next - prev;
				signCpmrpessor.AddSymbol(false);
			}
			else
			{
				nDiff = prev - next;
				signCpmrpessor.AddSymbol(true);
			}

			return m_PointCompressor.PreAddSympol(nDiff);
		}


		void CompreessCoord( uint32 nPos, TValue prev, TValue next, FxBitWriteStream& bitStream,
				TSignCompressor &signCpmrpessor)
		{
			TValue nDiff = 0;

			if(prev <  next)
			{
				nDiff = next - prev;
				signCpmrpessor.EncodeSign(false, nPos);
			}
			else
			{
				nDiff = prev - next;
				signCpmrpessor.EncodeSign(true, nPos);
			}

			m_PointCompressor.EncodeSymbol(nDiff, &bitStream);
		}

		virtual void PreCompress(const GisXYPoint *pPoint, uint32 nCount)
		{


			TValue X = (TValue)((pPoint[0].x + m_params.m_dOffsetX)/m_params.m_dScaleX);
			TValue Y = (TValue)((pPoint[0].y + m_params.m_dOffsetY)/m_params.m_dScaleY);

			/*ZOrder zOrderPrev(X, Y);
			ZOrder zOrderNext;
			ZOrder zOrderDiff;*/


			TValue xPrev = X;
			TValue yPrev = Y;

			TValue xDiff = 0;
			TValue yDiff = 0;
			for (uint32 i = 1; i < nCount; ++i)
			{

				X = (TValue)((pPoint[i].x + m_params.m_dOffsetX)/m_params.m_dScaleX);
				Y = (TValue)((pPoint[i].y + m_params.m_dOffsetY)/m_params.m_dScaleY);

			 
				m_nXBitLen += PreAddCoord(xPrev, X, m_SignCompressorX);
				m_nYBitLen += PreAddCoord(yPrev, Y, m_SignCompressorY);


				xPrev = X;
				yPrev = Y;

				/*zOrderNext.setZOrder(X, Y);

				if(zOrderPrev <  zOrderNext)
				{
					zOrderDiff = zOrderNext - zOrderPrev;
					m_SignCompressor.AddSymbol(false);
				}
				else
				{
					zOrderDiff = zOrderPrev - zOrderNext;
					m_SignCompressor.AddSymbol(true);
				}

				zOrderDiff.getXY(xDiff, yDiff);
				m_Compressor.PreAddSympol(xDiff);
				m_Compressor.PreAddSympol(yDiff);

				zOrderPrev = zOrderNext;*/
			}
		}

		virtual uint32 GetCompressSize() const
		{
			uint32 nSignSizeX = m_SignCompressorX.GetCompressSize();
			uint32 nSignSizeY = m_SignCompressorY.GetCompressSize();
			//uint32 nSize = m_PointCompressor.GetCompressSize() + (m_PointCompressor.GetBitsLen() +7)/8;
			uint32 nSize = m_PointCompressor.GetCompressSize() + (m_nXBitLen +7)/8 + (m_nYBitLen +7)/8 + 2*sizeof(uint32); 

			return  nSize +  2 * sizeof(TValue) + nSignSizeX + nSignSizeY;
		}
		virtual void WriteHeader(IWriteStream *pStream)
		{
			m_PointCompressor.WriteHeader(pStream);
		}

		virtual bool compress(const GisXYPoint *pPoint, uint32 nCount, IWriteStream *pStream)
		{

			uint32 nByteSizeX = (m_nXBitLen + 7)/8;
			uint32 nByteSizeY = (m_nYBitLen + 7)/8;


			pStream->write(nByteSizeX);
			pStream->write(nByteSizeY);

			CommonLib::FxBitWriteStream bitStreamX;
			CommonLib::FxBitWriteStream bitStreamY;

			bitStreamX.attach(pStream, pStream->pos(), nByteSizeX);
			pStream->seek(nByteSizeX, soFromCurrent);


			bitStreamY.attach(pStream, pStream->pos(), nByteSizeY);
			pStream->seek(nByteSizeY, soFromCurrent);
		
			m_SignCompressorX.BeginCompress(pStream);
			m_SignCompressorY.BeginCompress(pStream);

			TValue X = (TValue)((pPoint[0].x + m_params.m_dOffsetX)/m_params.m_dScaleX);
			TValue Y = (TValue)((pPoint[0].y + m_params.m_dOffsetY)/m_params.m_dScaleY);

			/*ZOrder zOrderPrev(X, Y);
			ZOrder zOrderNext;
			ZOrder zOrderDiff;*/

			TValue xPrev = X;
			TValue yPrev = Y;

			TValue xDiff = 0;
			TValue yDiff = 0;


			pStream->write(X);
			pStream->write(Y);
			m_PointCompressor.BeginCompreess(pStream);

			for (uint32 i = 1; i < nCount; ++i)
			{

				X = (TValue)((pPoint[i].x + m_params.m_dOffsetX)/m_params.m_dScaleX);
				Y = (TValue)((pPoint[i].y + m_params.m_dOffsetY)/m_params.m_dScaleY);


				CompreessCoord(i -1, xPrev, X, bitStreamX, m_SignCompressorX);
				CompreessCoord(i -1, yPrev, Y, bitStreamY, m_SignCompressorY);

				xPrev = X;
				yPrev = Y;
				/*zOrderNext.setZOrder(X, Y);

				if(zOrderPrev <  zOrderNext)
				{
					zOrderDiff = zOrderNext - zOrderPrev;
					//bitStream.writeBit(false);
					m_SignCompressor.EncodeSign(false, i - 1);

				}
				else
				{
					zOrderDiff = zOrderPrev - zOrderNext;
					m_SignCompressor.EncodeSign(true, i - 1);
				}


				zOrderDiff.getXY(xDiff, yDiff);
				uint32 xLen =  m_Compressor.EncodeSymbol(xDiff, &bitStream);
				uint32 yLen =  m_Compressor.EncodeSymbol(yDiff, &bitStream);


				zOrderPrev = zOrderNext;*/
			}

			m_PointCompressor.EncodeFinish();

#ifdef		_CALC_STATISTIC_
		
			if(bitStreamX.size() > 512)
			{
				CWriteMemoryStream writeDynamicStream;
				CWriteMemoryStream writeStaticStream;
				CByteCompressor bytecompress;
				uint32 nPos = bitStreamX.pos();
				bytecompress.encodeDynamic(bitStreamX.buffer(), bitStreamX.pos(), &writeDynamicStream);
				bytecompress.encodeStatic(bitStreamX.buffer(), bitStreamX.pos(), &writeStaticStream);
				int dynamicSize =writeDynamicStream.pos();
				int staticSize =writeStaticStream.pos();

				int dd = 0;
				dd++;
			}


			if(bitStreamY.pos() > 512)
			{
				CWriteMemoryStream writeDynamicStream;
				CWriteMemoryStream writeStaticStream;
				CByteCompressor bytecompress;

				uint32 nPos = bitStreamY.pos();
				bytecompress.encodeDynamic(bitStreamY.buffer(), bitStreamY.pos(), &writeDynamicStream);
				bytecompress.encodeStatic(bitStreamY.buffer(), bitStreamY.pos(), &writeStaticStream);
				int dynamicSize =writeDynamicStream.pos();
				int staticSize =writeStaticStream.pos();

				int dd = 0;
				dd++;
			}


#endif

			return true;
		}



		void clear(const CGeoShape::compress_params* pParams = NULL)
		{
			m_PointCompressor.clear();
			this->m_nBitLen = 0;
			m_nXBitLen = 0;
			m_nYBitLen = 0;

			m_SignCompressorX.clear();
			m_SignCompressorY.clear();

			if(pParams)
				m_params = *pParams;
		}

		virtual void ReadHeader(IReadStream *pStream)
		{
			m_PointCompressor.ReadHeader(pStream);
		}


		virtual uint32 GetPointCount() const
		{
			return m_PointCompressor.GetCount()/2 + 1;
		}

		virtual uint32 GetCount() const
		{
			return m_PointCompressor.GetCount()/2;
		}


		 

		virtual bool decompress(GisXYPoint *pPoint, uint32 nCount, IReadStream *pStream)
		{


			uint32 nByteSizeX = pStream->readIntu32();
			uint32 nByteSizeY = pStream->readIntu32();

			CommonLib::FxBitReadStream bitStreamX, bitStreamY;

			bitStreamX.attach(pStream, pStream->pos(), nByteSizeX);
			pStream->seek(nByteSizeX, soFromCurrent);


			bitStreamY.attach(pStream, pStream->pos(), nByteSizeY);
			pStream->seek(nByteSizeY, soFromCurrent);


			m_SignCompressorX.BeginDecompress(pStream, nCount - 1);
			m_SignCompressorY.BeginDecompress(pStream, nCount - 1);

			TValue X = 0;
			TValue Y = 0;

			pStream->read(X);
			pStream->read(Y);


			pPoint[0].x =  ((double)X *m_params.m_dScaleX) - m_params.m_dOffsetX;  
			pPoint[0].y =  ((double)Y *m_params.m_dScaleY) - m_params.m_dOffsetY;

			TValue xNext = X;
			TValue yNext = Y;
		

			TValue xDiff = 0;
			TValue yDiff = 0;

			uint32 nBitX = 0;
			uint32 nBitY = 0;

			m_PointCompressor.StartDecode(pStream);
			for (uint32 i = 1; i < nCount; ++i)
			{

				m_PointCompressor.DecodeSymbol(nBitX);
				m_PointCompressor.DecodeSymbol(nBitY);


				bool bSignX = m_SignCompressorX.DecodeSign(i - 1); 
				bool bSignY = m_SignCompressorY.DecodeSign(i - 1); 

				xDiff = nBitX;
				yDiff = nBitY;
				if(xDiff > 1)
				{
					xDiff = 0;
					bitStreamX.readBits(xDiff, nBitX - 1);
					xDiff |= (1 << nBitX- 1);
				}

				if(yDiff > 1)
				{
					yDiff = 0;
					bitStreamY.readBits(yDiff, nBitY- 1);
					yDiff |= (1 << nBitY- 1);
				}

				if(bSignX)
					xNext = xNext - xDiff;
				else
					xNext = xNext + xDiff;

				if(bSignY)
					yNext = yNext - yDiff;
				else
					yNext = yNext + yDiff;

				pPoint[i].x =  ((double)xNext *m_params.m_dScaleX) - m_params.m_dOffsetX;  
				pPoint[i].y =  ((double)yNext *m_params.m_dScaleY) - m_params.m_dOffsetY;

				//zOrderPrev = zOrderNext;
			}
			return true;
		}
	private:

		TPointLenCompressor m_PointCompressor;
		TSignCompressor m_SignCompressorX;

		TSignCompressor m_SignCompressorY;
		CGeoShape::compress_params m_params;

		uint32 m_nXBitLen;
		uint32 m_nYBitLen;
		
	};

	typedef TXYCompressorDiff2<uint16, TPointNumLen16> TXYCompressor16_2;
	typedef TXYCompressorDiff2<uint32, TPointNumLen32> TXYCompressor32_2;
	typedef TXYCompressorDiff2<uint64, TPointNumLen64> TXYCompressor64_2;
}

#endif