#ifndef _LIB_COMMON_GEO_SHAPE_XY_COMPRESSOR_H_
#define _LIB_COMMON_GEO_SHAPE_XY_COMPRESSOR_H_
#include "NumLenCompressor.h"
#include "BitStream.h"
#include "GeoShape.h"
#include "PointZOrder.h"
#include <set>
#include "ByteCompressor.h"
#include "SignCompressor.h"

namespace CommonLib
{


	typedef TNumLemCompressor<uint16, TFindMostSigBit, 16> TPointNumLen16;
	typedef TNumLemCompressor<uint32, TFindMostSigBit, 32> TPointNumLen32;
	typedef TNumLemCompressor<uint64, TFindMostSigBit, 64> TPointNumLen64;

//#define _CALC_STATISTIC_

	class IXYComressor
	{
	public:
		virtual ~IXYComressor() {}
		IXYComressor(){}

		virtual void PreCompress(const GisXYPoint *pPoint, uint32 nCount) = 0;
		virtual uint32 GetCompressSize() const= 0;
		virtual uint32 GetCount() const = 0;
		virtual uint32 GetPointCount() const = 0;
		virtual void WriteHeader(IWriteStream *pStream) = 0;
		virtual bool  compress(const GisXYPoint *pPoint, uint32 nCount, IWriteStream *pStream) = 0;

		virtual void ReadHeader(IReadStream *pStream) = 0;
		virtual bool  decompress(GisXYPoint *pPoint, uint32 nCount, IReadStream *pStream) = 0;
	};


	template <class _TValue, class _TPointLenCompressor, class _ZOrder>
	class TXYCompressorDiff : public IXYComressor
	{
	public:
		typedef _TPointLenCompressor TPointLenCompressor;
		typedef _TValue TValue;
		typedef _ZOrder ZOrder;


		TXYCompressorDiff(const CGeoShape::compress_params& params) : m_params(params)
		{
			
		}
		~TXYCompressorDiff(){}

		virtual void PreCompress(const GisXYPoint *pPoint, uint32 nCount)
		{
			m_SignCompressor.Init(nCount);

			TValue X = (TValue)((pPoint[0].x + m_params.m_dOffsetX)/m_params.m_dScaleX);
			TValue Y = (TValue)((pPoint[0].y + m_params.m_dOffsetY)/m_params.m_dScaleY);

			ZOrder zOrderPrev(X, Y);
			ZOrder zOrderNext;
			ZOrder zOrderDiff;

			TValue xDiff = 0;
			TValue yDiff = 0;
			for (uint32 i = 1; i < nCount; ++i)
			{

				X = (TValue)((pPoint[i].x + m_params.m_dOffsetX)/m_params.m_dScaleX);
				Y = (TValue)((pPoint[i].y + m_params.m_dOffsetY)/m_params.m_dScaleY);
				zOrderNext.setZOrder(X, Y);
			
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

				zOrderPrev = zOrderNext;
			}
		}

		virtual uint32 GetCompressSize() const
		{
			uint32 nSignSize = m_SignCompressor.GetCompressSize();
			return m_Compressor.GetCompressSize() + (m_Compressor.GetBitsLen() +7)/8 + 2 * sizeof(TValue) + nSignSize;
		}
		virtual void WriteHeader(IWriteStream *pStream)
		{
			m_Compressor.WriteHeader(pStream);
		}

