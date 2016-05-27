#ifndef _EMBEDDED_DATABASE_SIGNED_NUM_LEN_DIFF_COMPRESS_H_
#define _EMBEDDED_DATABASE_SIGNED_NUM_LEN_DIFF_COMPRESS_H_

#include "CommonLibrary/FixedBitStream.h"
#include <map>
#include <set>
#include "MathUtils.h"
#include "BPVector.h"
#include "NumLenCompress.h"
#include "CommonLibrary/RangeCoder.h"
#include "CommonLibrary/ArithmeticCoder.h"
#include "CommonLibrary/FileStream.h"
#include "SignedNumLenDiffCompress.h"
#include "SignCompressor.h"
namespace embDB
{



	//static int bits_lens[] = {1, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4};
	struct TSignedFindMostSigBit
	{
		int FMSB(uint16 val16)
		{
			int bits = 0;
			if(val16 > 0xff){
				bits += 8;
				val16 >>= 8;
			}
			if(val16 > 0xf){
				bits += 4;
				val16 >>= 4;
			}
			bits += bits_lens[val16];
			return bits;
		}


		int FMSB(uint32 val32)
		{
			int bits = 0;
		 
			if(val32 > 0xffff){
				bits = 16;
				val32 >>= 16;
			}
			if(val32 > 0xff){
				bits += 8;
				val32 >>= 8;
			}
			if(val32 > 0xf){
				bits += 4;
				val32 >>= 4;
			}
			bits += bits_lens[val32];
			return bits;
		}


		int FMSB(uint64 val64)
		{
			int bits = 0;
			uint32 val32;

			if(val64 > 0xffffffff)
			{
				val32 = (uint32)(val64 >> 32);
				bits = 32;
			}
			else
				val32 = (unsigned int)val64;
			bits += FMSB(val32);
			return bits;
		}

		int FMSB(int64 val64)
		{
			 return FMSB(uint64(val64 < 0 ? -val64 : val64));
		}
		int FMSB(int32 val32)
		{
			return FMSB(uint32(val32 < 0 ? -val32 : val32));
		}
		int FMSB(int16 val16)
		{
			return FMSB(uint16(val16 < 0 ? -val16 : val16));
		}
	};


