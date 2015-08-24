#include "stdafx.h"
#include "blob.h"
namespace CommonLib
{
	CBlob::CBlob(alloc_t *pAlloc) : m_pAlloc(pAlloc)
		,m_pBuffer(NULL), m_nSize(0), m_nCapacity(0), m_bAttach(false)
	{
		if(!m_pAlloc)
			m_pAlloc = &m_alloc;
	}
	CBlob::CBlob(uint32 nSize, alloc_t *pAlloc) : m_pAlloc(pAlloc)
		,m_pBuffer(NULL), m_nSize(0), m_nCapacity(0), m_bAttach(false)
	{
		if(!m_pAlloc)
			m_pAlloc = &m_alloc;

		resize(nSize);
	}
	CBlob::CBlob(const CBlob& blob)
	{
		if(blob.m_pAlloc != &blob.m_alloc)
			m_pAlloc = blob.m_pAlloc;
		else
			m_pAlloc = &m_alloc;

		m_bAttach = true;
		m_pBuffer = blob.m_pBuffer;
		m_nSize = blob.m_nSize;
		m_nCapacity = blob.m_nCapacity;
	}

	CBlob::CBlob(byte* pBuf, uint32 nSize, bool bAttach, alloc_t *pAlloc) : 
		m_pAlloc(pAlloc), m_bAttach(bAttach), m_nSize(nSize), m_nCapacity(nSize)
	{
		if(!m_pAlloc)
			m_pAlloc = &m_alloc;

		if(m_bAttach)
			m_pBuffer = pBuf;
		else if(m_pBuffer && m_nSize != 0)
		{
			m_pBuffer = (byte*)m_pAlloc->alloc(m_nSize);
			memcpy(m_pBuffer, pBuf, m_nSize);
		}
	}

	CBlob& CBlob::operator = (const CBlob& blob)
	{
		clear(true);

		if(blob.m_pAlloc != &blob.m_alloc)
			m_pAlloc = blob.m_pAlloc;
		else
			m_pAlloc = &m_alloc;

		m_bAttach = true;
		m_pBuffer = blob.m_pBuffer;
		m_nSize = blob.m_nSize;
		m_nCapacity = blob.m_nCapacity;

		return *this;
	}

	CBlob::~CBlob()
	{
		clear(true);
	}

	void CBlob::attach(const byte* pBuf, uint32 nSize)
	{
		clear(true);
	}
	byte* CBlob::deattach()
	{
		byte *pBuf = m_pBuffer;
		m_nCapacity = 0;
		m_nSize = 0;
		m_pBuffer = NULL;


		return pBuf;
	}
	void CBlob::copy(const byte* pBuf, uint32 nSize)
	{
	
		resize(nSize);
		memcpy(m_pBuffer, pBuf, nSize);
	}


	bool CBlob::operator ==(const CBlob& blob) const
	{
		return compare(blob) == 0;
	}

	bool CBlob::operator !=(const CBlob& blob) const
	{
		return compare(blob) != 0;
	}

	bool CBlob::operator <(const CBlob& blob) const
	{
		return compare(blob) < 0;
	}

	bool CBlob::operator >(const CBlob& blob) const
	{
		return compare(blob) > 0;
	}

	bool CBlob::operator <=(const CBlob& blob) const
	{
		return compare(blob) <= 0;
	}

	bool CBlob::operator >=(const CBlob& blob) const
	{
		return compare(blob) >= 0;
	}
	const unsigned char& CBlob::operator [](size_t index) const
	{
		return *(buffer() + index);
	}

	unsigned char& CBlob::operator [](size_t index)
	{
		return buffer()[index];
	}
	int CBlob::equals(const unsigned char *buffer, size_t _size) const
	{
		return compare(buffer, _size) == 0;
	}

	int CBlob::equals(const CBlob& blob) const
	{
		return compare(blob) == 0;
	}

	int CBlob::compare(const unsigned char *buffer, size_t _size) const
	{
		if(buffer == NULL)
			return empty() ? 0 : 1;

		if(empty())
			return -1;

		size_t minsize = min(size(), _size);

		int n = memcmp(m_pBuffer, buffer, minsize);
		if(n != 0 || size() == _size)
			return n;

		return size() < _size ? -1 : 1;
	}

	int CBlob::compare(const CBlob& blob) const
	{
		if(blob.empty())
			return empty() ? 0 : 1;

		return compare(blob.buffer(), blob.size());
	}

	void  CBlob::resize(uint32 nSize)
	{
		if(m_bAttach)
			deattach();
		if(m_nCapacity >= m_nSize)
			return;

		byte* pBuffer = (byte*)m_pAlloc->alloc(nSize);
		if(m_pBuffer != NULL)
		{
			memcpy(pBuffer, m_pBuffer, nSize);
			m_pAlloc->free(m_pBuffer);
		}

		m_nCapacity = nSize;
		m_nSize = nSize;

		m_pBuffer = pBuffer;

	}
	bool   CBlob::empty() const
	{
		return m_nSize == 0;
	}
	uint32 CBlob::size() const
	{
		return m_nSize;
	}
	uint32 CBlob::capacity() const
	{
		return m_nCapacity;
	}
	const byte* CBlob::buffer() const
	{
		return m_pBuffer;
	}
	byte* CBlob::buffer()
	{
		return m_pBuffer;
	}
	void CBlob::clear(bool bDel)
	{
		
		if(!m_bAttach && m_pBuffer != 0)
		{
			m_nSize = 0;
			if(bDel)
			{
				m_nCapacity = 0;
				m_pAlloc->free(m_pBuffer);
			}
		}
	}
}