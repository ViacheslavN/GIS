#ifndef _EMBEDDED_DATABASE_SIGNED_NUM_LEN_DIFF_COMPRESS_2_H_
#define _EMBEDDED_DATABASE_SIGNED_NUM_LEN_DIFF_COMPRESS_2_H_

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
#include "SignedNumLenDiffCompress.h"
namespace embDB
{

 

	template<class _TValue,
	class _TRangeEncoder,
	class _TACEncoder,
	class _TRangeDecoder,
	class _TACDecoder,		
		uint32 _nMaxBitsLens>
	class TSignedDiffNumLenCompressor2 : public TUnsignedNumLenCompressor<_TValue, TSignedFindMostSigBit, _TRangeEncoder,
		_TACEncoder, _TRangeDecoder, _TACDecoder, _nMaxBitsLens>
	{
	public:


		typedef TUnsignedNumLenCompressor<_TValue, TSignedFindMostSigBit, _TRangeEncoder,
			_TACEncoder, _TRangeDecoder, _TACDecoder, _nMaxBitsLens> TBase;

		typedef typename TBase::TValue TValue;
		typedef typename TBase::TFindBit TFindBit;
		typedef typename TBase::TRangeEncoder   TRangeEncoder;
		typedef typename TBase::TACEncoder		 TACEncoder;
		typedef typename TBase::TRangeDecoder	 TRangeDecoder;
		typedef typename TBase::TACDecoder		 TACDecoder;
 

		TSignedDiffNumLenCompressor2(CompressType type, uint32 nError = 200, bool bOnlineCalcSize = false) :
		TBase(type, nError, bOnlineCalcSize), m_nFlag(0), m_pWriteStream(0)
		{
		}
		uint16 AddSymbol(TValue symbol)
		{

			assert(m_SignCompressor.count() == this->m_nCount);
			m_SignCompressor.AddSymbol(symbol < 0);
			return TBase::AddSymbol(symbol);
		}
		void RemoveSymbol(TValue symbol)
		{
			assert(m_SignCompressor.count() == this->m_nCount);
			m_SignCompressor.RemoveSymbol(symbol< 0);
			TBase::RemoveSymbol(symbol);
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
			memset(m_FreqPrev, 0, sizeof(m_FreqPrev));
			m_nFlag = 0;
		}


		void BeginEncode(CommonLib::IWriteStream* pStream)
		{
			m_pWriteStream = pStream;
			assert(m_SignCompressor.count() == this->m_nCount);
			 
			byte nFlag = (byte)this->m_nTypeFreq;
			pStream->write(nFlag);

			this->WriteDiffsLens(m_pWriteStream);

			double dRowBitsLen = this->GetCodeBitSize();
			uint32 nByteSize = (dRowBitsLen + 7)/8;
						
			memset(m_FreqPrev, 0, sizeof(m_FreqPrev));

			int32 nPrevF = 0;
			for (uint32 i = 0; i < _nMaxBitsLens + 1; ++i)
			{

				m_FreqPrev[i + 1] = this->m_BitsLensFreq[i] + nPrevF;
				nPrevF = m_FreqPrev[i + 1];
			}
 
			uint32 nBitSize = (this->m_nLenBitSize +7)/8;

			m_SignCompressor.BeginCompress(m_pWriteStream);

			m_WriteBitStream.attach(m_pWriteStream, m_pWriteStream->pos(), nBitSize);
			m_pWriteStream->seek(nBitSize, CommonLib::soFromCurrent);
			m_ACEncoder.SetStream(m_pWriteStream);
		 
		}


		void EncodeSymbol(const TValue& symbol, int nIndex)
		{
			uint16 nBitLen =  this->m_FindBit.FMSB(symbol);

			assert(this->m_BitsLensFreq[nBitLen] != 0);

			m_SignCompressor.EncodeSign(symbol < 0, nIndex);
			if(nBitLen > 1)
				m_WriteBitStream.writeBits(symbol > 0 ? symbol : -symbol, nBitLen - 1);
			m_ACEncoder.EncodeSymbol(m_FreqPrev[nBitLen], m_FreqPrev[nBitLen + 1], this->m_nCount);
		}

		void EncodeFinish()
		{
			 m_ACEncoder.EncodeFinish();
		}



		void BeginDecode(CommonLib::IReadStream* pStream)
		{

			m_pReadStream = pStream;
			this->clear();
			byte nFlag = m_pReadStream->readByte();
			this->m_nTypeFreq = (eCompressDataType)(nFlag);
			this->ReadDiffsLens(m_pReadStream);


	 
			memset(m_FreqPrev, 0, sizeof(m_FreqPrev));

			int32 nPrevF = 0;
			for (uint32 i = 0; i < _nMaxBitsLens + 1; ++i)
			{

				m_FreqPrev[i + 1] = this->m_BitsLensFreq[i] + nPrevF;
				nPrevF = m_FreqPrev[i + 1];
			}
 
			uint32 nBitSize = (this->m_nLenBitSize + 7)/8;
			m_SignCompressor.BeginDecompress(m_pReadStream, this->m_nCount);

			m_readBitStream.attach(m_pReadStream, m_pReadStream->pos(), nBitSize);
			m_pReadStream->seek(nBitSize, CommonLib::soFromCurrent);

			m_ACDecoder.SetStream(m_pReadStream);
			m_ACDecoder.StartDecode();
		}


		bool DecodeSymbol(TValue& value, int nIndex)
		{
			uint32 freq = m_ACDecoder.GetFreq(this->m_nCount);
			int32 nBitLen = CommonLib::upper_bound(m_FreqPrev, _nMaxBitsLens + 1, freq);
			if(nBitLen != 0)
				nBitLen--;

			bool bSign = m_SignCompressor.DecodeSign( nIndex);
			value = nBitLen;
			if(value > 1)
			{
				value = 0;
				m_readBitStream.readBits(value, nBitLen - 1);
				value |= 1 << (nBitLen - 1);
			}
			if(bSign)
				value *= -1;
			m_ACDecoder.DecodeSymbol(m_FreqPrev[nBitLen], m_FreqPrev[nBitLen+1], this->m_nCount);
			return true;
		}

	private:
		byte m_nFlag;
		TSignCompressor m_SignCompressor;
		CommonLib::FxBitWriteStream m_WriteBitStream;
		CommonLib::FxBitReadStream m_readBitStream;
		CommonLib::IWriteStream* m_pWriteStream;
		CommonLib::IReadStream* m_pReadStream;
		uint32 m_FreqPrev[_nMaxBitsLens + 1 + 1];
		TACEncoder m_ACEncoder;
		TACDecoder m_ACDecoder;
	};






	typedef TSignedDiffNumLenCompressor2<int64, CommonLib::TRangeEncoder64, CommonLib::TACEncoder64, 
		CommonLib::TRangeDecoder64, CommonLib::TACDecoder64, 64> SignedDiffNumLenCompressor264i;

	typedef TSignedDiffNumLenCompressor2<int32, CommonLib::TRangeEncoder64, CommonLib::TACEncoder64, 
		CommonLib::TRangeDecoder64, CommonLib::TACDecoder64, 32> SignedDiffNumLenCompressor232i;

	typedef TSignedDiffNumLenCompressor2<int16, CommonLib::TRangeEncoder64, CommonLib::TACEncoder64, 
		CommonLib::TRangeDecoder64, CommonLib::TACDecoder64, 16> SignedDiffNumLenCompressor216i;

	typedef TSignedDiffNumLenCompressor2<int8, CommonLib::TRangeEncoder64, CommonLib::TACEncoder64, 
		CommonLib::TRangeDecoder64, CommonLib::TACDecoder64, 8> SignedDiffNumLenCompressor28i;

}
#endif