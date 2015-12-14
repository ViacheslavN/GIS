#ifndef _EMBEDDED_DATABASE_AC_CODER_H_
#define _EMBEDDED_DATABASE_AC_CODER_H_
#include "CommonLibrary/WriteBitStream.h"
namespace embDB
{
	template<class _TCodeValue, uint16 _nValueBits>
	class TACEncoder
	{
		public:

			typedef _TCodeValue TCodeValue;
			static const TCodeValue  _TopValue = (((TCodeValue)1 << _nValueBits) - 1);
			static const TCodeValue  _FirstQuarter = (_TopValue/4  + 1);
			static const TCodeValue  _Half = (2 * _FirstQuarter);
			static const TCodeValue  _ThirdQuarter = (3 * _FirstQuarter);

			TACEncoder(CommonLib::WriteBitStream* pBitStream) : m_pBitStream(pBitStream), m_nLow(0), m_nHigh(_TopValue), m_nScale(0), m_bBitsWrite(0)
			{}

			void BitsPlusFollow(bool bBit)
			{
				m_pBitStream->writeBit(bBit);
			
				for (; m_nScale > 0; m_nScale--)
				{
					m_pBitStream->writeBit(!bBit);
				}

				m_bBitsWrite += (1 + m_nScale);
			}
			void EncodeSymbol(TCodeValue nLowCount, TCodeValue nHightCount, TCodeValue nTotalCount)
			{
				TCodeValue range = m_nHigh - m_nLow + 1; 
		
				m_nHigh = m_nLow + (range* nHightCount)/nTotalCount - 1;
				m_nLow = m_nLow + (range * nLowCount )/nTotalCount;
				for (;;)
				{
					if(m_nHigh < _Half)
					{
						BitsPlusFollow(false);
					}
					else if(m_nLow >= _Half)
					{
						BitsPlusFollow(true);
						m_nLow -= _Half;
						m_nHigh -= _Half;
					}
					else if((m_nLow >= _FirstQuarter) && (m_nHigh < _ThirdQuarter))
					{
						m_nScale++;
						m_nLow -= _FirstQuarter;
						m_nHigh -= _FirstQuarter;
					}
					else
						break;

					m_nLow += m_nLow;
					m_nHigh += (m_nHigh  + 1 );
				}
			}
			void EncodeFinish()
			{
				m_nScale += 1;  
				if (m_nLow < _FirstQuarter) 
					BitsPlusFollow(false); 

				else BitsPlusFollow(true);     

			}

		private:
			CommonLib::WriteBitStream* m_pBitStream;
			TCodeValue m_nLow;
			TCodeValue m_nHigh;
			uint32	   m_nScale;
			uint32	   m_bBitsWrite;
	};
}

#endif