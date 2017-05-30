#pragma once
#include "BaseNumLenEncoder.h"
#include "CommonLibrary/ArithmeticCoder.h"
namespace embDB
{
	template<class _TValue, class _TCoder, class _TEncoder, class _TCompParams, uint32 _nMaxBitsLens>
	class UnsignedNumLenEncoder : public TBaseNumLenEncoder<_TValue, _TCoder, _TCompParams, _nMaxBitsLens>
	{
		public:
			typedef TBaseNumLenEncoder<_TValue, _TCoder, _TCompParams, _nMaxBitsLens> TBase;
			typedef	typename TBase::TValue TValue;
			typedef typename TBase::TCoder TCoder;
			typedef typename TBase::TEncoder TEncoder;
			typedef typename TBase::TCompParams TCompParams;

			UnsignedNumLenEncoder(CommonLib::alloc_t* pAlloc = nullptr, TCompParams *pCompParams = nullptr) : TBase(pAlloc, pCompParams)
			{}

			bool BeginEncoding(CommonLib::IWriteStream *pStream)
			{
				WriteHeader(pStream);

				double dRowBitsLen = GetCodeBitSize();
				uint32 nByteSize = (dRowBitsLen + 7) / 8;
				uint32 nBitSize = (m_nLenBitSize + 7) / 8;

				if ((pStream->pos() + nBitSize + nByteSize) > pStream->size())
					return false;
				m_bitWStream.attach(pStream, pStream->pos(), nBitSize, true);
				m_Encoder.Reset(pStream);
				return true;
			}
			bool encodeSymbol(TValue symbol)
			{
				uint16 nBitLen = 0;
				if (value == 0)
					nBitLen = 0;
				else if (value == 1)
					nBitLen = 1;
				else  nBitLen = BitsUtils::log2(symbol);

				assert(m_BitsLensFreq[nBitLen] != 0);

				if (!m_Encoder.EncodeSymbol(m_FreqPrev[nBitLen], m_FreqPrev[nBitLen + 1], m_nCount))
					return false;

				if (nBitLen > 1)
					m_bitWStream.writeBits(value, nBitLen - 1);

				return true;
			}
			void FinishEncoding(CommonLib::IWriteStream *pStream)
			{
				return m_Encoder.EncodeFinish();
			}


			void BeginDecoding(CommonLib::IWriteStream *pStream)
			{
				ReadHeader(pStream);
				double dRowBitsLen = GetCodeBitSize();

				uint32 nByteSize = (dRowBitsLen + 7) / 8;
				uint32 nBitSize = (m_nLenBitSize + 7) / 8;

				m_bitRStream.attach(pStream, pStream->pos(), nBitSize, true);
				m_Decoder.Reset(pStream);
				m_Decoder.StartDecode();
				
			}
			TValue encodeSymbol()
			{
				uint32 freq = (uint32)m_Decoder.GetFreq(m_nCount);
				TValue symbol = CommonLib::upper_bound(m_FreqPrev, _nMaxBitsLens + 1, freq);
				if (symbol != 0)
					symbol--;

				m_Decoder.DecodeSymbol(m_FreqPrev[symbol], m_FreqPrev[symbol + 1], m_nCount);

				if (symbol > 1)
				{
					uint32 nBitLen = symbol - 1;
					m_bitRStream.readBits(symbol, nBitLen);
					symbol |= ((TValue)1 << (nBitLen));
				}
				return symbol;
			}

			void FinishDecoding()
			{

			}
	private:

		CommonLib::FxBitWriteStream m_bitWStream;
		CommonLib::FxBitReadStream m_bitRStream;
		TEncoder m_Encoder;
		TDecoder m_Decoder;
	};
 
}