#ifndef _EMBEDDED_DATABASE_POINT_ZORDER_COMPRESOR_H_
#define _EMBEDDED_DATABASE_POINT_ZORDER_COMPRESOR_H_

#include "NumLenCompress.h"



namespace embDB
{
	template<class _TCoord, 
	class _TRangeEncoder,
	class _TACEncoder,
	class _TRangeDecoder,
	class _TACDecoder,		
	class _ZOrder, uint32 _nMaxBitsLens>
	class TPointZOrderCompressor : public TUnsignedNumLenCompressor<_TCoord, TFindMostSigBit, _TRangeEncoder,
		_TACEncoder, _TRangeDecoder, _TACDecoder, _nMaxBitsLens>
	
	{
		public:


			typedef _ZOrder ZOrder;
			typedef _TCoord TCoord;

			typedef TUnsignedNumLenCompressor<_TCoord, TFindMostSigBit, _TRangeEncoder,
				_TACEncoder, _TRangeDecoder, _TACDecoder, _nMaxBitsLens> TBase;


			TPointZOrderCompressor(uint32 nError = 200 , bool bOnlineCalcSize = false) :
					TBase(nError, bOnlineCalcSize)
			{
 
			}


			void AddZOrder(const ZOrder& zOrder)
			{
				TCoord x = 0, y = 0;
				zOrder.getXY(x, y);
				this->AddSymbol(x);
				this->AddSymbol(y);
			}

			void RemoveZOrder(const ZOrder& zOrder)
			{
				TCoord x = 0, y = 0;
				zOrder.getXY(x, y);
				this->RemoveSymbol(x);
				this->RemoveSymbol(y);
			}
			uint32 GetCompressSize() const
			{ 
				uint32 nBaseSize = TBase::GetCompressSize();
				return nBaseSize +  ZOrder::SizeInByte;
			}


			void WriteZorder(const ZOrder& zOrder, CommonLib::IWriteStream* pStream)
			{
				TCoord x = 0, y = 0;
				zOrder.getXY(x, y);
				pStream->write(x);
				pStream->write(y);
			}

			void ReadZorder(ZOrder& zOrder, CommonLib::IReadStream* pStream)
			{
				TCoord x = 0, y = 0;
				pStream->read(x);
				pStream->read(y);

				zOrder.setZOrder(x, y);
			}

			bool compress(const TBPVector<ZOrder>& vecValues, CommonLib::IWriteStream* pStream)
			{

				assert(m_nCount == (vecValues.size() - 1) * 2);
			 
				uint32 nBeginPos = pStream->pos();
				byte nFlag = 0;
				pStream->write(nFlag);

				WriteDiffsLens(pStream);

				double dRowBitsLen = GetCodeBitSize();
				uint32 nByteSize = (dRowBitsLen + 7)/8;



				uint32 FreqPrev[_nMaxBitsLens + 1];
				memset(&FreqPrev, 0, sizeof(uint32) *_nMaxBitsLens);

				int32 nPrevF = 0;
				for (uint32 i = 0; i < _nMaxBitsLens ; ++i)
				{

					FreqPrev[i + 1] = m_BitsLensFreq[i] + nPrevF;
					nPrevF = FreqPrev[i + 1];
				}

				CommonLib::FxBitWriteStream bitStream;
				uint32 nBitSize = (this->m_nLenBitSize +7)/8;

				WriteZorder(vecValues[0], pStream);
				pStream->write((uint16)nBitSize);
				bitStream.attach(pStream, pStream->pos(), nBitSize);
				pStream->seek(nBitSize, CommonLib::soFromCurrent);
				uint32 nBeginCompressPos = pStream->pos();
				bool bRangeCode = true;

				if(!CompressRangeCode(vecValues, pStream, FreqPrev, nByteSize, &bitStream))
				{
					bitStream.seek(0, CommonLib::soFromBegin);
					pStream->seek(nBeginCompressPos, CommonLib::soFromBegin);
					CompressAcCode(vecValues, pStream,  FreqPrev,  &bitStream);
					bRangeCode = false;
				}


				uint32 nEndPos = pStream->pos();

				uint32 nCompressSize= nEndPos - nBeginCompressPos;

				if(bRangeCode)
					nFlag |= 0x1;

				pStream->seek(nBeginPos, CommonLib::soFromBegin);

				nFlag |= (((byte)m_nTypeFreq) << 1);
				pStream->write(nFlag);
				pStream->seek(nEndPos, CommonLib::soFromBegin);
				return true;
			}
			bool decompress(TBPVector<ZOrder>& vecValues, CommonLib::IReadStream* pStream)
			{

				byte nFlag = pStream->readByte();
				bool bRangeCode = nFlag & 0x01;
				m_nTypeFreq = (eTypeFreq)(nFlag>>1);
				ReadDiffsLens(pStream);
				CalcRowBitSize();



				uint32 FreqPrev[_nMaxBitsLens + 1];
				memset(&FreqPrev, 0, sizeof(uint32) * _nMaxBitsLens);

				int32 nPrevF = 0;
				for (uint32 i = 0; i < _nMaxBitsLens; ++i)
				{

					FreqPrev[i + 1] = m_BitsLensFreq[i] + nPrevF;
					nPrevF = FreqPrev[i + 1];
				}

				ZOrder zBegin;
				ReadZorder(zBegin, pStream);
				vecValues.push_back(zBegin);
				uint16 nBitSize = pStream->readintu16();

				CommonLib::FxBitReadStream bitStream;


				bitStream.attach(pStream, pStream->pos(), nBitSize);
				pStream->seek(nBitSize, CommonLib::soFromCurrent);

				if(bRangeCode)
					return Decompress<TRangeDecoder>(vecValues, pStream, FreqPrev, &bitStream, zBegin);
				else
					return Decompress<TACDecoder>(vecValues, pStream, FreqPrev, &bitStream, zBegin);


			}

			uint32 count() const
			{
				return m_nCount;
			}
	protected:

		bool CompressRangeCode(const TBPVector<ZOrder>& vecValues, CommonLib::IWriteStream* pStream, uint32 *FreqPrev, uint32 nMaxByteSize,
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

		template<class TCompressor>
		bool compressCoord(TCompressor& compressor, CommonLib::FxBitWriteStream *pBitStream, TCoord coord, uint32 *FreqPrev)
		{
		 	uint16 nBitLen =  m_FindBit.FMSB(coord);
			assert(m_BitsLensFreq[nBitLen] != 0);
			pBitStream->writeBits(coord, nBitLen);
			return compressor.EncodeSymbol(FreqPrev[nBitLen], FreqPrev[nBitLen + 1], m_nCount);
		}

		template<class TDecoder>
		void decompressCoord(TDecoder& decoder, CommonLib::FxBitReadStream *pBitStream, TCoord coord, uint32 *FreqPrev)
		{

			TCoord value = 0;
			uint32 freq = decoder.GetFreq(m_nCount);

			int32 nBitLen = CommonLib::upper_bound(FreqPrev, _nMaxBitsLens, freq);
			if(nBitLen != 0)
				nBitLen--;
			
			pBitStream->readBits(coord, nBitLen);
			decoder.DecodeSymbol(FreqPrev[nBitLen], FreqPrev[nBitLen + 1], m_nCount);
		 
		}


		template<class TCompressor>
		bool compressZOrder(TCompressor& compressor, CommonLib::FxBitWriteStream *pBitStream, const ZOrder& zOrder, uint32 *FreqPrev)
		{
			TCoord x = 0, y = 0;
			zOrder.getXY(x, y);

			if(!compressCoord(compressor, pBitStream, x, FreqPrev))
				return false;

			return compressCoord(compressor, pBitStream, y, FreqPrev);
		}



		template<class TCompressor>
		void DecompressZOrder(TCompressor& compressor, CommonLib::FxBitReadStream *pBitStream, ZOrder& zOrder, uint32 *FreqPrev)
		{
			TCoord x = 0, y = 0;
			decompressCoord(compressor, pBitStream, x, FreqPrev);
			decompressCoord(compressor, pBitStream, y, FreqPrev);
			zOrder.setZOrder(x, y);
		}

		template<class TDecoder>
		bool Decompress(TBPVector<ZOrder>& vecValues, CommonLib::IReadStream* pStream, uint32 *FreqPrev, 
			CommonLib::FxBitReadStream *pBitStream, ZOrder zBegin)
		{
			TDecoder decoder(pStream);
			decoder.StartDecode();
			ZOrder zValue;
			for (size_t i = 0; i < m_nCount/2; ++i)
			{
				DecompressZOrder(decoder, pBitStream,zValue,  FreqPrev);
				zBegin += zValue;
				vecValues.push_back(zBegin);
			}
			return true;
		}
		protected:
 
	};
	
}

#endif