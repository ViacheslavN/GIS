#include "stdafx.h"
#include "BitMap.h"

namespace embDB
{
	CBitMap::CBitMap() : m_pBuf(NULL), m_nSize(0)
	{

	}
	CBitMap::CBitMap(uint32 nSizeInByte)
	{

		if(nSizeInByte > 0)
		{
			m_pBuf = new byte[nSizeInByte];
			m_nSize = nSizeInByte; 
		}
		else
		{
			m_pBuf = NULL;
			m_nSize = 0;
		}
	}
	CBitMap::CBitMap(byte *pBuf, uint32 nSize)
	{
		if(pBuf && nSize > 0)
		{
			m_pBuf = new byte[nSize];
			memcpy(m_pBuf, pBuf, nSize);
			m_nSize = nSize; 
		}
		else
		{
			m_pBuf = NULL;
			m_nSize = 0;
		}

	}
	CBitMap::CBitMap(const CBitMap& bm)
	{
		if(bm.m_pBuf && bm.m_nSize > 0)
		{
			m_pBuf = new byte[bm.m_nSize];
			memcpy(m_pBuf, bm.m_pBuf, bm.m_nSize);
		}
		else
		{
			m_pBuf = NULL;
		}

		m_nSize = bm.m_nSize; 
	}

	bool CBitMap::init(uint32 nSize)
	{
		if(nSize == 0)
			return false;

		if(m_pBuf)
			delete m_pBuf;

		m_pBuf = new byte[nSize];
		memset(m_pBuf, 0, nSize);
		m_nSize = nSize; 
		return true;
	}
	CBitMap::~CBitMap()
	{
		if(m_pBuf)
			delete m_pBuf;
	}

	void CBitMap::setBits(byte *pBuf, uint32 nSize)
	{
		if(m_pBuf)
			delete m_pBuf;
		m_pBuf = new byte[nSize];
		memcpy(m_pBuf, pBuf, nSize);
		m_nSize = nSize; 
	}
	byte *CBitMap::getBits()
	{
		return m_pBuf;
	}
	uint32 CBitMap::getBitSize() const
	{
		return m_nSize *8;
	}
	uint32 CBitMap::size() const
	{
		return m_nSize;
	}

	bool CBitMap::getBit(uint32 nBit) const
	{
		if(nBit < getBitSize())
		{
			uint32 nByte = nBit/8;
			uint32 nBitInByte = nBit%8;
			return ((m_pBuf[nByte] >> nBitInByte) & 0x01) == 1;
		}
		return false;
	}
	bool CBitMap::getBit(uint32 nBit, uint32& nVal) const
	{
		if(nBit < getBitSize())
		{
			uint32 nByte = nBit/8;
			uint32 nBitInByte = nBit%8;
			nVal = (m_pBuf[nByte] >> nBitInByte) & 0x01;
			return true;
		}
		return false;
	}
	bool CBitMap::setBit(uint32 nBit, bool bBit)
	{
		if(nBit < getBitSize())
		{
			uint32 nByte = nBit/8;
			uint32 nBitInByte = nBit%8;
			byte   nMask = 0x01 << nBitInByte;
			if(bBit)
				m_pBuf[nByte] |= nMask;
			else
				m_pBuf[nByte] &= (~nMask);
			return true;
		}
		return false;
	}
}