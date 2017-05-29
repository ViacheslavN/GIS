#pragma once
#include "CommonLibrary/FixedBitStream.h"
#include <map>
#include "../MathUtils.h"
#include "BPVector.h"
#include "CommonLibrary/algorithm.h"
#include "CommonLibrary/RangeCoder.h"
#include "CommonLibrary/ArithmeticCoder.h"
#include "CompressUtils.h"
#include "embDB.h"
 
namespace embDB
{


	template <class _TValue, class _TFindBit, uint32 _nMaxBitsLens>
	class TNumLemCompressor2
	{
	public:
		typedef _TValue TValue;
		typedef _TFindBit TFindBit;

		//typedef TRangeEncoder64 TEncoder;
		//typedef TRangeDecoder64 TDecoder;

		typedef CommonLib::TACEncoder64 TEncoder;
		typedef CommonLib::TACDecoder64 TDecoder;

		TNumLemCompressor2(uint32 nError = 100) : m_nError(nError)
		{
			clear();
		}
		~TNumLemCompressor2() {}

		uint16 PreAddSympol(const TValue& value)
		{

			uint16 nBitLen = 0;
			if (value == 0)
				nBitLen = 0;
			else if (value == 1)
				nBitLen = 1;
			else  nBitLen = m_FindBit.FMSB(value);




			assert(nBitLen < _nMaxBitsLens + 1);
			m_BitsLensFreq[nBitLen] += 1;

			if (m_FreqType != ectUInt64 && m_BitsLensFreq[nBitLen] > 0xFFFFFFFF - 1)
				m_FreqType = ectUInt64;
			else if(m_FreqType != ectUInt32 && m_BitsLensFreq[nBitLen] > 65535)
				m_FreqType = ectUInt32;
			else if (m_FreqType != ectUInt16 && m_BitsLensFreq[nBitLen] > 255)
				m_FreqType = ectUInt16;

			m_nBitLen += nBitLen > 1 ? nBitLen - 1 : 0;
			m_nCount += 1;
			return nBitLen > 1 ? nBitLen - 1 : 0;

		}

		uint32 GetMaxBitLen() const
		{
			uint16 nBitLen = 0;
			for (uint32 i = 0; i < _nMaxBitsLens + 1; ++i)
			{
				if (m_BitsLensFreq[i] != 0)
					nBitLen = i;
			}
			return nBitLen;
		}

		void WriteHeader(CommonLib::IWriteStream* pStream)
		{

			uint32 nBitLen = GetMaxBitLen();

			pStream->write((byte)nBitLen);

			byte nFlag = m_FreqType;
			pStream->write(nFlag);
			byte LensMask[sizeof(uint64) + 1];

			memset(LensMask, 0, sizeof(uint64) + 1);
			for (uint32 i = 0; i < nBitLen + 1; ++i)
			{
				uint32 nByte = i / 8;
				uint32 nBit = i - (nByte * 8);
				if (m_BitsLensFreq[i] != 0)
					LensMask[nByte] |= (0x01 << nBit);
			}
			for (uint32 i = 0; i < (nBitLen) / 8 + 1; ++i)
			{
				pStream->write((byte)LensMask[i]);
			}
			for (uint32 i = 0; i < nBitLen + 1; ++i)
			{
				if (m_BitsLensFreq[i] == 0)
					continue;

				switch (m_FreqType)
				{
				case ectByte:
					pStream->write((byte)m_BitsLensFreq[i]);
					break;
				case ectUInt16:
					pStream->write((uint16)m_BitsLensFreq[i]);
					break;
				case ectUInt32:
					pStream->write((uint32)m_BitsLensFreq[i]);
					break;
				case ectUInt64:
					pStream->write((uint64)m_BitsLensFreq[i]);
					break;
				}

			}
			memset(m_FreqPrev, 0, sizeof(m_FreqPrev));
			int32 nPrevF = 0;
			for (uint32 i = 0; i < _nMaxBitsLens + 1; ++i)
			{
				m_FreqPrev[i + 1] = m_BitsLensFreq[i] + nPrevF;
				nPrevF = m_FreqPrev[i + 1];
			}
		}


		void BeginEncode(CommonLib::IWriteStream* pStream)
		{
			//clear();
			m_Encoder.Reset(pStream);
		}


		uint32 EncodeSymbol(const TValue& value, CommonLib::FxBitWriteStream *pBitStream)
		{
			uint16 nBitLen = 0;
			if (value == 0)
				nBitLen = 0;
			else if (value == 1)
				nBitLen = 1;
			else  nBitLen = m_FindBit.FMSB(value);

			assert(m_BitsLensFreq[nBitLen] != 0);


			m_Encoder.EncodeSymbol(m_FreqPrev[nBitLen], m_FreqPrev[nBitLen + 1], m_nCount);
			if (nBitLen > 1)
				pBitStream->writeBits(value, nBitLen - 1);

			return nBitLen;

		}

