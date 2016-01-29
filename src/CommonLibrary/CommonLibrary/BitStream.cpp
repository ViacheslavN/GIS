#include "stdafx.h"
#include "general.h"
#include "BitStream.h"

namespace CommonLib
{

	IReadBitStreamBase::IReadBitStreamBase()  
	{

	}
	IReadBitStreamBase::~IReadBitStreamBase(){

	}
	void IReadBitStreamBase::readBits(byte& nBits, size_t nCntBits)
	{
		assert(nCntBits < 9);
		_readBits<byte>(nBits, nCntBits); 
	}
	void IReadBitStreamBase::readBits(uint16&  nBits, size_t nCntBits)
	{
		assert(nCntBits < 17);
		_readBits<uint16>(nBits, nCntBits);
	}
	void IReadBitStreamBase::readBits(uint32&  nBits, size_t nCntBits)
	{
		assert(nCntBits < 33);
		_readBits<uint32>(nBits, nCntBits); 
	}
	void IReadBitStreamBase::readBits(uint64&  nBits, size_t nCntBits)
	{
		assert(nCntBits < 65);
		_readBits<uint64>(nBits, nCntBits); 
	}



	IWriteBitStreamBase::IWriteBitStreamBase()  {}
	IWriteBitStreamBase::~IWriteBitStreamBase()  {}

	void IWriteBitStreamBase::writeBit(byte nBit)
	{
				
		bool bitValue = (nBit & 0x01) ? true : false;
		writeBit(bitValue);

	}

	void IWriteBitStreamBase::writeBits(byte nBits, size_t nCntBits)
	{
		assert(nCntBits < 9);
		_writeBits<byte>(nBits, nCntBits); 
	}
	void IWriteBitStreamBase::writeBits(uint16 nBits, size_t nCntBits)
	{
		assert(nCntBits < 17);
		_writeBits<uint16>(nBits, nCntBits); 

	}
	void IWriteBitStreamBase::writeBits(uint32 nBits, size_t nCntBits)
	{
		assert(nCntBits < 33);
		_writeBits<uint32>(nBits, nCntBits); 
	}
	void IWriteBitStreamBase::writeBits(uint64 nBits, size_t nCntBits)
	{
		assert(nCntBits < 65);
		_writeBits<uint64>(nBits, nCntBits); 
	}



	BitStreamBase::BitStreamBase(alloc_t *pAlloc ):
	m_pAlloc(pAlloc)
		,m_pBuffer(0)
		,m_nPos(0)
		,m_nSize(0)
		,m_bAttach(false)
		,m_nCurrBit(0)
	{

	}
	BitStreamBase::~BitStreamBase()
	{
		if(!m_bAttach && m_pAlloc && m_pBuffer)
		{
			m_pAlloc->free(m_pBuffer);
			m_pBuffer = 0;
		}
	}


	bool BitStreamBase::create(uint32 nSize)
	{
		assert(m_pAlloc);
		if(m_pBuffer && !m_bAttach)
		{
			m_pAlloc->free(m_pBuffer);
		}
		m_pBuffer = (byte*)m_pAlloc->alloc(sizeof(byte) * nSize);
		m_nPos = 0;
		m_nSize = nSize;
		m_bAttach = false;
		m_nCurrBit = 0;
		m_pBuffer[m_nPos] = 0;
		m_nEndBits = m_nBitBase;

		return m_pBuffer != NULL;
	}
	bool BitStreamBase::attach(byte* pBuffer, uint32 nSize, bool bCopy)
	{

		if(bCopy)
		{
			create(nSize);
			memcpy(m_pBuffer, pBuffer, nSize);
			m_bAttach = false;
		}
		else
		{
			m_pBuffer = pBuffer;
			m_bAttach = true;

		}
		m_nCurrBit = 0;
		m_nPos = 0;
		m_nSize = nSize;
		m_nEndBits = m_nBitBase;

		return true;

	}
	void BitStreamBase::attachBits(byte* pBuffer, uint32 nSizeInBits, bool bCopy)
	{

		m_nSize = nSizeInBits/m_nBitBase + 1;
		uint32 nEndBit = nSizeInBits%m_nBitBase;
		m_nEndBits = nEndBit ? nEndBit : m_nBitBase;
		if(bCopy)
		{
			create(m_nSize);
			memcpy(m_pBuffer, pBuffer, m_nSize);
			m_bAttach = false;
		}
		else
		{
			m_pBuffer = pBuffer;
			m_bAttach = true;

		}
		m_nCurrBit = 0;
		m_nPos = 0;

	}
	byte* BitStreamBase::deattach()
	{
		return m_pBuffer;
	}
	byte* BitStreamBase::buffer()
	{
		return m_pBuffer;
	}
	const byte* BitStreamBase::buffer() const
	{
		return m_pBuffer;
	}
	int32 BitStreamBase::size() const
	{
		return m_nSize;
	}
	int64 BitStreamBase::size64() const
	{
		return (int64)m_nSize;
	}
	uint32 BitStreamBase::sizeInBits() const
	{
		if(!m_nSize)
			return 0;

		return (m_nSize - 1) * m_nBitBase  + m_nEndBits;
	}
	bool BitStreamBase::seek64(uint64 pos, enSeekOffset offset )
	{
		return seek(uint32(pos), offset);
	}

	bool BitStreamBase::seek(uint32 pos, enSeekOffset offset )
	{
		if(!m_pBuffer)
			return false;

		uint32 newpos = 0;
		switch(offset)
		{
		case soFromBegin:
			newpos = pos;
			break;
		case soFromCurrent:
			newpos = m_nPos + pos;
			break;
		case soFromEnd:
			newpos = m_nSize - pos;
			break;
		}
		if(newpos > m_nSize)
		{
			assert(false);
			return false;
		}
		m_nPos = newpos;
		return true;
	}
	int32 BitStreamBase::pos() const
	{
		return m_nPos;
	}
	int64 BitStreamBase::pos64() const
	{
		return (int64)m_nPos;
	}
	uint32 BitStreamBase::posInBits() const
	{

		return m_nPos* (m_nBitBase + 1) + m_nCurrBit;
	}
	void BitStreamBase::reset()
	{
		m_nPos = 0;
		m_nCurrBit = 0;
	}
	void BitStreamBase::close()
	{
		m_nPos = 0;
		m_nSize = 0;
		m_nCurrBit = 0;
		if(m_pBuffer && !m_bAttach && m_pAlloc)
		{
			m_pAlloc->free(m_pBuffer);
		}
		m_pBuffer = 0;
	}



}