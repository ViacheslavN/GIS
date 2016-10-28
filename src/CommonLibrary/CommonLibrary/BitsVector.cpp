#include "stdafx.h"
#include "BitsVector.h"
namespace CommonLib
{

	CBitsVector::CBitsVector(CommonLib::alloc_t *pAlloc , uint32 nSizeInByte ) :
			m_pAlloc(pAlloc), m_nCapacity(nSizeInByte), m_nSize(nSizeInByte), m_pBuffer(NULL), m_nSizeInBits(0)
	{
		
		if(!m_pAlloc)
			m_pAlloc = &m_alloc;


		resize(nSizeInByte);
	}

	CBitsVector::~CBitsVector()
	{ 

	}
	void CBitsVector::push_back(bool bBit)
	{
	   uint32 nByte = m_nSizeInBits/8;
	   uint32 nBitInByte = m_nSizeInBits%8;

	   if(nBitInByte == 7)
	   {
		   m_nSize += 1;
		   nByte = m_nSize;

		   nBitInByte = 0;
	   }
	   if(m_nSize == m_nCapacity)
	   {
		   reserve(m_nSize != 0 ? m_nSize *2 : 2);
	   }
	   byte   nMask = 0x01 << nBitInByte;
	   if(bBit)
		   m_pBuffer[nByte] |= nMask;
	   else
		   m_pBuffer[nByte] &= (~nMask);

		m_nSizeInBits += 1;
	}

	void CBitsVector::setBit(bool bBit, uint32 nBit)
	{
		assert(nBit < sizeInBits());
		if(nBit < m_nSizeInBits)
		{
			assert(nBit < sizeInBits());		 
			uint32 nByte = nBit/8;
			uint32 nBitInByte = nBit%8;
			byte   nMask = 0x01 << nBitInByte;
			if(bBit)
				m_pBuffer[nByte] |= nMask;
			else
				m_pBuffer[nByte] &= (~nMask);
		}

		

	}
	bool CBitsVector:: getBit(uint32 nBit) const
	{
		assert(nBit < sizeInBits());
		int32 nByte = nBit/8;
		uint32 nBitInByte = nBit%8;
		return ((m_pBuffer[nByte] >> nBitInByte) & 0x01) == 1;
	}

	bool CBitsVector::operator [](uint32 nIndex) const
	{
		return getBit(nIndex);
	}

	void CBitsVector::resize(uint32 nSizeInByte)
	{
		if(nSizeInByte <= m_nCapacity)
		{
			m_nSize = nSizeInByte;
			return;
		}

		byte* pBuffer = (byte*)m_pAlloc->alloc(nSizeInByte);
		if(m_pBuffer != NULL)
		{
			memcpy(pBuffer, m_pBuffer, m_nSize);
			m_pAlloc->free(m_pBuffer);
		}

		m_nCapacity = nSizeInByte;
		m_nSize = nSizeInByte;

		m_pBuffer = pBuffer;
	}
	void CBitsVector::resizeBits(uint32 nSizeInBits)
	{
		 resize((nSizeInBits + 7)/8);
	}


	void CBitsVector::reserve(uint32 nSizeInByte)
	{
		if(nSizeInByte <= m_nCapacity)
			return;

		byte* pBuffer = (byte*)m_pAlloc->alloc(nSizeInByte);
		if(m_pBuffer != NULL)
		{
			memcpy(pBuffer, m_pBuffer, m_nSize);
			m_pAlloc->free(m_pBuffer);


		}
		m_nCapacity = nSizeInByte;
		m_pBuffer = pBuffer;
			 
	}
	void reserveBits(uint32 nSizeInBits)
	{

	}

	uint32 CBitsVector::size() const
	{
		return m_nSize;
	}
	uint32 CBitsVector::sizeInBits()const
	{
		return m_nSizeInBits;
	}

	uint32 CBitsVector::capacity() const
	{
		return m_nCapacity;
	}
	uint32 CBitsVector::capacityInBits() const
	{
		return capacity()*8;
	}

	const byte *CBitsVector::bits() const
	{
		return m_pBuffer;
	}
	byte *CBitsVector::bits()
	{
		return m_pBuffer;
	}

	void CBitsVector::fill(bool bBit)
	{
		memset(m_pBuffer, bBit ? 0xff : 0, m_nCapacity);
	}

	void CBitsVector::clear(bool bDel)
	{
		m_nSizeInBits = 0;
		m_nSize = 0;

		if(bDel && m_pBuffer)
		{
			m_pAlloc->free(m_pBuffer);
			m_nCapacity = 0;
		}
	}
}
