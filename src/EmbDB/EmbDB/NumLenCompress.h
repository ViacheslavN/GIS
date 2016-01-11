#ifndef _EMBEDDED_DATABASE_ARITHMETIC_CODER_H_
#define _EMBEDDED_DATABASE_ARITHMETIC_CODER_H_

#include "CommonLibrary/FixedBitStream.h"
#include <map>
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

	template<class _TValue, class _TFindMostSigBit>
	class TUnsignedCalcNumLen
	{
		public:

			typedef _TValue TValue;
			typedef _TFindMostSigBit TFindBit;

			typedef std::map<uint16, uint32> TLenFreq;

			TUnsignedCalcNumLen() : m_nLenBitSize(0), m_nCount(0)
			{

			}

			uint16 AddSymbol(TValue symbol)
			{
				uint16 nBitLen =  m_FindBit.FMSB(symbol);

				m_nLenBitSize += nBitLen;
				m_nCount++;

				m_MapFreq[nBitLen] += 1;

				return nBitLen;
			}

			void RemoveSymbol(TValue symbol)
			{
				uint16 nBitLen =  m_FindBit.FMSB(symbol);
				m_nLenBitSize -= nBitLen;
				m_nCount--;

				m_MapFreq[nBitLen] -= 1;
			}

			const TLenFreq& GetLenFreq() const
			{
				return m_MapFreq;
			}
			double Log2( double n )  const
			{  

				return log( n ) / log( (double)2 );  
			}
			double GetCodeBitSize() const
			{
				double dBitRowSize = 0;
				for (TLenFreq::const_iterator it = m_MapFreq.begin(); it != m_MapFreq.end(); ++it)
				{
					double dFreq = it->second;
					double dLog2 = -1*Log2(dFreq/m_nCount); 
					dBitRowSize += dFreq* dLog2;

				}

				return dBitRowSize;
			}
			int GetBitLenSize() const
			{
				return m_nLenBitSize;
			}

		private:	

			TFindBit    m_FindBit;
			TLenFreq m_MapFreq;
			uint32 m_nLenBitSize;
			uint32 m_nCount;
	};
}
#endif