#ifndef _LIB_COMMON_GEO_SHAPE_XY_COMPRESSOR_H_
#define _LIB_COMMON_GEO_SHAPE_XY_COMPRESSOR_H_
#include "NumLenCompressor.h"
#include "BitStream.h"
#include "GeoShape.h"
#include "PointZOrder.h"
namespace CommonLib
{


	typedef TNumLemCompressor<uint16, TFindMostSigBit, 16> TPointNumLen16;
	typedef TNumLemCompressor<uint32, TFindMostSigBit, 32> TPointNumLen32;
	typedef TNumLemCompressor<uint64, TFindMostSigBit, 64> TPointNumLen64;

	template <class _TValue, class _TPointLenCompressor, class _ZOrder>
	class TXYCompressor
	{
	public:
		typedef _TPointLenCompressor TPointLenCompressor;
		typedef _TValue TValue;
		typedef _ZOrder ZOrder;


		TXYCompressor(const CGeoShape::compress_params& params) : m_params(params), m_nSignBits(0)
		{
			
		}
		~TXYCompressor(){}

		void PreCompressPart(const GisXYPoint *pPoint, uint32 nCount)
		{


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
					zOrderDiff = zOrderNext - zOrderPrev;
				else
					zOrderDiff = zOrderPrev - zOrderNext;

				zOrderDiff.getXY(xDiff, yDiff);
				m_Compressor.PreAddSympol(xDiff);
				m_Compressor.PreAddSympol(yDiff);
 
			}
		}


		bool compress(const GisXYPoint *pPoint, uint32 nCount, IWriteStream *pStream)
		{

			PreCompressPart(pPoint, nCount);
	
			uint32 nBitLen = m_Compressor.GetLenBits() + nCount;
			uint32 nByteSize = (nBitLen + 7)/8;
			CommonLib::FxBitWriteStream bitStream;

			m_Compressor.BeginCompreess(pStream);
			bitStream.attach(pStream, pStream->pos(), nByteSize);
			pStream->seek(nByteSize, soFromCurrent);


			TValue X = (TValue)((pPoint[0].x + m_params.m_dOffsetX)/m_params.m_dScaleX);
			TValue Y = (TValue)((pPoint[0].y + m_params.m_dOffsetY)/m_params.m_dScaleY);

			ZOrder zOrderPrev(X, Y);
			ZOrder zOrderNext;
			ZOrder zOrderDiff;

			TValue xDiff = 0;
			TValue yDiff = 0;


			pStream->write(X);
			pStream->write(Y);
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
				m_Compressor.EncodeSymbol(xDiff, &bitStream);
				m_Compressor.EncodeSymbol(yDiff, &bitStream);

				//zOrderPrev = zOrderNext;
			}

			m_Compressor.EncodeFinish();
			return true;
		}

	

		void clear()
		{
			m_Compressor.clear();
			m_nSignBits = 0;
		}


		bool BeginDecompress(IWriteStream *pStream)
		{
			m_Compressor.BeginDecode(pStream);
			return true;
		}

		uint32 GetCount() const
		{
			return m_Compressor.count()/2;
		}
		bool decompress(GisXYPoint *pPoint, uint32 nCount, IReadStream *pStream)
		{
			uint32 nBitLen = m_Compressor.GetLenBits() + nCount;
			uint32 nByteSize = (nBitLen + 7)/8;
			CommonLib::FxBitReadStream bitStream;

			m_Compressor.BeginCompreess(pStream);
			bitStream.attach(pStream, pStream->pos(), nByteSize);
			pStream->seek(soFromCurrent, nByteSize);

			TValue X = 0;
			TValue Y = 0;

			pStream->read(X);
			pStream->read(Y);


			pPoint[0].x =  ((double)X *m_params.m_dScaleX) - m_params.m_dOffsetX;  
			pPoint[0].y =  ((double)Y *m_params.m_dScaleY) - m_params.m_dOffsetY;

			ZOrder zOrderPrev(X, Y);
			ZOrder zOrderNext;
			ZOrder zOrderDiff;

			TValue xDiff = 0;
			TValue yDiff = 0;

			uint32 nBitPart = 0;
			uint32 nBitX = 0;
			uint32 nBitY = 0;
			for (uint32 i = 1; i < nCount; ++i)
			{

				m_Compressor.DecodeSymbol(nBitX);
				m_Compressor.DecodeSymbol(nBitY);

				bool bSign = bitStream.readBit();
				bitStream.readBits(xDiff, nBitY);
				bitStream.readBits(yDiff, nBitY);

				zOrderDiff.setXY(xDiff, yDiff);

				if(bSign)
					zOrderNext = zOrderPrev - zOrderDiff;
				else
					zOrderNext = zOrderPrev + zOrderDiff;


				zOrderNext.getXY(X, Y);
				pPoint[i].x =  ((double)X *m_params.m_dScaleX) - m_params.m_dOffsetX;  
				pPoint[i].y =  ((double)Y *m_params.m_dScaleY) - m_params.m_dOffsetY;

			}
		}
	private:

		TPointLenCompressor m_Compressor;
		CGeoShape::compress_params m_params;
		uint32 m_nSignBits;
	};

	typedef TXYCompressor<uint16, TPointNumLen16, ZOrderPoint2DU16> TXYCompressor16;
	typedef TXYCompressor<uint32, TPointNumLen32, ZOrderPoint2DU32> TXYCompressor32;
	typedef TXYCompressor<uint64, TPointNumLen64, ZOrderPoint2DU64> TXYCompressor64;
}

#endif