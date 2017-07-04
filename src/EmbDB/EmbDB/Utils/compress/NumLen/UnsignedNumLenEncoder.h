#pragma once
#include "BaseNumLenEncoder.h"
#include "CommonLibrary/ArithmeticCoder.h"
namespace embDB
{
	template<class _TValue, class _TEncoder, class _TDecoder, class _TCompParams, uint32 _nMaxBitsLens>
	class UnsignedNumLenEncoder : public TBaseNumLenEncoder<_TValue,_TCompParams, _nMaxBitsLens>
	{
		public:
			typedef TBaseNumLenEncoder<_TValue, _TCompParams, _nMaxBitsLens> TBase;
			typedef	typename TBase::TValue TValue;
			typedef typename _TEncoder TEncoder;
			typedef typename _TDecoder TDecoder;
			typedef typename TBase::TCompParams TCompParams;
		

			UnsignedNumLenEncoder(uint32 nPageSize, CommonLib::alloc_t* pAlloc = nullptr, TCompParams *pCompParams = nullptr) : TBase(nPageSize, pAlloc, pCompParams)
			{}

			bool BeginEncoding(CommonLib::IWriteStream *pStream)
			{
				WriteHeader(pStream);

				double dRowBitsLen = GetCodeBitSize();
				uint32 nByteSize = uint32((dRowBitsLen + 7) / 8);
				uint32 nBitSize = (m_nLenBitSize + 7) / 8;

				if ((pStream->pos() + nBitSize + nByteSize) > pStream->size())
					return false;
				m_bitWStream.attach(pStream, pStream->pos(), nBitSize, true);
				m_Encoder.Reset(pStream);
				return true;
			}

			bool encodeSymbol(TValue symbol)
			{

				assert(!(symbol < 0));

				uint16 nBitLen = 0;
				if (symbol < 2)
					nBitLen = (uint16)symbol;
				else  nBitLen = BitsUtils::log2(symbol) + 1;

				assert(m_BitsLensFreq[nBitLen] != 0);

				if (!m_Encoder.EncodeSymbol(m_FreqPrev[nBitLen], m_FreqPrev[nBitLen + 1], m_nCount))
					return false;

				if (nBitLen > 1)
					m_bitWStream.writeBits(symbol, nBitLen - 1);

				return true;
			}
			bool FinishEncoding(CommonLib::IWriteStream *pStream)
			{
				return m_Encoder.EncodeFinish();
			}


			void BeginDecoding(CommonLib::IReadStream *pStream)
			{
				ReadHeader(pStream);
				double dRowBitsLen = GetCodeBitSize();

				uint32 nByteSize = uint32((dRowBitsLen + 7) / 8);
				uint32 nBitSize = (m_nLenBitSize + 7) / 8;

				m_bitRStream.attach(pStream, pStream->pos(), nBitSize, true);
				m_Decoder.Reset(pStream);
				m_Decoder.StartDecode();
				
			}
			void decodeSymbol(TValue& symbol)
			{
				symbol = 0;
				uint32 freq = (uint32)m_Decoder.GetFreq(m_nCount);
				int32 nBitLen = CommonLib::upper_bound(m_FreqPrev, _nMaxBitsLens + 1, freq);
				if (nBitLen != 0)
					nBitLen--;

				m_Decoder.DecodeSymbol(m_FreqPrev[nBitLen], m_FreqPrev[nBitLen + 1], m_nCount);

				if (nBitLen > 1)
				{
					m_bitRStream.readBits(symbol, nBitLen - 1);
					symbol |= ((TValue)1 << (nBitLen - 1));
				}
				else
					symbol = nBitLen;
			}
			TValue decodeSymbol()
			{
				TValue symbol = 0;
				decodeSymbol(symbol);
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