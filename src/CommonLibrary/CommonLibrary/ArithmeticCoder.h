#ifndef _EMBEDDED_DATABASE_ARITHMETIC_CODER_H_
#define _EMBEDDED_DATABASE_ARITHMETIC_CODER_H_

#include "stream.h"
#include "BaseACCoder.h"
namespace CommonLib
{
	template<class _TCodeValue, uint16 _nValueBits>
	class TACEncoder : public BaseACEncoder<_TCodeValue>
	{
	public:

		typedef _TCodeValue TCodeValue;
		static const TCodeValue  _TopValue = (((TCodeValue)1 << _nValueBits) - 1);
		static const TCodeValue  _FirstQuarter = (_TopValue/4  + 1);
		static const TCodeValue  _Half = (2 * _FirstQuarter);
		static const TCodeValue  _ThirdQuarter = (3 * _FirstQuarter);
		static const _TCodeValue MaxRange = _FirstQuarter - 1;

		TACEncoder(IWriteStream* pStream, uint32 nMaxSize = 0) : m_pStream(pStream), m_nLow(0), m_nHigh(_TopValue), m_nScale(0),
			m_nBitsBuf(0), m_nCurrBit(0), m_nMaxSize(nMaxSize)
#ifdef _DEBUG
			, m_nBitsWrite(0)
#endif

		{}

		void BitsPlusFollow(bool bBit)
		{
			writeBit(bBit);

			for (; m_nScale > 0; m_nScale--)
			{
				writeBit(!bBit);
			}


		}
		virtual bool EncodeSymbol(TCodeValue nLowCount, TCodeValue nHightCount, TCodeValue nTotalCount)
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
			return true;
		}
		virtual bool EncodeFinish()
		{
			m_nScale += 1;  
			if (m_nLow < _FirstQuarter) 
				BitsPlusFollow(false); 

			else BitsPlusFollow(true);     

			if(m_nCurrBit != 0)
			{
				m_pStream->write(m_nBitsBuf);
			}
			return true;
				 
		}

		void writeBit(bool bBit)
		{
			if(m_nCurrBit > 7)
			{
				m_pStream->write(m_nBitsBuf);
				m_nBitsBuf = 0;
				m_nCurrBit = 0;
			}
			if(bBit)
				m_nBitsBuf |= (1 << m_nCurrBit);
			m_nCurrBit++;
#ifdef _DEBUG
			m_nBitsWrite++;
#endif
		}

	private:
		IWriteStream* m_pStream;
		TCodeValue m_nLow;
		TCodeValue m_nHigh;
		uint32	   m_nScale;
#ifdef _DEBUG
		uint32	   m_nBitsWrite;
#endif
		byte	   m_nBitsBuf;
		uint32	   m_nCurrBit;
		uint32	   m_nMaxSize;
	};

	template<class _TCodeValue, uint16 _nValueBits>
	class TACDecoder : public BaseACDecoder<_TCodeValue>
	{
	public:

		typedef _TCodeValue TCodeValue;
		static const TCodeValue  _TopValue = (((TCodeValue)1 << _nValueBits) - 1);
		static const TCodeValue  _FirstQuarter = (_TopValue/4  + 1);
		static const TCodeValue  _Half = (2 * _FirstQuarter);
		static const TCodeValue  _ThirdQuarter = (3 * _FirstQuarter);
		static const _TCodeValue MaxRange = _FirstQuarter - 1;

		TACDecoder(IReadStream* pStream) : m_pStream(pStream), m_nLow(0), m_nHigh(_TopValue),
			m_nBitsRead(0), m_nValue(0), m_nBitsBuf(0), m_nCurrBit(8)
		{}

		TCodeValue GetBit()
		{

			if(m_nCurrBit > 7 && (m_pStream->pos() == m_pStream->size()))
				return 0;

			if (m_nCurrBit > 7)
			{
				m_nCurrBit = 0;
				m_nBitsBuf = m_pStream->readByte();
			} 

			TCodeValue nBit = (m_nBitsBuf & (1 << m_nCurrBit)) ? 1 : 0;
			m_nCurrBit++;
			return nBit;
		}
		virtual void StartDecode()
		{
			for (int i = 1; i<= _nValueBits; i++)
			{

				TCodeValue nBit = GetBit();
				m_nValue = ( m_nValue << 1 ) | nBit;
			}
		}

		virtual TCodeValue GetFreq(TCodeValue nTotalCount)
		{
			return ((m_nValue - m_nLow + 1) * nTotalCount -1)/(m_nHigh - m_nLow + 1);
		}

		virtual void DecodeSymbol(TCodeValue lowCount, TCodeValue highCount, TCodeValue nTotal)
		{
			TCodeValue nRange = m_nHigh - m_nLow + 1; 
			m_nHigh = m_nLow + (nRange* highCount)/nTotal - 1;
			m_nLow = m_nLow + (nRange * lowCount)/nTotal;

			for (;;)
			{
				if(m_nHigh < _Half)
					;
				else if(m_nLow >= _Half)
				{
					m_nLow -= _Half;
					m_nHigh -= _Half;
					m_nValue -=_Half;

				}
				else if ((m_nLow >= _FirstQuarter) && (m_nHigh < _ThirdQuarter))
				{
					m_nLow -= _FirstQuarter;
					m_nHigh -= _FirstQuarter;
					m_nValue -=_FirstQuarter;
				}
				else
					break;
				m_nLow = 2 *m_nLow;
				m_nHigh = (2 *m_nHigh)  +1;


				m_nValue = (2 * m_nValue) + GetBit();
			}
		}


	private:
		IReadStream* m_pStream;
		TCodeValue m_nLow;
		TCodeValue m_nHigh;
		uint32	   m_nBitsRead;
		TCodeValue m_nValue;
		byte	   m_nBitsBuf;
		uint32	   m_nCurrBit;
	};


	typedef TACEncoder<uint64, 32> TACEncoder64;
	typedef TACDecoder<uint64, 32> TACDecoder64;

	typedef TACEncoder<uint32, 16> TACEncoder32;
	typedef TACDecoder<uint32, 16> TACDecoder32;
}

#endif