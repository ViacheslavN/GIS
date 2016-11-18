#ifndef _LIB_COMMON_GEO_SHAPE_XY_COMPRESSOR_H_
#define _LIB_COMMON_GEO_SHAPE_XY_COMPRESSOR_H_
#include "NumLenCompressor.h"
#include "BitStream.h"
#include "GeoShape.h"
#include "PointZOrder.h"
#include <set>
#include "ByteCompressor.h"
#include "SignCompressor.h"
//#include "XYCompressZOrderDiff.h"
#include "IXYComressor.h"
#include "StaticSignCompressor.h"
namespace CommonLib
{
 

//#define _CALC_STATISTIC_




	template <class _TValue, class _TPointLenCompressor>
	class TXYCompressorDiff : public IXYComressor
	{
	public:
		typedef _TPointLenCompressor TPointLenCompressor;
		typedef _TValue TValue;
		TXYCompressorDiff(const CGeoShape::compress_params& params) : m_params(params)
		{

			m_dScaleX = 1/pow(10., m_params.m_nScaleX);
			m_dScaleY = 1/pow(10., m_params.m_nScaleY);
	
		}
		~TXYCompressorDiff(){}


		virtual eCompressDataType GeteCompressDataType() const
		{
			return m_params.m_PointType;
		}
		template<class TCompressor>
		uint32 PreAddCoord(uint32 nPos, TValue prev, TValue next, TCompressor &signCpmrpessor)
		{
			TValue nDiff = 0;

			if(prev <  next)
			{
				nDiff = next - prev;
				signCpmrpessor.AddSymbol(false, nPos);
			}
			else
			{
				nDiff = prev - next;
				signCpmrpessor.AddSymbol(true, nPos);
			}

			return m_PointCompressor.PreAddSympol(nDiff);
		}

		template<class TCompressor>
		void CompreessCoord( uint32 nPos, TValue prev, TValue next, FxBitWriteStream& bitStream,
				TCompressor &signCpmrpessor)
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

			m_SignCompressorX.InitCompress(nCount - 1);
			m_SignCompressorY.InitCompress(nCount - 1);


			TValue X = (TValue)((pPoint[0].x + m_params.m_dOffsetX)/m_dScaleX);
			TValue Y = (TValue)((pPoint[0].y + m_params.m_dOffsetY)/m_dScaleY);

			TValue xPrev = X;
			TValue yPrev = Y;

			TValue xDiff = 0;
			TValue yDiff = 0;
			for (uint32 i = 1; i < nCount; ++i)
			{

				X = (TValue)((pPoint[i].x + m_params.m_dOffsetX)/m_dScaleX);
				Y = (TValue)((pPoint[i].y + m_params.m_dOffsetY)/m_dScaleY);
							 
				PreAddCoord(i - 1, xPrev, X, m_SignCompressorX);
				PreAddCoord(i - 1, yPrev, Y, m_SignCompressorY);
				
				xPrev = X;
				yPrev = Y;
			}
		}

		virtual uint32 GetCompressSize() const
		{
			uint32 nSignSizeX = m_SignCompressorX.GetCompressSize();
			uint32 nSignSizeY = m_SignCompressorY.GetCompressSize();
			uint32 nSize = m_PointCompressor.GetCompressSize() + (m_PointCompressor.GetBitsLen() +7)/8;

			return  nSize +  2 * sizeof(TValue) + nSignSizeX + nSignSizeY;
		}
		virtual void WriteHeader(IWriteStream *pStream)
		{
			m_PointCompressor.WriteHeader(pStream);
		}

