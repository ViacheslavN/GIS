#ifndef _EMBEDDED_DATABASE_RECT_ZORDER_COMPRESOR_H_
#define _EMBEDDED_DATABASE_RECT_ZORDER_COMPRESOR_H_


#include "PointZOrderCompressor.h"


namespace embDB
{
	template<class _ZOrder, 
	class _TCoord, uint32 _nMaxBitsLens>
	class TRectZOrderCompressor : public TBaseZOrderCompressor<_ZOrder,_TCoord, _nMaxBitsLens, 4>
	
	{
		public:


			typedef _ZOrder ZOrder;
			typedef _TCoord TCoord;


			typedef TBaseZOrderCompressor<_ZOrder,_TCoord, _nMaxBitsLens, 4> TBase;


			typedef typename TBase::TEncoder TEncoder;
			typedef typename TBase::TDecoder TDecoder;


			TRectZOrderCompressor(uint32 nError = 200 , bool bOnlineCalcSize = false) :
					TBase(nError, bOnlineCalcSize)
			{
 
			}
			void AddZOrder(const ZOrder& zOrder)
			{
				TCoord xMin = 0, yMin = 0, xMax = 0, yMax = 0;
				zOrder.getXY(xMin, yMin, xMax, yMax);
				this->AddSymbol(xMin);
				this->AddSymbol(yMin);
				this->AddSymbol(xMax);
				this->AddSymbol(yMax);
			}

			void RemoveZOrder(const ZOrder& zOrder)
			{
				TCoord xMin = 0, yMin = 0, xMax = 0, yMax = 0;
				zOrder.getXY(xMin, yMin, xMax, yMax);
				this->RemoveSymbol(xMin);
				this->RemoveSymbol(yMin);
				this->RemoveSymbol(xMax);
				this->RemoveSymbol(yMax);
			}
		
			void WriteZorder(const ZOrder& zOrder, CommonLib::IWriteStream* pStream)
			{
				TCoord xMin = 0, yMin = 0, xMax = 0, yMax = 0;
				zOrder.getXY(xMin, yMin, xMax, yMax);
				pStream->write(xMin);
				pStream->write(yMin);
				pStream->write(xMax);
				pStream->write(yMax);
			}

			void ReadZorder(ZOrder& zOrder, CommonLib::IReadStream* pStream)
			{
				TCoord xMin = 0, yMin = 0, xMax = 0, yMax = 0;
				pStream->read(xMin);
				pStream->read(yMin);
				pStream->read(xMax);
				pStream->read(yMax);

				zOrder.setZOrder(xMin, yMin, xMax, yMax);
			}
			
		virtual bool compressZOrder(TEncoder* pEncoder, CommonLib::FxBitWriteStream *pBitStream, const ZOrder& zOrder, uint32 *FreqPrev)
		{
			TCoord xMin = 0, yMin = 0, xMax  = 0, yMax = 0;
			zOrder.getXY(xMin, yMin, xMax, yMax);

			if(!compressCoord(pEncoder, pBitStream, xMin, FreqPrev))
				return false;
			if(!compressCoord(pEncoder, pBitStream, yMin, FreqPrev))
				return false;
			if(!compressCoord(pEncoder, pBitStream, xMax, FreqPrev))
				return false;
			return compressCoord(pEncoder, pBitStream, yMax, FreqPrev);
		}



	 
		virtual void DecompressZOrder(TDecoder* pDecoder, CommonLib::FxBitReadStream *pBitStream, ZOrder& zOrder, uint32 *FreqPrev)
		{
			TCoord xMin = 0, yMin = 0, xMax  = 0, yMax = 0;
			decompressCoord(pDecoder, pBitStream, xMin, FreqPrev);
			decompressCoord(pDecoder, pBitStream, yMin, FreqPrev);
			decompressCoord(pDecoder, pBitStream, xMax, FreqPrev);
			decompressCoord(pDecoder, pBitStream, yMax, FreqPrev);
			zOrder.setZOrder(xMin, yMin, xMax, yMax);
		}



		/*bool CompressRangeCode(const TBPVector<ZOrder>& vecValues, CommonLib::IWriteStream* pStream, uint32 *FreqPrev, uint32 nMaxByteSize,
			CommonLib::FxBitWriteStream *pBitStream)
		{

			TRangeEncoder rgEncoder(pStream, nMaxByteSize);

			for (uint32 i = 1, sz = vecValues.size(); i< sz; ++i)
			{
				ZOrder zDiff = vecValues[i] - vecValues[i - 1];				 
				if(!compressZOrder(rgEncoder, pBitStream, zDiff, FreqPrev))
					return false;
			}


			return rgEncoder.EncodeFinish();
		}

		void CompressAcCode(const TBPVector<ZOrder>& vecValues, CommonLib::IWriteStream* pStream,
			uint32 *FreqPrev, CommonLib::FxBitWriteStream *pBitStream)
		{
			TACEncoder acEncoder(pStream);

			for (uint32 i = 1, sz = vecValues.size(); i< sz; ++i)
			{
				ZOrder zDiff = vecValues[i] - vecValues[i - 1];
				compressZOrder(acEncoder, pBitStream, zDiff, FreqPrev);

			}


			acEncoder.EncodeFinish();
		}

		template<class TDecoder>
		bool Decompress(TBPVector<ZOrder>& vecValues, CommonLib::IReadStream* pStream, uint32 *FreqPrev, 
			CommonLib::FxBitReadStream *pBitStream, ZOrder zBegin)
		{
			TDecoder decoder(pStream);
			decoder.StartDecode();
			ZOrder zValue;
			for (uint32 i = 0, sz = m_nCount/4; i < sz; ++i)
			{
				DecompressZOrder(decoder, pBitStream,zValue,  FreqPrev);
				zBegin += zValue;
				vecValues.push_back(zBegin);
			}
			return true;
		}

		template<class TCompressor>
		bool compressCoord(TCompressor& compressor, CommonLib::FxBitWriteStream *pBitStream, TCoord coord, uint32 *FreqPrev)
		{
			uint16 nBitLen =  m_FindBit.FMSB(coord);
			assert(m_BitsLensFreq[nBitLen] != 0);
			pBitStream->writeBits(coord, nBitLen);
			return compressor.EncodeSymbol(FreqPrev[nBitLen], FreqPrev[nBitLen + 1], m_nCount);
		}

		template<class TDecoder>
		void decompressCoord(TDecoder& decoder, CommonLib::FxBitReadStream *pBitStream, TCoord& coord, uint32 *FreqPrev)
		{

			TCoord value = 0;
			uint32 freq = decoder.GetFreq(m_nCount);

			int32 nBitLen = CommonLib::upper_bound(FreqPrev, _nMaxBitsLens, freq);
			if(nBitLen != 0)
				nBitLen--;



			pBitStream->readBits(coord, nBitLen);
			decoder.DecodeSymbol(FreqPrev[nBitLen], FreqPrev[nBitLen + 1], m_nCount);

		}*/

	 
	};
	
}

#endif