#ifndef _EMBEDDED_DATABASE_NUM_LEN_COMPRESS_H_
#define _EMBEDDED_DATABASE_NUM_LEN_COMPRESS_H_

#include "CommonLibrary/FixedBitStream.h"
#include <map>
#include "MathUtils.h"
namespace embDB
{
	

	static int bits_lens[] = {0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3};
	struct TFindMostSigBit
	{

		
/*#ifdef _WIN32
		uint16 FMSB(uint32 x)
		{
			int32 y;
			__asm
			{
				bsr eax, x
				mov y, eax
			}
			return (uint16)y;
		}
#endif*/
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



	};

	template<class _TValue, class _TFindMostSigBit, int _nMaxBitsLens>
	class TUnsignedCalcNumLen
	{
		public:

			typedef _TValue TValue;
			typedef _TFindMostSigBit TFindBit;

	 

			TUnsignedCalcNumLen() : m_nLenBitSize(0), m_nCount(0), m_nDIffsLen(0)
			{

			}

			uint16 AddSymbol(TValue symbol)
			{
				uint16 nBitLen =  m_FindBit.FMSB(symbol);

				assert(nBitLen < _nMaxBitsLens);

				m_nLenBitSize += nBitLen;
				m_nCount++;

				if(!m_BitsLensFreq[nBitLen])
					m_nDIffsLen++;

				m_BitsLensFreq[nBitLen] += 1;

				//m_MapFreq[nBitLen] += 1;

				return nBitLen;
			}

			void RemoveSymbol(TValue symbol)
			{
				uint16 nBitLen =  m_FindBit.FMSB(symbol);
				m_nLenBitSize -= nBitLen;
				m_nCount--;

				m_BitsLensFreq[nBitLen] -= 1;
				if(!m_BitsLensFreq[nBitLen])
					m_nDIffsLen--;
			}
 
			 
			double GetCodeBitSize() const
			{
				double dBitRowSize = 0;
				for (uint32 i = 0; i < _nMaxBitsLens; ++i)
				{
					double dFreq = m_BitsLensFreq[i];
					double dLog2 = -1*mathUtils::Log2(dFreq/m_nCount); 
					dBitRowSize += dFreq* dLog2;

				}

				if(dBitRowSize < 32)
					dBitRowSize = 32;

				dBitRowSize += dBitRowSize*0.05;

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



				return nByteSize + m_nDIffsLen + 1 + m_nLenBitSize;

			}


			void compress(TBPVector<_TValue>& vecValues, CommonLib::IWriteStream* pStream)
			{

				uint16 ModelLens[_nMaxBitsLens];

				pStream->write(byte(m_nDIffsLen));
				for (uint32 i = 0, i < _nMaxBitsLens; ++i)
				{
					if(m_BitsLensFreq[i] != 0)
					{
						ModelLens[i] = 1;
						pStream->write(byte(i));
					}
					else
						ModelLens[i] = 0;
				}
				
			



			}

		private:	

			TFindBit    m_FindBit;
	  
			uint32 m_nLenBitSize;
			uint32 m_nCount;

			uint32 m_nDIffsLen;

			uint16  m_BitsLensFreq[_nMaxBitsLens];
	};
}
#endif