		void EncodeFinish()
		{
			m_Encoder.EncodeFinish();
		}


		void ReadHeader(CommonLib::IReadStream* pStream)
		{
			clear();
			uint32 nBitsLen = (uint32)pStream->readByte();
			m_FreqType = (eCompressDataType)pStream->readByte();


			byte LensMask[(_nMaxBitsLens) / 8 + 1];
			memset(LensMask, 0, (_nMaxBitsLens) / 8 + 1);

			for (uint32 i = 0; i < (nBitsLen) / 8 + 1; ++i)
			{
				LensMask[i] = pStream->readByte();
			}

			for (uint32 i = 0; i < nBitsLen + 1; ++i)
			{

				uint32 nByte = i / 8;
				uint32 nBit = i - (nByte * 8);

				if (!(LensMask[nByte] & (0x01 << nBit)))
				{
					m_BitsLensFreq[i] = 0;
					continue;
				}
				switch (m_FreqType)
				{
				case ectByte:
					m_BitsLensFreq[i] = pStream->readByte();
					m_nCount += m_BitsLensFreq[i];
					break;
				case ectUInt16:
					m_BitsLensFreq[i] = pStream->readintu16();
					m_nCount += m_BitsLensFreq[i];
					break;
				case ectUInt32:
					m_BitsLensFreq[i] = pStream->readIntu32();
					m_nCount += m_BitsLensFreq[i];
					break;
				case ectUInt64:
					m_BitsLensFreq[i] = pStream->readIntu64();
					m_nCount += m_BitsLensFreq[i];
					break;
				}
				if (i > 1)
					m_nBitLen += m_BitsLensFreq[i] * (i - 1);
			}

			int32 nPrevF = 0;
			for (uint32 i = 0; i < _nMaxBitsLens + 1; ++i)
			{

				m_FreqPrev[i + 1] = m_BitsLensFreq[i] + nPrevF;
				nPrevF = m_FreqPrev[i + 1];
			}


		}


		void Init(CommonLib::IReadStream* pStream)
		{
			m_Decoder.Reset(pStream);
			ReadHeader(pStream);


		}


		void StartDecode()
		{
			m_Decoder.StartDecode();

		}
		template<class TSum>
		bool DecodeSymbol(TSum& nSymbol)
		{
			uint64 freq =  m_Decoder.GetFreq(m_nCount);
			nSymbol = (TSum)CommonLib::upper_bound(m_FreqPrev, _nMaxBitsLens + 1, freq);
			if (nSymbol != 0)
				nSymbol--;

			m_Decoder.DecodeSymbol(m_FreqPrev[nSymbol], m_FreqPrev[nSymbol + 1], m_nCount);
			return true;
		}

		void clear()
		{
			memset(m_BitsLensFreq, 0, sizeof(m_BitsLensFreq));
			memset(m_FreqPrev, 0, sizeof(m_FreqPrev));
			m_nCount = 0;
			m_FreqType = ectByte;
			m_nBitLen = 0;
		}


		uint32 GetCount() const
		{
			return m_nCount;
		}

		double CalcRowBitSize() const
		{
			double dBitRowSize = 0;
			for (uint32 i = 0; i < _nMaxBitsLens + 1; ++i)
			{
				if (m_BitsLensFreq[i] == 0)
					continue;
				double dFreq = m_BitsLensFreq[i];
				double dLog2 = mathUtils::Log2((double)m_nCount / dFreq);
				dBitRowSize += (dFreq* dLog2);
			}
			dBitRowSize += 64;
			//dBitRowSize += (dBitRowSize/m_nError);

			return dBitRowSize;
		}
		uint32 GetHeaderSize() const
		{
			//uint32 nSize = _nMaxBitsLens/8  + 1;
			uint32 nSize = 1 + 1; //maxLen + freqtype
			for (uint32 i = 0; i < _nMaxBitsLens + 1; ++i)
			{
				if (m_BitsLensFreq[i] != 0)
					nSize += GetSizeTypeValue(m_FreqType);
			}
			return nSize;
		}
		uint32 GetBitsLen() const
		{
			return m_nBitLen;
		}
		uint32 GetCompressSize() const
		{

			double dBitRowSize = CalcRowBitSize();
			uint32 nHeaderSize = GetHeaderSize();
			return  (uint32)(dBitRowSize + 7) / 8 + nHeaderSize;
		}
	private:

		uint64 m_BitsLensFreq[_nMaxBitsLens + 1];
		uint64 m_FreqPrev[_nMaxBitsLens + 1 + 1];
		TFindBit m_FindBit;
		uint64 m_nCount;
		uint64 m_nBitLen;
		eCompressDataType m_FreqType;

		TEncoder m_Encoder;
		TDecoder m_Decoder;
		uint32 m_nError;
	};


}

