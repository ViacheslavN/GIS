#pragma once
#include "CommonLibrary/general.h"
#include "CommonLibrary/alloc_t.h"
#include "../../MathUtils.h"
#include "../../BitUtils.h"
#include "../CompressUtils.h"

namespace embDB
{
	template<class _TValue,  class _TCompParams, uint32 _nMaxBitsLens>
	class TBaseNumLenEncoder
	{
		public:
			typedef _TValue TValue;
			typedef _TCompParams TCompParams;

			TBaseNumLenEncoder(uint32 nPageSize, CommonLib::alloc_t* pAlloc = nullptr, TCompParams *pCompParams = nullptr) : m_nError(100), m_bOnlineCalcSize(true), m_nTypeFreq(ectByte)
				 
			{
				clear();

				if (pCompParams)
				{
					m_nError = pCompParams->m_nErrorCalc;
					m_bOnlineCalcSize = pCompParams->m_bCalcOnlineSize;
				}
 
			}

			void clear()
			{
				m_dBitRowSize = 0.;
				m_nCount = 0;
				m_nDiffsLen = 0;
				m_nFlags = 0;
				m_nTypeFreq = ectByte;
				m_nLenBitSize = 0;
	 			memset(m_BitsLensFreq, 0, sizeof(m_BitsLensFreq));
				memset(m_FreqPrev, 0, sizeof(m_FreqPrev));
			}

			void AddSymbol(TValue symbol)
			{
				uint32 nBitLen = 0;
				if (symbol < 2)
					nBitLen = (uint32)symbol;
				else
				{
					nBitLen = BitsUtils::log2(symbol);
					assert(nBitLen < _nMaxBitsLens + 1);
					m_nLenBitSize += nBitLen;

					nBitLen += 1; // for 0,1
				}				
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
					return;

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
				/*	if (m_dBitRowSize < 0)
					{
						int dd = 0;
						dd++;
					}*/
				}

				/*double dBitRowSize = CalcRowBitSize<uint32>(m_BitsLensFreq, _nMaxBitsLens + 1, m_nDiffsLen, m_nCount);
				if(fabs(m_dBitRowSize - dBitRowSize) > 0.00000001)
				{
					int dd = 0;
					dd++;
				}*/

			}


			void RemoveSymbol(TValue symbol)
			{
				uint16 nBitLen = 0;
				if (symbol < 2)
					nBitLen = (uint16)symbol;
				else
				{
					nBitLen = BitsUtils::log2(symbol);
					assert(m_nLenBitSize >= nBitLen);
					m_nLenBitSize -= nBitLen;

					nBitLen += 1;
				}
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
				uint32 nByteSize = uint32((dRowBitsLen + 7) / 8);
				
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
				byte nFlag = m_nTypeFreq;
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

					switch (m_nTypeFreq)
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
				m_nTypeFreq = (eCompressDataType)pStream->readByte();
				
			
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

					m_nDiffsLen += 1;
					switch (m_nTypeFreq)
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
						m_nLenBitSize += m_BitsLensFreq[i] * (i - 1);
				}

				int32 nPrevF = 0;
				for (uint32 i = 0; i < _nMaxBitsLens + 1; ++i)
				{

					m_FreqPrev[i + 1] = m_BitsLensFreq[i] + nPrevF;
					nPrevF = m_FreqPrev[i + 1];
				}

				m_dBitRowSize = CalcRowBitSize<uint32>(m_BitsLensFreq, _nMaxBitsLens + 1, m_nDiffsLen, m_nCount);

			}


			uint32 count() const
			{
				return m_nCount;
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
			uint32 m_nLenBitSize;
	 
	};
}