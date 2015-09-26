#include "stdafx.h"
#include "FixedMemoryStream.h"
namespace CommonLib
{
	FxStreamBase::FxStreamBase(alloc_t *pAlloc) :
		m_pAlloc(pAlloc)
		,m_pBuffer(0)
		,m_nPos(0)
		,m_nSize(0)
		,m_bAttach(false)
	{
		m_bIsBigEndian = IStream::isBigEndian();
		if(!m_pAlloc)
			m_pAlloc = &m_alloc;
	}
	FxStreamBase::~FxStreamBase()
	{
		if(!m_bAttach && m_pAlloc && m_pBuffer)
		{
			m_pAlloc->free(m_pBuffer);
			m_pBuffer = 0;
		}
	}


	void FxStreamBase::create(size_t nSize)
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
	}
	void FxStreamBase::attach(byte* pBuffer, size_t nSize, bool bCopy)
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
		m_nPos = 0;
		m_nSize = nSize;
	}
	byte* FxStreamBase::deattach()
	{
		byte* tmp = m_pBuffer;
		m_nPos = 0;
		m_nSize = 0;
		m_pBuffer = 0;
		m_bAttach = false;
		return tmp;
	}
	byte* FxStreamBase::buffer()
	{
		return m_pBuffer;
	}
	const byte*  FxStreamBase::buffer() const
	{
		return m_pBuffer;
	}
	size_t FxStreamBase::size() const
	{
		return m_nSize;
	}
	bool FxStreamBase::seek(size_t pos, enSeekOffset offset )
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
	size_t FxStreamBase::pos() const
	{
		return m_nPos;
	}
	void FxStreamBase::reset()
	{
			m_nPos = 0;
	}
	void FxStreamBase::close()
	{
		m_nPos = 0;
		m_nSize = 0;
		if(m_pBuffer && !m_bAttach && m_pAlloc)
		{
			m_pAlloc->free(m_pBuffer);
		}
		m_pBuffer = 0;
	}
	
	
	void FxMemoryReadStream::read_bytes(byte* dst, size_t size)
	{
		::memcpy(dst, m_pBuffer + m_nPos, size);
		m_nPos += size;
		assert(m_nPos <= m_nSize);
	}

	void FxMemoryReadStream::read_inverse(byte* buffer, size_t size)
	{
		for(size_t i = 0; i < size; m_nPos++, i++)
			buffer[i] = m_pBuffer[m_nPos + size - i - 1];
		m_nPos += size;
		assert(m_nPos <= m_nSize);
	}

	bool FxMemoryReadStream::checkRead(uint32 nSize) const
	{
		return (m_nSize - m_nPos) >= nSize;
	}
	bool FxMemoryReadStream::AttachStream(IStream *pStream, uint32 nSize, bool bSeek)
	{
		if(!checkRead(nSize))
			return false;
		pStream->attach(buffer() + pos(), nSize);
		if(bSeek)
			seek(nSize, soFromCurrent);
		return true;
	}
	bool FxMemoryReadStream::IsEndOfStream() const
	{
		return m_nSize == m_nPos;
	}
	FxMemoryReadStream::FxMemoryReadStream(alloc_t *pAlloc) : FxStreamBase(pAlloc)
	{

	}

	FxMemoryReadStream::FxMemoryReadStream(byte* pBuffer, uint32 nSize, bool bAttach, alloc_t *pAlloc) : FxStreamBase(pAlloc)
	{
		if(bAttach)
		{
			m_pBuffer = pBuffer;
			m_bAttach = true;
		}
		else
		{
			m_pBuffer = (byte*)m_pAlloc->alloc(sizeof(byte) * nSize);
			memcpy(m_pBuffer, pBuffer, nSize);
		}
		
		m_nPos = 0;
		m_nSize = nSize;
	}

	FxMemoryReadStream::~FxMemoryReadStream()
	{

	}
	
	
	FxMemoryWriteStream::FxMemoryWriteStream(alloc_t *pAlloc) : FxStreamBase(pAlloc)
	{

	}
	FxMemoryWriteStream::~FxMemoryWriteStream()
	{

	}


	void FxMemoryWriteStream::write_bytes(const byte* buffer, size_t size)
	{
		if(size > 0)
		{
			::memcpy(m_pBuffer + m_nPos, buffer, size);
			m_nPos += size;
		}
		assert(m_nPos <= m_nSize);
	}
	void FxMemoryWriteStream::write_inverse(const byte* buffer, size_t size)
	{
		for(size_t i = 0; i < size; m_nPos++, i++)
			m_pBuffer[m_nPos + size - i - 1] = buffer[i];
		assert(m_nPos <= m_nSize);
	}
	
	/*void  FxMemoryWriteStream::write(const byte* pBuffer, size_t bufLen )
	{
		if(m_bIsBigEndian)
			write_inverse(pBuffer, bufLen);
		else
			write_bytes(pBuffer, bufLen);
	}
	void FxMemoryWriteStream::write(bool value)
	{
		writeT<byte>(value ? (byte)1 : (byte)0);
	}
	void FxMemoryWriteStream::write(byte value)
	{
		writeT<byte>(value);
	}
	void FxMemoryWriteStream::write(char value)
	{
		writeT<char>(value);
	}
	void FxMemoryWriteStream::write(int16 value)
	{
		writeT<int16>(value);
	}
	void FxMemoryWriteStream::write(uint16 value)
	{
		writeT<uint16>(value);
	}

	void FxMemoryWriteStream::write(uint32 value)
	{
		writeT<uint32>(value);
	}

	void FxMemoryWriteStream::write(int32 value)
	{
		writeT<int32>(value);
	}

	void FxMemoryWriteStream::write(int64 value)
	{
		writeT<int64>(value);
	}
	void FxMemoryWriteStream::write(uint64 value)
	{
		writeT<uint64>(value);
	}

	void FxMemoryWriteStream::write(float value)
	{
		writeT<float>(value);
	}

	void FxMemoryWriteStream::write(double value)
	{
		writeT<double>(value);
	}
	void FxMemoryWriteStream::write(const CommonLib::str_t& str)
	{
		writeT<uint32>(str.length());
		if(str.length())
			write((byte*)str.cwstr(), str.length() *2);
	}*/
}