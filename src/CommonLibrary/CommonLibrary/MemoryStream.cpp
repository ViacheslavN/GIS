#include "stdafx.h"
#include "MemoryStream.h"
namespace CommonLib
{
	MemoryStream::MemoryStream(alloc_t *pAlloc) :
	m_pAlloc(pAlloc)
	,m_pBuffer(0)
	,m_nPos(0)
	,m_nSize(0)
	,m_bAttach(false)
	{
		if(!m_pAlloc)
			m_pAlloc = &m_alloc;

		m_bIsBigEndian = IStream::isBigEndian();
	}
	MemoryStream::~MemoryStream()
	{
		if(!m_bAttach && m_pBuffer)
		{
			m_pAlloc->free(m_pBuffer);
			m_pBuffer = 0;
		}
	}

	//IStream
	void MemoryStream::create(uint32 nSize)
	{
		if(m_pBuffer && !m_bAttach)
		{
			m_pAlloc->free(m_pBuffer);
		}
		m_pBuffer = (byte*)m_pAlloc->alloc(sizeof(byte) * nSize);
		m_nPos = 0;
		m_nSize = nSize;
		m_bAttach = false;
	}
	void MemoryStream::attach(byte* pBuffer, uint32 nSize, bool bCopy)
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
	byte* MemoryStream::deattach()
	{
		byte* tmp = m_pBuffer;
		m_nPos = 0;
		m_nSize = 0;
		m_pBuffer = 0;
		m_bAttach = false;
		return tmp;
	}
	byte* MemoryStream:: buffer()
	{
		return m_pBuffer;
	}
	const byte*  MemoryStream:: buffer() const
	{
		return m_pBuffer;
	}
	uint32 MemoryStream::size() const
	{
		return m_nSize;
	}

	bool MemoryStream::seek(uint32 pos, enSeekOffset offset  )
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
			newpos = m_nSize + pos;
			break;
		}
		if(newpos > m_nSize && m_bAttach)
			return false;
		if(newpos > m_nSize && !m_bAttach)
			resize(newpos - m_nSize);
		m_nPos = newpos;
		return true;

	}
	uint32 MemoryStream::pos() const
	{
		return m_nPos;
	}
	void MemoryStream::reset()
	{
		m_nPos = 0;
	}
	void MemoryStream::close()
	{
		m_nPos = 0;
		m_nSize = 0;
		m_pBuffer = 0;
	}
	void  MemoryStream::resize(uint32 nSize)
	{
		uint32 newSize = m_nSize;

		while(m_nPos + nSize > newSize)
			newSize = uint32(newSize * 1.5) + 1;
		if(newSize > m_nSize)
		{
			assert(!m_bAttach);
			m_nSize = newSize;
			byte* buffer =  (byte*)m_pAlloc->alloc(sizeof(byte) * newSize);
			if(m_pBuffer)
			{
				memcpy(buffer, m_pBuffer, m_nPos);
				if(!m_bAttach)
				{
					m_pAlloc->free(m_pBuffer);
				}
			}
			m_pBuffer = buffer;
		 
		}
	}


	void MemoryStream::read_bytes(byte* dst, uint32 size)
	{
		::memcpy(dst, m_pBuffer + m_nPos, size);
		m_nPos += size;
		assert(m_nPos <= m_nSize);
	}

	void MemoryStream::read_inverse(byte* buffer, uint32 size)
	{
		for(int64 i = 0; i < size; m_nPos++, i++)
			buffer[i] = m_pBuffer[m_nPos + size - i - 1];
		m_nPos += size;
		assert(m_nPos <= m_nSize);
	}
	void MemoryStream::write_bytes(const byte* buffer, uint32 size)
	{
		if(size > 0)
		{
			resize(size);
			::memcpy(m_pBuffer + m_nPos, buffer, size);
			m_nPos += size;
		}
		assert(m_nPos <= m_nSize);
	}
	void MemoryStream::write_inverse(const byte* buffer, uint32 size)
	{
		resize(size);
		for(size_t i = 0; i < size; m_nPos++, i++)
			m_pBuffer[m_nPos + size - i - 1] = buffer[i];
		assert(m_nPos <= m_nSize);
	}
	bool MemoryStream::checkRead(uint32 nSize) const
	{
		return (m_nSize - m_nPos) >= nSize;
	}
	bool MemoryStream::IsEndOfStream() const
	{
		return m_nSize == m_nPos;
	}
	void MemoryStream::read(IStream *pStream, bool bAttach)
	{

	}
	bool MemoryStream::AttachStream(IStream *pStream, uint32 nSize, bool bSeek)
	{
		if(!checkRead(nSize))
			return false;
		pStream->attach(buffer() + pos(), nSize);
		if(bSeek)
			seek(nSize, soFromCurrent);
		return true;
	}
	
}