#pragma once
#include "BaseNumLenEncoder.h"
#include "CommonLibrary/ArithmeticCoder.h"
#include "../SignCompressor.h"
namespace embDB
{

	template<class _TValue, class _TEncoder, class _TDecoder, class _TCompParams, uint32 _nMaxBitsLens>
	class SignedNumLenEncoder : public TBaseNumLenEncoder<_TValue, _TEncoder, _TDecoder, _TCompParams, _nMaxBitsLens>
	{
	public:
		typedef TBaseNumLenEncoder<_TValue, _TEncoder, _TDecoder, _TCompParams, _nMaxBitsLens> TBase;
		typedef	typename TBase::TValue TValue;
		typedef typename TBase::TEncoder TEncoder;
		typedef typename TBase::TDecoder TDecoder;
		typedef typename TBase::TCompParams TCompParams;
		typedef typename TDefSign<TValue>::TSignType TSignValue;
	

		SignedNumLenEncoder(uint32 nPageSize, CommonLib::alloc_t* pAlloc = nullptr, TCompParams *pCompParams = nullptr) : TBase(nPageSize,pAlloc, pCompParams)
		{}


		void clear()
		{
			m_nPos = 0;
			m_signEncode.clear();
			TBase::clear();
		}

 

		void AddSymbol(TSignValue symbol)
		{
			m_signEncode.AddSymbol(symbol > 0 ? false : true);
			TBase::AddSymbol(abs(symbol));
		}

		void RemoveSymbol(TSignValue symbol)
		{
			m_signEncode.RemoveSymbol(symbol > 0 ? false : true);
			TBase::RemoveSymbol(abs(symbol));
		}

		uint32 GetCompressSize() const
		{
			return m_signEncode.GetCompressSize() + TBase::GetCompressSize();
		}

		bool BeginEncoding(CommonLib::IWriteStream *pStream)
		{
			WriteHeader(pStream);
			m_nPos = 0;
			double dRowBitsLen = GetCodeBitSize();
			uint32 nByteSize = (dRowBitsLen + 7) / 8;
			uint32 nBitSize = (m_nLenBitSize + 7) / 8;
			uint32 nSignSize = m_signEncode.GetCompressSize();

			if ((pStream->pos() + nBitSize + nByteSize + nSignSize) > pStream->size())
				return false;
			m_bitWStream.attach(pStream, pStream->pos(), nBitSize, true);
			m_signEncode.BeginEncoding(pStream);
			m_Encoder.Reset(pStream);
			return true;
		}
		bool encodeSymbol(TSignValue symbol)
		{
			uint32 nBitLen = 0;

			m_signEncode.EncodeSign(symbol > 0 ? false : true, m_nPos);
			symbol = abs(symbol);
			if (symbol < 2)
				nBitLen = symbol;
			else  nBitLen = BitsUtils::log2(symbol) + 1;

			assert(m_BitsLensFreq[nBitLen] != 0);

			if (!m_Encoder.EncodeSymbol(m_FreqPrev[nBitLen], m_FreqPrev[nBitLen + 1], m_nCount))
				return false;

			if (nBitLen > 1)
				m_bitWStream.writeBits(symbol, nBitLen - 1);
			m_nPos += 1;
			return true;
		}
		bool FinishEncoding(CommonLib::IWriteStream *pStream)
		{
			return m_Encoder.EncodeFinish();
		}


		void BeginDecoding(CommonLib::IReadStream *pStream)
		{
			m_nPos = 0;
			ReadHeader(pStream);
			double dRowBitsLen = GetCodeBitSize();

			uint32 nByteSize = (dRowBitsLen + 7) / 8;
			uint32 nBitSize = (m_nLenBitSize + 7) / 8;

			m_bitRStream.attach(pStream, pStream->pos(), nBitSize, true);

			m_signEncode.BeginDecoding(pStream, this->m_nCount);
			m_Decoder.Reset(pStream);
			m_Decoder.StartDecode();
		}
		TSignValue decodeSymbol()
		{
			uint32 freq = (uint32)m_Decoder.GetFreq(m_nCount);
			TSignValue symbol = 0;
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

			if (m_signEncode.DecodeSign(m_nPos))
				symbol = -symbol;
			m_nPos += 1;
			return symbol;
		}

		void FinishDecoding()
		{

		}
	private:
		TSignEncoder m_signEncode;
		CommonLib::FxBitWriteStream m_bitWStream;
		CommonLib::FxBitReadStream m_bitRStream;
		TEncoder m_Encoder;
		TDecoder m_Decoder;
		int m_nPos;
	};
 
}