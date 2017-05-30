#pragma once
#include "BaseNumLenEncoder.h"
#include "CommonLibrary/ArithmeticCoder.h"
#include "../SignCompressor.h"
namespace embDB
{

	template<class _TType>
	struct TDefSign { typedef _TType TSignType; };

	template<> struct TDefSign< byte > { typedef char TSignType; };
	template<> struct TDefSign< uint16 > { typedef int16 TSignType; };
	template<> struct TDefSign< uint32 > { typedef int32 TSignType; };
	template<> struct TDefSign< uint64 > { typedef int64 TSignType; };

	template<class _TValue, class _TCoder, class _TEncoder, class _TCompParams, uint32 _nMaxBitsLens>
	class SignedNumLenEncoder : public TBaseNumLenEncoder<_TValue, _TCoder, _TCompParams, _nMaxBitsLens>
	{
	public:
		typedef TBaseNumLenEncoder<_TValue, _TCoder, _TCompParams, _nMaxBitsLens> TBase;
		typedef	typename TBase::TValue TValue;
		typedef typename TBase::TCoder TCoder;
		typedef typename TBase::TEncoder TEncoder;
		typedef typename TBase::TCompParams TCompParams;
		typedef typename TDefSign<TValue>::TSignType TSignValue

		SignedNumLenEncoder(CommonLib::alloc_t* pAlloc = nullptr, TCompParams *pCompParams = nullptr) : TBase(pAlloc, pCompParams)
		{}


		void clear()
		{
			m_nPos = 0;
			m_signEncode.clear();
			TBase::clear();
		}

		void AddSymbol(TValue symbol)
		{
			m_signEncode.AddSymbol(false);
			TBase::AddSymbol(symbol);
		}

		void AddSymbol(TSignValue symbol)
		{
			m_signEncode.AddSymbol(symbol > 0 ? true : false);
			TBase::AddSymbol(abs(symbol));
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

			m_signEncode.EncodeSign(symbol > 0 ? false : true, m_nPos)
			symbol = abs(symbol);
			if (symbol == 0)
				nBitLen = 0;
			else if (symbol == 1)
				nBitLen = 1;
			else  nBitLen = BitsUtils::log2(symbol);

			assert(m_BitsLensFreq[nBitLen] != 0);

			if (!m_Encoder.EncodeSymbol(m_FreqPrev[nBitLen], m_FreqPrev[nBitLen + 1], m_nCount))
				return false;

			if (nBitLen > 1)
				m_bitWStream.writeBits(value, nBitLen - 1);
			m_nPos += 1;
			return true;
		}
		void FinishEncoding(CommonLib::IWriteStream *pStream)
		{
			return m_Encoder.EncodeFinish();
		}


		void BeginDecoding(CommonLib::IWriteStream *pStream)
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


			m_nPos += 1;
		}
		TSignValue encodeSymbol()
		{
			uint32 freq = (uint32)m_Decoder.GetFreq(m_nCount);
			TSignValue symbol = CommonLib::upper_bound(m_FreqPrev, _nMaxBitsLens + 1, freq);
			if (symbol != 0)
				symbol--;

			m_Decoder.DecodeSymbol(m_FreqPrev[symbol], m_FreqPrev[symbol + 1], m_nCount);

			if (symbol > 1)
			{
				uint32 nBitLen = symbol - 1;
				m_bitRStream.readBits(symbol, nBitLen);
				symbol |= ((TValue)1 << (nBitLen));
			}

			if (m_signEncode.DecodeSign(nPos))
				symbol = -symbol;
			nPos += 1;
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