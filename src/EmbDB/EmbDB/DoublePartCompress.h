#ifndef _EMBEDDED_DATABASE_DOUBLE_PART_COMPRESS_H_
#define _EMBEDDED_DATABASE_DOUBLE_PART_COMPRESS_H_

#include "NumLenCompress.h"
#include "SignCompressor.h"
namespace embDB
{


	template<class _TValue, uint32 _nMaxBitsLens>
	class TDoublePartCompress
	{
		public:

			typedef _TValue TValue;
			TDoublePartCompress() : m_nError(100)
			{
				clear();
			}
			~TDoublePartCompress(){}


			void AddSymbol(TValue value)
			{



				bool bSign = value < 0;



				uint32 nBitLen = 0;
				value = abs(value);

				if(value == 0)
					nBitLen = 0;
				else if(value == 1)
					nBitLen = 1;
				else 
					nBitLen = TFindMostSigBit::FMSB(value);
				assert(nBitLen < _nMaxBitsLens + 1);

				m_BitsLensFreq[nBitLen] += 1;
				if(m_BitsLensFreq[nBitLen] == 1)
					m_nDiffsLen += 1;

				if(m_nTypeFreq != etfInt32)
				{
					if(m_BitsLensFreq[nBitLen] > 255)
						m_nTypeFreq = etfShort;
					if(m_BitsLensFreq[nBitLen] > 65535)
						m_nTypeFreq = etfInt32;
				}

				m_nLenBitSize  += nBitLen > 1 ? nBitLen - 1 : 0;
				m_nCount++;

				m_nSignCompressor.AddSymbol(value < 0 );
			}

			void RemoveSymbol(TValue value)
			{
				uint32 nBitLen = TFindMostSigBit::FMSB(value > 0 ? value : abs(value));
				assert(nBitLen < _nMaxBitsLens);

				assert(m_BitsLensFreq[nBitLen] );				

				m_BitsLensFreq[nBitLen] -= 1;
				if(m_BitsLensFreq[nBitLen] == 65535 || m_BitsLensFreq[nBitLen] == 255)
				{

					m_nTypeFreq = etfByte;
					for (uint32 i = 0; i < _nMaxBitsLens; ++i)
					{
						if(m_BitsLensFreq[i] > 65535)
						{
							m_nTypeFreq = etfInt32;
							break;
						}
						if(m_nTypeFreq != etfInt32)
						{
							if(m_BitsLensFreq[nBitLen] > 255)
								m_nTypeFreq = etfShort;
						}
					}
				}
				if(!m_BitsLensFreq[nBitLen])
					m_nDiffsLen--;

				m_nLenBitSize -= ((nBitLen + 1));
				m_nCount--;

				m_nSignCompressor.RemoveSymbol(value < 0 );
			}

			void clear()
			{
				memset(m_BitsLensFreq, 0, sizeof(m_BitsLensFreq));
				m_nDiffsLen = 0;
				m_nTypeFreq = etfByte;
				m_nCount = 0;
				m_nLenBitSize = 0;
			}


			double GetCodeBitSize() const
			{
				double dBitRowSize = CalcRowBitSize<uint32>(m_BitsLensFreq, _nMaxBitsLens, m_nDiffsLen, m_nCount);
				dBitRowSize += (dBitRowSize/m_nError)   + 64 /*code  finish*/; 

				return dBitRowSize;
			}
			int GetBitLenSize() const
			{
				return m_nLenBitSize;
			}

			uint32 GetCompressSize() const
			{
				double dRowBitsLen = GetCodeBitSize();
				uint32 nByteSize = (dRowBitsLen + 7)/8;
				uint32 nDiffLens = GetLenForDiffLen(m_nTypeFreq, m_nDiffsLen);
				uint32 nSignSize = m_nSignCompressor.GetCompressSize();
				
				return nByteSize + 1 + (_nMaxBitsLens + 7)/8 + nDiffLens + nSignSize + (GetBitLenSize() +7)/8 + 1;

			}
		private:
			uint32 m_BitsLensFreq[_nMaxBitsLens];
			uint32 m_nDiffsLen;
			uint32 m_nLenBitSize;
			eTypeFreq m_nTypeFreq;
			uint32 m_nCount;
			uint32 m_nError;

			TSignCompressor m_nSignCompressor;
			 
	};
}

#endif