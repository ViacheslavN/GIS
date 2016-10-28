#include "stdafx.h"
#include "BitsMap.h"

namespace CommonLib
{

	CBitsMap::CBitsMap(CommonLib::alloc_t *pAlloc , uint32 nSizeInByte , bool bInitBit ) :
		m_blob(pAlloc), m_bInitBit(bInitBit)
	{
		m_blob.resize_fill(nSizeInByte,bInitBit ? 0xff : 0);

	}
	CBitsMap::~CBitsMap()
	{

	}
	void CBitsMap::setBit(bool bBit, uint32 nBit)
	{
		assert(nBit < sizeInBits());		 
		uint32 nByte = nBit/8;
		uint32 nBitInByte = nBit%8;
		byte   nMask = 0x01 << nBitInByte;
		if(bBit)
			m_blob[nByte] |= nMask;
		else
			m_blob[nByte] &= (~nMask);
	}
	bool CBitsMap::getBit(uint32 nBit) const
	{
		assert(nBit < sizeInBits());
		int32 nByte = nBit/8;
		uint32 nBitInByte = nBit%8;
		return ((m_blob[nByte] >> nBitInByte) & 0x01) == 1;
	}

	void CBitsMap::resize(uint32 nSizeInByte)
	{
		m_blob.resize_fill(nSizeInByte, m_bInitBit ? 0xff : 0);
	}
	void  CBitsMap::resizeBits(uint32 nSizeInBits)
	{
		m_blob.resize_fill((nSizeInBits + 7)/8, m_bInitBit ? 0xff : 0);
	}
	uint32 CBitsMap::size() const
	{
		return m_blob.size();
	}
	uint32 CBitsMap::sizeInBits() const
	{
		return m_blob.size() * 8;
	}

	const byte *CBitsMap::bits() const
	{
		return m_blob.buffer();
	}
	byte *CBitsMap::bits()
	{
		return m_blob.buffer();
	}

	void CBitsMap::fill(bool bBit)
	{
		memset(m_blob.buffer(), bBit ? 0xff : 0, m_blob.size());
		 
	}

	/*uint32 CBitsMap::getBitCapacity() const
	{
		return m_blob.capacity() * 8;
	}*/
}