		virtual bool compress(const GisXYPoint *pPoint, uint32 nCount, IWriteStream *pStream)
		{


			uint32 nBitSize = m_PointCompressor.GetBitsLen();
			uint32 nByteSize = (nBitSize + 7)/8;

 

			CommonLib::FxBitWriteStream bitStream;

			bitStream.attach(pStream, pStream->pos(), nByteSize);
			pStream->seek(nByteSize, soFromCurrent);

		
			m_SignCompressorX.BeginCompress(pStream);
			m_SignCompressorY.BeginCompress(pStream);

			TValue X = (TValue)((pPoint[0].x + m_params.m_dOffsetX)/m_dScaleX);
			TValue Y = (TValue)((pPoint[0].y + m_params.m_dOffsetY)/m_dScaleY);


			TValue xPrev = X;
			TValue yPrev = Y;

			TValue xDiff = 0;
			TValue yDiff = 0;


			pStream->write(X);
			pStream->write(Y);
			m_PointCompressor.BeginCompress(pStream);

			for (uint32 i = 1; i < nCount; ++i)
			{

				X = (TValue)((pPoint[i].x + m_params.m_dOffsetX)/m_dScaleX);
				Y = (TValue)((pPoint[i].y + m_params.m_dOffsetY)/m_dScaleY);


				CompreessCoord(i -1, xPrev, X, bitStream, m_SignCompressorX);
				CompreessCoord(i -1, yPrev, Y, bitStream, m_SignCompressorY);

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
			byte nBytes[256];
			uint32 nLens[20000];
			memset(nBytes, 0, sizeof(nBytes));
			memset(nLens, 0, sizeof(nLens));
			byte* pByte = bitStream.buffer();

			byte nCurrByte;
			int nCurrLen = 0;
			int nDiff = 0;
			int nCountByte = 0;
			for (uint32 i = 0; i < bitStream.pos(); ++i)
			{


				nBytes[pByte[i]] += 1;

				if(nBytes[pByte[i]] == 1)
				{
					nDiff +=1;
				}
				nCountByte += 1;
				if(i == 0)
				{
					nCurrByte = pByte[i];
				}
				else
				{
					if(nCurrByte != pByte[i])
					{
						nCurrByte = pByte[i];
						nLens[nCurrLen] += 1;
						nCurrLen = 0;
					}
					else
					{
						nCurrLen++;
					}
				}

			}


			if(nCountByte > 1024)
			{
				CWriteMemoryStream writeDynamicStream;
				CWriteMemoryStream writeStaticStream;
				CByteCompressor bytecompress;

				bytecompress.encodeDynamic(pByte, bitStream.pos(), &writeDynamicStream);
				bytecompress.encodeStatic(pByte, bitStream.pos(), &writeStaticStream);
				int dynamicSize =writeDynamicStream.pos();
				int staticSize =writeStaticStream.pos();

				int dd = 0;
				dd++;
			}

#endif

			return true;
		}



		virtual void clear(const CGeoShape::compress_params* pParams = NULL)
		{
			m_PointCompressor.clear();
			 
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
			uint32 nBitLen = m_PointCompressor.GetBitsLen();
			uint32 nByteSize = (nBitLen + 7)/8;
			CommonLib::FxBitReadStream bitStream;

			bitStream.attach(pStream, pStream->pos(), nByteSize);
			pStream->seek(nByteSize, soFromCurrent);


			m_SignCompressorX.BeginDecompress(pStream, nCount - 1);
			m_SignCompressorY.BeginDecompress(pStream, nCount - 1);

			TValue X = 0;
			TValue Y = 0;

			pStream->read(X);
			pStream->read(Y);


			pPoint[0].x =  ((double)X *m_dScaleX) - m_params.m_dOffsetX;  
			pPoint[0].y =  ((double)Y *m_dScaleY) - m_params.m_dOffsetY;

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
					bitStream.readBits(xDiff, nBitX - 1);
					xDiff |= (1 << nBitX- 1);
				}

				if(yDiff > 1)
				{
					yDiff = 0;
					bitStream.readBits(yDiff, nBitY- 1);
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

				pPoint[i].x =  ((double)xNext *m_dScaleX) - m_params.m_dOffsetX;  
				pPoint[i].y =  ((double)yNext *m_dScaleY) - m_params.m_dOffsetY;

				//zOrderPrev = zOrderNext;
			}
			return true;
		}
	private:

		TPointLenCompressor m_PointCompressor;
		//TSignCompressor m_SignCompressorX;
		//TSignCompressor m_SignCompressorY;


		TStaticSignCompressor m_SignCompressorX;
		TStaticSignCompressor m_SignCompressorY;

		CGeoShape::compress_params m_params;



		double m_dScaleX;
		double m_dScaleY;
 
		
	};

	typedef TXYCompressorDiff<uint16, TPointNumLen16> TXYCompressor16;
	typedef TXYCompressorDiff<uint32, TPointNumLen32> TXYCompressor32;
	typedef TXYCompressorDiff<uint64, TPointNumLen64> TXYCompressor64;
}

#endif