	template<class _TValue,class _TSignValue,
	class _TRangeEncoder,
	class _TACEncoder,
	class _TRangeDecoder,
	class _TACDecoder,		
		uint32 _nMaxBitsLens>
	class TSignedDiffNumLenCompressor : public TUnsignedNumLenCompressor<_TValue, TSignedFindMostSigBit, _TRangeEncoder,
		_TACEncoder, _TRangeDecoder, _TACDecoder, _nMaxBitsLens>
	{
	public:


		typedef TUnsignedNumLenCompressor<_TValue, TSignedFindMostSigBit, _TRangeEncoder,
			_TACEncoder, _TRangeDecoder, _TACDecoder, _nMaxBitsLens> TBase;

		typedef typename TBase::TValue TValue;
		typedef _TSignValue  TSignValue;
		typedef typename TBase::TFindBit TFindBit;
		typedef typename TBase::TRangeEncoder   TRangeEncoder;
		typedef typename TBase::TACEncoder		 TACEncoder;
		typedef typename TBase::TRangeDecoder	 TRangeDecoder;
		typedef typename TBase::TACDecoder		 TACDecoder;


		TSignedDiffNumLenCompressor(uint32 nError = 200 /*0.5%*/, bool bOnlineCalcSize = false) : TBase(nError, bOnlineCalcSize)
		{
		}
		uint16 AddSymbol(TSignValue symbol)
		{

			assert(m_SignCompressor.count() == this->m_nCount);
			m_SignCompressor.AddSymbol(symbol < 0);
	 		return TBase::AddSymbol(symbol < 0 ? TValue(-1*symbol) : symbol);
		}
		void RemoveSymbol(TSignValue symbol)
		{
			assert(m_SignCompressor.count() == this->m_nCount);
			m_SignCompressor.RemoveSymbol(symbol< 0);
			TBase::RemoveSymbol(symbol < 0 ? TValue(-1*symbol) : symbol);
		}
		uint32 GetCompressSize() const
		{ 
			uint32 nBaseSize = TBase::GetCompressSize();
			uint32 nSignSize = m_SignCompressor.GetCompressSize();
			
			return nBaseSize +  sizeof(TValue) + nSignSize;
		}
		void clear()
		{
			TBase::clear();
			m_SignCompressor.clear();
		}
		bool compress(const TBPVector<TValue>& vecValues, CommonLib::IWriteStream* pStream)
		{

			assert(this->m_nCount == (vecValues.size() - 1));
			assert(m_SignCompressor.count() == this->m_nCount);
			uint32 nBeginPos = pStream->pos();
			byte nFlag = 0;
			pStream->write(nFlag);

			this->WriteDiffsLens(pStream);

			double dRowBitsLen = this->GetCodeBitSize();
			uint32 nByteSize = (dRowBitsLen + 7)/8;



			uint32 FreqPrev[_nMaxBitsLens + 1 + 1];
			memset(FreqPrev, 0, sizeof(FreqPrev));

			int32 nPrevF = 0;
			for (uint32 i = 0; i < _nMaxBitsLens + 1; ++i)
			{

				FreqPrev[i + 1] = this->m_BitsLensFreq[i] + nPrevF;
				nPrevF = FreqPrev[i + 1];
			}

			CommonLib::FxBitWriteStream bitStream;
			uint32 nBitSize = (this->m_nLenBitSize +7)/8;

			pStream->write(vecValues[0]);
			//pStream->write((uint16)nBitSize);


			
			m_SignCompressor.BeginCompress(pStream);

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
		bool decompress(uint32 nSize, TBPVector<_TValue>& vecValues, CommonLib::IReadStream* pStream)
		{
			this->clear();
			byte nFlag = pStream->readByte();
			bool bRangeCode = nFlag & 0x01;
			this->m_nTypeFreq = (eCompressDataType)(nFlag>>1);
			this->ReadDiffsLens(pStream);


			uint32 FreqPrev[_nMaxBitsLens + 1 + 1];
			memset(FreqPrev, 0, sizeof(FreqPrev));

			int32 nPrevF = 0;
			for (uint32 i = 0; i < _nMaxBitsLens + 1; ++i)
			{

				FreqPrev[i + 1] = this->m_BitsLensFreq[i] + nPrevF;
				nPrevF = FreqPrev[i + 1];
			}

			TValue nBegin = 0;
			pStream->read(nBegin);
			vecValues.push_back(nBegin);
			//uint16 nBitSize = pStream->readintu16();
			uint32 nBitSize = (this->m_nLenBitSize +7)/8;

			CommonLib::FxBitReadStream bitStream;


			m_SignCompressor.BeginDecompress(pStream, nSize - 1);

			bitStream.attach(pStream, pStream->pos(), nBitSize);
			pStream->seek(nBitSize, CommonLib::soFromCurrent);

			if(bRangeCode)
				return Decompress<TRangeDecoder>(nSize, vecValues, pStream, FreqPrev, &bitStream, nBegin);
			else
				return Decompress<TACDecoder>(nSize, vecValues, pStream, FreqPrev, &bitStream, nBegin);


		}





	private:


		template <class TEncoder>
		bool Compress(TEncoder& encoder, const TBPVector<_TValue>& vecValues, CommonLib::IWriteStream* pStream, uint32 *FreqPrev, CommonLib::FxBitWriteStream *pBitStream)
		{		 
			for (uint32 i = 1, sz = vecValues.size(); i< sz; ++i)
			{

				TSignValue nDiff = vecValues[i] - vecValues[i - 1];
				uint16 nBitLen =  this->m_FindBit.FMSB(nDiff);

				assert(this->m_BitsLensFreq[nBitLen] != 0);

				m_SignCompressor.EncodeSign(nDiff < 0 , i - 1);
				if(nBitLen > 1)
					pBitStream->writeBits(nDiff > 0 ? nDiff : -nDiff, nBitLen - 1);
				if(!encoder.EncodeSymbol(FreqPrev[nBitLen], FreqPrev[nBitLen + 1], this->m_nCount))
					return false;
			}
			return encoder.EncodeFinish();
		}


		bool CompressRangeCode(const TBPVector<_TValue>& vecValues, CommonLib::IWriteStream* pStream, uint32 *FreqPrev, uint32 nMaxByteSize,
			CommonLib::FxBitWriteStream *pBitStream)
		{

			TRangeEncoder rgEncoder(pStream, nMaxByteSize);
			return Compress(rgEncoder, vecValues, pStream, FreqPrev, pBitStream);
		}

		void CompressAcCode(const TBPVector<_TValue>& vecValues, CommonLib::IWriteStream* pStream,
			uint32 *FreqPrev, CommonLib::FxBitWriteStream *pBitStream)
		{
			TACEncoder acEncoder(pStream);
			Compress(acEncoder, vecValues, pStream, FreqPrev, pBitStream);
		}

		template<class TDecoder>
		bool Decompress(uint32 nSize, TBPVector<_TValue>& vecValues, CommonLib::IReadStream* pStream, uint32 *FreqPrev, 
			CommonLib::FxBitReadStream *pBitStream, TValue nBegin)
		{


			TDecoder decoder(pStream);
			decoder.StartDecode();
			TSignValue value = 0;

			for (uint32 i = 0; i < this->m_nCount; ++i)
			{
				uint32 freq = decoder.GetFreq(this->m_nCount);

				//uint32 nBitLen;
				//for(nBitLen = _nMaxBitsLens;FreqPrev[nBitLen] > freq;nBitLen--);
				int32 nBitLen = CommonLib::upper_bound(FreqPrev, _nMaxBitsLens + 1, freq);
				if(nBitLen != 0)
					nBitLen--;

				//bool bSign = pBitStream->readBit();


				bool bSign = m_SignCompressor.DecodeSign( i - 1);
				value = nBitLen;
				if(value > 1)
				{
					value = 0;
					pBitStream->readBits(value, nBitLen - 1);
					value |= 1 << (nBitLen - 1);
				}
					 
				nBegin +=  (bSign ? -1 *value : value);

				vecValues.push_back(nBegin);
			 
				decoder.DecodeSymbol(FreqPrev[nBitLen], FreqPrev[nBitLen+1], this->m_nCount);

			}

			return true;
		}
		private:
			TSignCompressor m_SignCompressor;
	};






	typedef TSignedDiffNumLenCompressor<int64, int64, CommonLib::TRangeEncoder64, CommonLib::TACEncoder64, 
		CommonLib::TRangeDecoder64, CommonLib::TACDecoder64, 64> SignedDiffNumLenCompressor64i;

	typedef TSignedDiffNumLenCompressor<uint64, int64, CommonLib::TRangeEncoder64, CommonLib::TACEncoder64, 
		CommonLib::TRangeDecoder64, CommonLib::TACDecoder64, 64> SignedDiffNumLenCompressor64u;

	typedef TSignedDiffNumLenCompressor<int32, int32, CommonLib::TRangeEncoder64, CommonLib::TACEncoder64, 
		CommonLib::TRangeDecoder64, CommonLib::TACDecoder64, 32> SignedDiffNumLenCompressor32i;

	typedef TSignedDiffNumLenCompressor<uint32, int32, CommonLib::TRangeEncoder64, CommonLib::TACEncoder64, 
		CommonLib::TRangeDecoder64, CommonLib::TACDecoder64, 32> SignedDiffNumLenCompressor32u;


	typedef TSignedDiffNumLenCompressor<int16, int16, CommonLib::TRangeEncoder64, CommonLib::TACEncoder64, 
		CommonLib::TRangeDecoder64, CommonLib::TACDecoder64, 16> SignedDiffNumLenCompressor16i;

	typedef TSignedDiffNumLenCompressor<uint16, int16, CommonLib::TRangeEncoder64, CommonLib::TACEncoder64, 
		CommonLib::TRangeDecoder64, CommonLib::TACDecoder64, 16> SignedDiffNumLenCompressor16u;



}
#endif