#ifndef _EMBEDDED_DATABASE_NUM_LEN_DIFF_COMPRESS_H_
#define _EMBEDDED_DATABASE_NUM_LEN_DIFF_COMPRESS_H_

#include "CommonLibrary/FixedBitStream.h"
#include <map>
#include "MathUtils.h"
#include "BPVector.h"
#include "NumLenCompress.h"
namespace embDB
{
	

	template<class _TValue, class _TFindMostSigBit, 
			class _TRangeEncoder,
			class _TACEncoder,
			class _TRangeDecoder,
			class _TACDecoder,		
			uint32 _nMaxBitsLens>
	class TUnsignedDiffNumLenCompressor : public TUnsignedNumLenCompressor<_TValue, _TFindMostSigBit, _TRangeEncoder,
			_TACEncoder, _TRangeDecoder, _TACDecoder, _nMaxBitsLens>
	{
		public:


			typedef TUnsignedNumLenCompressor<_TValue, _TFindMostSigBit, _TRangeEncoder,
				_TACEncoder, _TRangeDecoder, _TACDecoder, _nMaxBitsLens> TBase;

			typedef typename TBase::TValue TValue;
			typedef typename TBase::TFindBit TFindBit;
			typedef typename TBase::TRangeEncoder   TRangeEncoder;
			typedef typename TBase::TACEncoder		 TACEncoder;
			typedef typename TBase::TRangeDecoder	 TRangeDecoder;
			typedef typename TBase::TACDecoder		 TACDecoder;


			TUnsignedDiffNumLenCompressor(uint32 nError = 200 /*0.5%*/, bool bOnlineCalcSize = false) : TBase(nError, bOnlineCalcSize)
			{
			}

			uint32 GetCompressSize() const
			{ 
				uint32 nBaseSize = TBase::GetCompressSize();
				return nBaseSize +  sizeof(TValue);
			}

			bool compress(const TBPVector<TValue>& vecValues, CommonLib::IWriteStream* pStream)
			{

				assert(m_nCount == (vecValues.size() - 1));
			 	uint32 nBeginPos = pStream->pos();
				byte nFlag = 0;
				pStream->write(nFlag);

				this->WriteDiffsLens(pStream);

				double dRowBitsLen = this->GetCodeBitSize();
				uint32 nByteSize = (dRowBitsLen + 7)/8;

				

				uint32 FreqPrev[_nMaxBitsLens + 1];
				memset(&FreqPrev, 0, sizeof(uint32) * _nMaxBitsLens);

				int32 nPrevF = 0;
				for (uint32 i = 0; i < _nMaxBitsLens; ++i)
				{

					FreqPrev[i + 1] = this->m_BitsLensFreq[i] + nPrevF;
					nPrevF = FreqPrev[i + 1];
				}

				CommonLib::FxBitWriteStream bitStream;
				uint32 nBitSize = (this->m_nLenBitSize +7)/8;
				
				pStream->write(vecValues[0]);
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

				nFlag |= (((byte)this->m_nTypeFreq) << 1);
				pStream->write(nFlag);
				pStream->seek(nEndPos, CommonLib::soFromBegin);
				return true;
			}
			bool decompress(uint32 nSize,  TBPVector<_TValue>& vecValues, CommonLib::IReadStream* pStream)
			{
				
				byte nFlag = pStream->readByte();
				bool bRangeCode = nFlag & 0x01;
				this->m_nTypeFreq = (typename TBase::eTypeFreq)(nFlag>>1);
				this->ReadDiffsLens(pStream);
				this->CalcRowBitSize();

		 

				uint32 FreqPrev[_nMaxBitsLens + 1];
				memset(&FreqPrev, 0, sizeof(uint32) * _nMaxBitsLens);

				int32 nPrevF = 0;
				for (uint32 i = 0; i < _nMaxBitsLens; ++i)
				{

					FreqPrev[i + 1] = this->m_BitsLensFreq[i] + nPrevF;
					nPrevF = FreqPrev[i + 1];
				}

				TValue nBegin = 0;
				pStream->read(nBegin);
				vecValues.push_back(nBegin);
				uint16 nBitSize = pStream->readintu16();

				CommonLib::FxBitReadStream bitStream;

		 
				bitStream.attach(pStream, pStream->pos(), nBitSize);
				pStream->seek(nBitSize, CommonLib::soFromCurrent);

				if(bRangeCode)
					return Decompress<TRangeDecoder>(vecValues, pStream, FreqPrev, &bitStream, nBegin);
				else
					return Decompress<TACDecoder>(vecValues, pStream, FreqPrev, &bitStream, nBegin);

		 
			}
		private:

			bool CompressRangeCode(const TBPVector<_TValue>& vecValues, CommonLib::IWriteStream* pStream, uint32 *FreqPrev, uint32 nMaxByteSize,
				CommonLib::FxBitWriteStream *pBitStream)
			{

				TRangeEncoder rgEncoder(pStream, nMaxByteSize);

				for (uint32 i = 1, sz = vecValues.size(); i< sz; ++i)
				{
				
					_TValue nDiff = vecValues[i] - vecValues[i - 1];
					uint16 nBitLen =  this->m_FindBit.FMSB(nDiff);

					assert(this->m_BitsLensFreq[nBitLen] != 0);


					pBitStream->writeBits(nDiff, nBitLen + 1);
					if(!rgEncoder.EncodeSymbol(FreqPrev[nBitLen], FreqPrev[nBitLen + 1], this->m_nCount))
						return false;
				}
				

				return rgEncoder.EncodeFinish();
			}

			void CompressAcCode(const TBPVector<_TValue>& vecValues, CommonLib::IWriteStream* pStream,
				uint32 *FreqPrev, CommonLib::FxBitWriteStream *pBitStream)
			{
				TACEncoder acEncoder(pStream);

				for (uint32 i = 1, sz = vecValues.size(); i< sz; ++i)
				{
					_TValue nDiff = vecValues[i] - vecValues[i - 1];
					uint16 nBitLen =  this->m_FindBit.FMSB(nDiff);
			 

					pBitStream->writeBits(nDiff, nBitLen + 1);
					acEncoder.EncodeSymbol(FreqPrev[nBitLen], FreqPrev[nBitLen + 1], this->m_nCount);
						
				}


				acEncoder.EncodeFinish();
			}

			template<class TDecoder>
			bool Decompress(TBPVector<_TValue>& vecValues, CommonLib::IReadStream* pStream, uint32 *FreqPrev, 
				CommonLib::FxBitReadStream *pBitStream, TValue nBegin)
			{


				TDecoder decoder(pStream);
				decoder.StartDecode();
				TValue value = 0;
				for (uint32 i = 0; i < this->m_nCount; ++i)
				{
					uint32 freq = decoder.GetFreq(this->m_nCount);

					//uint32 nBitLen;
					//for(nBitLen = _nMaxBitsLens;FreqPrev[nBitLen] > freq;nBitLen--);
					int32 nBitLen = CommonLib::upper_bound(FreqPrev, _nMaxBitsLens, freq);
					if(nBitLen != 0)
						nBitLen--;

					
					pBitStream->readBits(value, nBitLen + 1);
					nBegin += value;
					vecValues.push_back(nBegin);

					decoder.DecodeSymbol(FreqPrev[nBitLen], FreqPrev[nBitLen+1], this->m_nCount);

				}

				return true;
			}
	};

	typedef TUnsignedDiffNumLenCompressor<int64, TFindMostSigBit, CommonLib::TRangeEncoder64, CommonLib::TACEncoder64, 
		CommonLib::TRangeDecoder64, CommonLib::TACDecoder32, 64> UnsignedDiffNumLenCompressor64i;

	typedef TUnsignedDiffNumLenCompressor<int32, TFindMostSigBit, CommonLib::TRangeEncoder64, CommonLib::TACEncoder64, 
		CommonLib::TRangeDecoder64, CommonLib::TACDecoder32, 32>  UnsignedDiffNumLenCompressor32i;
}
#endif