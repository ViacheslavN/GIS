#include "stdafx.h"
#include "FixedBitStream.h"
namespace CommonLib
{
	FxBitStreamBase::FxBitStreamBase(alloc_t *pAlloc ):
	m_pAlloc(pAlloc)
	,m_pBuffer(0)
	,m_nPos(0)
	,m_nSize(0)
	,m_bAttach(false)
	,m_nCurrBit(0)
	{

	}
	FxBitStreamBase::~FxBitStreamBase()
	{
		if(!m_bAttach && m_pAlloc && m_pBuffer)
		{
			m_pAlloc->free(m_pBuffer);
			m_pBuffer = 0;
		}
	}


	void FxBitStreamBase::create(size_t nSize)
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
	}
	void FxBitStreamBase::attach(byte* pBuffer, size_t nSize, bool bCopy)
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
		
	}
	void FxBitStreamBase::attachBits(byte* pBuffer, size_t nSizeInBits, bool bCopy)
	{

		m_nSize = nSizeInBits/m_nBitBase + 1;
		size_t nEndBit = nSizeInBits%m_nBitBase;
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
	byte* FxBitStreamBase::deattach()
	{
		return m_pBuffer;
	}
	byte* FxBitStreamBase::buffer()
	{
		return m_pBuffer;
	}
	const byte* FxBitStreamBase::buffer() const
	{
		return m_pBuffer;
	}
	size_t FxBitStreamBase::size() const
	{
		return m_nSize;
	}
	size_t FxBitStreamBase::sizeInBits() const
	{
		if(!m_nSize)
			return 0;

		return (m_nSize - 1) * m_nBitBase  + m_nEndBits;
	}

	
	bool FxBitStreamBase::seek(size_t pos, enSeekOffset offset )
	{
		if(!m_pBuffer)
			return false;

		size_t newpos = 0;
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
	size_t FxBitStreamBase::pos() const
	{
			return m_nPos;
	}
	size_t FxBitStreamBase::posInBits() const
	{

		return m_nPos* (m_nBitBase + 1) + m_nCurrBit;
	}
	void FxBitStreamBase::reset()
	{
		m_nPos = 0;
		m_nCurrBit = 0;
	}
	void FxBitStreamBase::close()
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



	FxBitWriteStream::FxBitWriteStream(alloc_t *pAlloc) : FxBitStreamBase(pAlloc)
	{}

	FxBitWriteStream::~FxBitWriteStream()
	{}

	void FxBitWriteStream::attach(byte* pBuffer, size_t nSize)
	{
		FxBitStreamBase::attach(pBuffer, nSize);
		if(m_nSize)
			m_pBuffer[m_nPos] = 0;
	}

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
	void FxBitWriteStream::writeBit(byte nBit)
	{
		assert(m_nPos < m_nSize);
		BYTE bitValue = (nBit & 0x01);
		if (m_nCurrBit > m_nBitBase)
		{
			m_nPos++;
			m_nCurrBit = 0;
			m_pBuffer[m_nPos] = 0;
		}

		m_pBuffer[m_nPos] |= (bitValue << m_nCurrBit);
		m_nCurrBit++;
		 
	}
	void FxBitWriteStream::writeBits(byte nBits, size_t nCntBits)
	{
		assert(nCntBits < 9);
		_writeBits<byte>(nBits, nCntBits); 
	}
	void FxBitWriteStream::writeBits(uint16 nBits, size_t nCntBits)
	{
		assert(nCntBits < 17);
		_writeBits<uint16>(nBits, nCntBits); 
	
	}
	void FxBitWriteStream::writeBits(uint32 nBits, size_t nCntBits)
	{
		assert(nCntBits < 33);
		_writeBits<uint32>(nBits, nCntBits); 
	}
	void FxBitWriteStream::writeBits(uint64 nBits, size_t nCntBits)
	{
		assert(nCntBits < 65);
		_writeBits<uint64>(nBits, nCntBits); 
	}



	FxBitReadStream::FxBitReadStream(alloc_t *pAlloc) : FxBitStreamBase(pAlloc)
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
	void  FxBitReadStream::readBit(byte & nBit)
	{
		assert(m_nPos < m_nSize);
		nBit = (m_pBuffer[m_nPos] & (1 << m_nCurrBit)) ? 1 : 0;
		m_nCurrBit++;
		if(m_nCurrBit > m_nBitBase)
		{
			m_nPos++;
			m_nCurrBit = 0;
		}
	}
	void FxBitReadStream::readBits(byte& nBits, size_t nCntBits)
	{
		assert(nCntBits < 9);
		_readBits<byte>(nBits, nCntBits); 
	}
	void FxBitReadStream::readBits(uint16&  nBits, size_t nCntBits)
	{
		assert(nCntBits < 17);
		_readBits<uint16>(nBits, nCntBits); 
	}
	void FxBitReadStream::readBits(uint32&  nBits, size_t nCntBits)
	{
		assert(nCntBits < 33);
		_readBits<uint32>(nBits, nCntBits); 
	}
	void FxBitReadStream::readBits(uint64&  nBits, size_t nCntBits)
	{
		assert(nCntBits < 65);
		_readBits<uint64>(nBits, nCntBits); 
	}
}