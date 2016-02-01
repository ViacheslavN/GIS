#include "stdafx.h"
#include "FixedBitStream.h"
namespace CommonLib
{


	FxBitWriteStream::FxBitWriteStream(alloc_t *pAlloc) : TBase(pAlloc)
	{}

	FxBitWriteStream::~FxBitWriteStream()
	{}

	void FxBitWriteStream::writeBit(bool bBit)
	{
		assert(m_nPos < m_nSize);
		if (m_nCurrBit > m_nBitBase)
		{
			m_nPos++;
			m_nCurrBit = 0;
			m_pBuffer[m_nPos] = 0;
		}
		if(bBit)
			m_pBuffer[m_nPos] |= (1 << m_nCurrBit);
		m_nCurrBit++;
	 
	}
	


	FxBitReadStream::FxBitReadStream(alloc_t *pAlloc) : TBase(pAlloc)
	{

	}
	FxBitReadStream::~FxBitReadStream(){}

	bool FxBitReadStream::readBit()
	{
		assert(m_nPos < m_nSize);
		
		bool bBit = m_pBuffer[m_nPos] & (1 << m_nCurrBit) ? true : false ;
		m_nCurrBit++;
		if (m_nCurrBit > m_nBitBase)
		{
			m_nPos++;
			m_nCurrBit = 0;
		} 

		return bBit;
	}
	
}