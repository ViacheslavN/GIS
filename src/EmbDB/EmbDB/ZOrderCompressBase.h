#ifndef _EMBEDDED_DATABASE_BASE_ZORDER_COMPRESOR_H_
#define _EMBEDDED_DATABASE_BASE_ZORDER_COMPRESOR_H_

#include "NumLenCompress.h"
#include "CommonLibrary/RangeCoder.h"
#include "CommonLibrary/ArithmeticCoder.h"
#include "SignCompressor.h"
namespace embDB
{
	template<class _ZOrder, class _TPointType, uint32 _nMaxBitsLens, uint32 _nPointNum>
	class TBaseZOrderCompressor : public TUnsignedNumLenCompressor<_TPointType, TFindMostSigBit, CommonLib::TRangeEncoder<uint64, 64>,
						CommonLib::TACEncoder<uint64, 32>, CommonLib::TRangeDecoder<uint64, 64> , CommonLib::TACDecoder<uint64, 32>, _nMaxBitsLens>
	
	{
		public:


			typedef _ZOrder ZOrder;
			typedef _TPointType TPointType;

			typedef TUnsignedNumLenCompressor<TPointType, TFindMostSigBit, CommonLib::TRangeEncoder<uint64, 64>,
						CommonLib::TACEncoder<uint64, 32>, CommonLib::TRangeDecoder<uint64, 64> , CommonLib::TACDecoder<uint64, 32>, _nMaxBitsLens> TBase;


			typedef CommonLib::BaseACEncoder<uint64> TEncoder;
			typedef CommonLib::BaseACDecoder<uint64> TDecoder;

			TBaseZOrderCompressor(uint32 nError = 200 , bool bOnlineCalcSize = false) :
					TBase(nError, bOnlineCalcSize)
			{
 
			}


			uint32 GetCompressSize() const
			{ 
				uint32 nBaseSize = TBase::GetCompressSize();
				return nBaseSize +  ZOrder::SizeInByte;
			}

			virtual void WriteZorder(const ZOrder& zOrder, CommonLib::IWriteStream* pStream) = 0;
			virtual void ReadZorder(ZOrder& zOrder, CommonLib::IReadStream* pStream) = 0;


			 
			virtual bool compressZOrder(TEncoder* pEncoder, CommonLib::FxBitWriteStream *pBitStream, const ZOrder& zOrder, uint32 *FreqPrev)= 0;
			virtual void DecompressZOrder(TDecoder* pDecoder, CommonLib::FxBitReadStream *pBitStream, ZOrder& zOrder, uint32 *FreqPrev)= 0;


		
			bool compress(const TBPVector<ZOrder>& vecValues, CommonLib::IWriteStream* pStream)
			{

				assert(m_nCount == (vecValues.size() - 1) * _nPointNum);
			 
				uint32 nBeginPos = pStream->pos();
				byte nFlag = 0;
				pStream->write(nFlag);

				this->WriteDiffsLens(pStream);

				double dRowBitsLen = this->GetCodeBitSize();
				uint32 nByteSize = (dRowBitsLen + 7)/8;



				uint32 FreqPrev[_nMaxBitsLens + 1];
				memset(&FreqPrev, 0, sizeof(uint32) *_nMaxBitsLens);

				int32 nPrevF = 0;
				for (uint32 i = 0; i < _nMaxBitsLens ; ++i)
				{

					FreqPrev[i + 1] = this->m_BitsLensFreq[i] + nPrevF;
					nPrevF = FreqPrev[i + 1];
				}

				CommonLib::FxBitWriteStream bitStream;
				uint32 nBitSize = (this->m_nLenBitSize +7)/8;

				WriteZorder(vecValues[0], pStream);
			//	pStream->write((uint16)nBitSize);
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

				nFlag |= (((byte)this->m_nTypeFreq) << 1);
				pStream->write(nFlag);
				pStream->seek(nEndPos, CommonLib::soFromBegin);
				return true;
			}
			bool decompress(TBPVector<ZOrder>& vecValues, CommonLib::IReadStream* pStream)
			{
				this->clear();
				byte nFlag = pStream->readByte();
				bool bRangeCode = nFlag & 0x01;
				this->m_nTypeFreq = (eCompressDataType)(nFlag>>1);
				this->ReadDiffsLens(pStream);
				//this->CalcRowBitSize();



				uint32 FreqPrev[_nMaxBitsLens + 1];
				memset(&FreqPrev, 0, sizeof(uint32) * _nMaxBitsLens);

				int32 nPrevF = 0;
				for (uint32 i = 0; i < _nMaxBitsLens; ++i)
				{

					FreqPrev[i + 1] = this->m_BitsLensFreq[i] + nPrevF;
					nPrevF = FreqPrev[i + 1];
				}

				ZOrder zBegin;
				ReadZorder(zBegin, pStream);
				vecValues.push_back(zBegin);
				//uint16 nBitSize = pStream->readintu16();
				uint32 nBitSize = (this->m_nLenBitSize +7)/8;
				CommonLib::FxBitReadStream bitStream;


				bitStream.attach(pStream, pStream->pos(), nBitSize);
				pStream->seek(nBitSize, CommonLib::soFromCurrent);

				if(bRangeCode)
					return Decompress<typename TBase::TRangeDecoder>(vecValues, pStream, FreqPrev, &bitStream, zBegin);
				else
					return Decompress<typename TBase::TACDecoder>(vecValues, pStream, FreqPrev, &bitStream, zBegin);


			}

			uint32 count() const
			{
				return this->m_nCount;
			}
	protected:

		bool CompressRangeCode(const TBPVector<ZOrder>& vecValues, CommonLib::IWriteStream* pStream, uint32 *FreqPrev, uint32 nMaxByteSize,
			CommonLib::FxBitWriteStream *pBitStream)
		{

			typename TBase::TRangeEncoder rgEncoder(pStream, nMaxByteSize);

			for (uint32 i = 1, sz = vecValues.size(); i< sz; ++i)
			{
				ZOrder zDiff = vecValues[i] - vecValues[i - 1];				 
				if(!compressZOrder(&rgEncoder, pBitStream, zDiff, FreqPrev))
					return false;
			}


			return rgEncoder.EncodeFinish();
		}

		void CompressAcCode(const TBPVector<ZOrder>& vecValues, CommonLib::IWriteStream* pStream,
			uint32 *FreqPrev, CommonLib::FxBitWriteStream *pBitStream)
		{
			typename  TBase::TACEncoder acEncoder(pStream);

			for (uint32 i = 1, sz = vecValues.size(); i< sz; ++i)
			{
				ZOrder zDiff = vecValues[i] - vecValues[i - 1];
				compressZOrder(&acEncoder, pBitStream, zDiff, FreqPrev);

			}


			acEncoder.EncodeFinish();
		}

	 
		bool compressCoord(TEncoder *pEncoder, CommonLib::FxBitWriteStream *pBitStream, TPointType coord, uint32 *FreqPrev)
		{
		 	uint16 nBitLen =  this->m_FindBit.FMSB(coord);
			assert(this->m_BitsLensFreq[nBitLen] != 0);
			if(nBitLen > 1)
				pBitStream->writeBits(coord, nBitLen - 1);
			return pEncoder->EncodeSymbol(FreqPrev[nBitLen], FreqPrev[nBitLen + 1], this->m_nCount);
		}

	 
		void decompressCoord(TDecoder* pDecoder, CommonLib::FxBitReadStream *pBitStream, TPointType& coord, uint32 *FreqPrev)
		{

			uint32 freq = pDecoder->GetFreq(this->m_nCount);

			int32 nBitLen = CommonLib::upper_bound(FreqPrev, _nMaxBitsLens + 1, freq);
			if(nBitLen != 0)
				nBitLen--;

			coord = nBitLen;

			if(coord > 1)
			{
				coord = 0;
				pBitStream->readBits(coord, nBitLen - 1);
				coord |= 1 << nBitLen - 1;
			}
			
		
			pDecoder->DecodeSymbol(FreqPrev[nBitLen], FreqPrev[nBitLen + 1], this->m_nCount);
		 
		}


		template<class TDecoder>
		bool Decompress(TBPVector<ZOrder>& vecValues, CommonLib::IReadStream* pStream, uint32 *FreqPrev, 
			CommonLib::FxBitReadStream *pBitStream, ZOrder zBegin)
		{
			TDecoder decoder(pStream);
			decoder.StartDecode();
			ZOrder zValue;
			for (uint32 i = 0, sz = this->m_nCount/_nPointNum; i < sz; ++i)
			{
				DecompressZOrder(&decoder, pBitStream,zValue,  FreqPrev);
				zBegin += zValue;
				vecValues.push_back(zBegin);
			}
			return true;
		}
		protected:
 
	};
	
}

#endif