		virtual bool compress(const GisXYPoint *pPoint, uint32 nCount, IWriteStream *pStream)
		{

		
			uint32 nBitSize = (m_Compressor.GetBitsLen() + nCount);
			uint32 nByteSize = (nBitSize + 7)/8;
			CommonLib::FxBitWriteStream bitStream;
						
			bitStream.attach(pStream, pStream->pos(), nByteSize);
			pStream->seek(nByteSize, soFromCurrent);

			m_SignCompressor.InitCompress(pStream);


			TValue X = (TValue)((pPoint[0].x + m_params.m_dOffsetX)/m_params.m_dScaleX);
			TValue Y = (TValue)((pPoint[0].y + m_params.m_dOffsetY)/m_params.m_dScaleY);
			
			ZOrder zOrderPrev(X, Y);
			ZOrder zOrderNext;
			ZOrder zOrderDiff;

			TValue xDiff = 0;
			TValue yDiff = 0;


			pStream->write(X);
			pStream->write(Y);
			m_Compressor.BeginCompreess(pStream);

			for (uint32 i = 1; i < nCount; ++i)
			{

				X = (TValue)((pPoint[i].x + m_params.m_dOffsetX)/m_params.m_dScaleX);
				Y = (TValue)((pPoint[i].y + m_params.m_dOffsetY)/m_params.m_dScaleY);



				zOrderNext.setZOrder(X, Y);

				if(zOrderPrev <  zOrderNext)
				{
					zOrderDiff = zOrderNext - zOrderPrev;
					bitStream.writeBit(false);
					
				}
				else
				{
					zOrderDiff = zOrderPrev - zOrderNext;
					bitStream.writeBit(true);
				}

	
				zOrderDiff.getXY(xDiff, yDiff);
				uint32 xLen =  m_Compressor.EncodeSymbol(xDiff, &bitStream);
				uint32 yLen =  m_Compressor.EncodeSymbol(yDiff, &bitStream);

		
				zOrderPrev = zOrderNext;
			}

			m_Compressor.EncodeFinish();
			

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

	

		void clear()
		{
			m_Compressor.clear();
			this->m_nBitLen = 0;
		}

		virtual void ReadHeader(IReadStream *pStream)
		{
			m_Compressor.ReadHeader(pStream);
		}
	 

		virtual uint32 GetPointCount() const
		{
			return m_Compressor.GetCount()/2 + 1;
		}

		virtual uint32 GetCount() const
		{
			return m_Compressor.GetCount()/2;
		}
		virtual bool decompress(GisXYPoint *pPoint, uint32 nCount, IReadStream *pStream)
		{
			uint32 nBitLen = m_Compressor.GetBitsLen() + nCount;
			uint32 nByteSize = (nBitLen + 7)/8;
			CommonLib::FxBitReadStream bitStream;

			bitStream.attach(pStream, pStream->pos(), nByteSize);
			pStream->seek(nByteSize, soFromCurrent);


			

			TValue X = 0;
			TValue Y = 0;

			pStream->read(X);
			pStream->read(Y);


			pPoint[0].x =  ((double)X *m_params.m_dScaleX) - m_params.m_dOffsetX;  
			pPoint[0].y =  ((double)Y *m_params.m_dScaleY) - m_params.m_dOffsetY;

			ZOrder zOrderNext(X, Y);
			//ZOrder zOrderNext;
			ZOrder zOrderDiff;

			TValue xDiff = 0;
			TValue yDiff = 0;

			uint32 nBitPart = 0;
			uint32 nBitX = 0;
			uint32 nBitY = 0;

			m_Compressor.StartDecode(pStream);
			for (uint32 i = 1; i < nCount; ++i)
			{

				m_Compressor.DecodeSymbol(nBitX);
				m_Compressor.DecodeSymbol(nBitY);


				bool bSign = bitStream.readBit();
				
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
		
				zOrderDiff.setZOrder(xDiff, yDiff);

				if(bSign)
					zOrderNext = zOrderNext - zOrderDiff;
				else
					zOrderNext = zOrderNext + zOrderDiff;


				zOrderNext.getXY(X, Y);
				pPoint[i].x =  ((double)X *m_params.m_dScaleX) - m_params.m_dOffsetX;  
				pPoint[i].y =  ((double)Y *m_params.m_dScaleY) - m_params.m_dOffsetY;

				//zOrderPrev = zOrderNext;
			}
			return true;
		}
	private:

		TPointLenCompressor m_Compressor;
		CGeoShape::compress_params m_params;
		TSignCompressor m_SignCompressor;
	};

	typedef TXYCompressorDiff<uint16, TPointNumLen16, ZOrderPoint2DU16> TXYCompressor16;
	typedef TXYCompressorDiff<uint32, TPointNumLen32, ZOrderPoint2DU32> TXYCompressor32;
	typedef TXYCompressorDiff<uint64, TPointNumLen64, ZOrderPoint2DU64> TXYCompressor64;
}

#endif