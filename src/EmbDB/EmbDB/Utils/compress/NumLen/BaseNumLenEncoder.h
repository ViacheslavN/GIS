#pragma once

#include "CommonLibrary/general.h"
#include "../../MathUtils.h"
#include "../../BitUtils.h"
#include "../CompressUtils.h"

namespace embDB
{
	template<class _TValue, class _TCoder, class _TEncoder, class _TCompParams, uint32 _nMaxBitsLens>
	class TBaseNumLenEncoder
	{
		public:
			typedef _TValue TValue;
			typedef _TCoder TCoder;
			typedef _TEncoder TEncoder;
			typedef _TCompParams TCompParams

			TBaseNumLenEncoder(CommonLib::alloc_t* pAlloc = nullptr, TCompParams *pCompParams = nullptr) : m_nError(100), m_bOnlineCalcSize(true), m_nTypeFreq(ectByte),
				m_nLenBitSize(0), m_nCount(0), m_nDiffsLen(0), m_nFlags(0)
			{
				if (pCompParams)
				{
					m_nError = pCompParams->m_nError;
					m_bOnlineCalcSize = pCompParams->m_bOnlineCalcSize;
				}

				memset(m_BitsLensFreq, 0, sizeof(m_BitsLensFreq));
				memset(m_FreqPrev, 0, sizeof(m_FreqPrev));
			}

			void clear()
			{
				m_nCount = 0;
				m_nDiffsLen = 0;
				m_nFlags = 0;
				m_nTypeFreq = ectByte;
				m_nLenBitSize = 0;
				memset(m_BitsLensFreq, 0, sizeof(m_BitsLensFreq));
				memset(m_FreqPrev, 0, sizeof(m_FreqPrev));
			}

			uint32 AddSymbol(TValue symbol)
			{
				uint32 nBitLen = 0;
				if (symbol == 0)
					nBitLen = 0;
				else if (symbol == 1)
					nBitLen = 1;
				else  nBitLen = BitsUtils::log2(symbol);

				assert(nBitLen < _nMaxBitsLens + 1);

				m_nLenBitSize += nBitLen > 1 ? nBitLen - 1 : 0;
				m_nCount++;

				if (!m_BitsLensFreq[nBitLen])
					m_nDiffsLen++;

				m_BitsLensFreq[nBitLen] += 1;



				if (m_nTypeFreq != ectUInt32)
				{
					if (m_BitsLensFreq[nBitLen] > 255)
						m_nTypeFreq = ectUInt16;
					if (m_BitsLensFreq[nBitLen] > 65535)
						m_nTypeFreq = ectUInt32;
				}

				if (!m_bOnlineCalcSize)
					return nBitLen;

				uint32 nNewCount = m_BitsLensFreq[nBitLen];
				uint32 nOldCount = nNewCount - 1;

				if (m_nDiffsLen > 1)
				{
					//m_dBitRowSize += (nNewCount * mathUtils::Log2((double)m_nCount/nNewCount) -  
					//	nOldCount* mathUtils::Log2((double)(m_nCount - 1)/nOldCount) + (m_nCount - nNewCount) * mathUtils::Log2((double)m_nCount/(m_nCount - 1)));

					m_dBitRowSize += (nNewCount * mathUtils::Log2((double)m_nCount / nNewCount));
					if (nOldCount > 0)
						m_dBitRowSize -= nOldCount* mathUtils::Log2((double)(m_nCount - 1) / nOldCount);

					m_dBitRowSize += (m_nCount - nNewCount) * mathUtils::Log2((double)m_nCount / (m_nCount - 1));

				}

				/*	double dBitRowSize = CalcRowBitSize();
				if(fabs(m_dBitRowSize - dBitRowSize) > 0.00000001)
				{
					int dd = 0;
					dd++;
				}*/

				return nBitLen;
			}


			void RemoveSymbol(TValue symbol)
			{
				uint16 nBitLen = 0;
				if (symbol == 0)
					nBitLen = 0;
				else if (symbol == 1)
					nBitLen = 1;
				else
				{
					 nBitLen = BitsUtils::log2(symbol);
					assert(m_nLenBitSize >= nBitLen - 1);
				}
				

				m_nLenBitSize -= nBitLen > 1 ? nBitLen - 1 : 0;
				m_nCount--;

				assert(m_BitsLensFreq[nBitLen]);

				m_BitsLensFreq[nBitLen] -= 1;
				if (m_BitsLensFreq[nBitLen] == 65535 || m_BitsLensFreq[nBitLen] == 255)
				{
					m_nTypeFreq = ectByte;
					for (uint32 i = 0; i < _nMaxBitsLens; ++i)
					{
						if (m_BitsLensFreq[i] > 65535)
						{
							m_nTypeFreq = ectUInt32;
							break;
						}
						if (m_nTypeFreq != ectUInt32)
						{
							if (m_BitsLensFreq[nBitLen] > 255)
								m_nTypeFreq = ectUInt16;
						}
					}
				}
				if (!m_BitsLensFreq[nBitLen])
					m_nDiffsLen--;

				if (!m_bOnlineCalcSize)
					return;

				uint32 nNewCount = m_BitsLensFreq[nBitLen];
				uint32 nOldCount = nNewCount + 1;

				if (m_nDiffsLen > 1)
				{

					m_dBitRowSize -= (nOldCount* mathUtils::Log2((double)(m_nCount + 1) / (nOldCount)));
					if (nNewCount > 0)
						m_dBitRowSize += (nNewCount* mathUtils::Log2((double)(m_nCount) / (nNewCount)));

					m_dBitRowSize -= (m_nCount - nNewCount) * mathUtils::Log2((double)(m_nCount + 1) / (m_nCount));

				}
				else
				{
					m_dBitRowSize = 0;

				}


				/*	double dBitRowSize = CalcRowBitSize();
				if(fabs(m_dBitRowSize - dBitRowSize) > 0.00000001)
				{
				int dd = 0;
				dd++;
				}*/

			}


			double GetCodeBitSize() const
			{
				double dBitRowSize = m_bOnlineCalcSize ? m_dBitRowSize : CalcRowBitSize<uint32>(m_BitsLensFreq, _nMaxBitsLens + 1, m_nDiffsLen, m_nCount);
				dBitRowSize += (dBitRowSize / m_nError) + 8 /*code  finish*/;

				return dBitRowSize;
			}
			int GetBitLenSize() const
			{
				return m_nLenBitSize;
			}

			uint32 GetCompressSize() const
			{
				double dRowBitsLen = GetCodeBitSize();
				uint32 nByteSize = (dRowBitsLen + 7) / 8;
				
				return nByteSize + 1 + (_nMaxBitsLens + 1 + 7) / 8 + GetLenForDiffLen(m_nTypeFreq, m_nDiffsLen) + (m_nLenBitSize + 7) / 8; //Type comp (rang or ac) + 4 +

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

		protected:
			uint32  m_BitsLensFreq[_nMaxBitsLens + 1];
			uint32 m_FreqPrev[_nMaxBitsLens + 1 + 1];
			uint16 m_nFlags;
			eCompressDataType m_nTypeFreq;
			mutable double m_dBitRowSize;
			bool m_bOnlineCalcSize;
			uint32 m_nCount;
			uint32 m_nDiffsLen;
			uint32 m_nError;
	};
}