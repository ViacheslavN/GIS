#ifndef _EMBEDDED_DATABASE_ARITHMETIC_CODER_H_
#define _EMBEDDED_DATABASE_ARITHMETIC_CODER_H_

#include "CommonLibrary/FixedBitStream.h"
#include <map>
namespace embDB
{

	struct TUint32Log2
	{
		uint16 log2(uint32 x)
		{
			int32 y;
			__asm
			{
				bsr eax, x
				mov y, eax
			}
			return (uint16)y;
		}
	};

	template<class _TValue, class _TLog2>
	class TUnsignedCalcNumLen
	{
		public:

			typedef _TValue TValue;
			typedef _TLog2 TLog2;

			typedef std::map<uint16, uint32> TLenFreq;

			TUnsignedCalcNumLen() : m_nLenBitSize(0), m_nCount(0)
			{

			}

			uint16 addSymbol(TValue symbol)
			{
				uint16 nBitLen =  m_Log2.log2(symbol);

				m_nLenBitSize += nBitLen;
				m_nCount++;

				return nBitLen;
			}

			const TLenFreq& GetLenFreq() const
			{
				return m_MapFreq;
			}


	 

		private:	

			TLog2    m_Log2;
			TLenFreq m_MapFreq;
			uint32 m_nLenBitSize;
			uint32 m_nCount;
	};
}
#endif