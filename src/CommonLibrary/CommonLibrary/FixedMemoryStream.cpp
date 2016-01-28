#include "stdafx.h"
#include "FixedMemoryStream.h"
namespace CommonLib
{
	
	FxMemoryReadStream::FxMemoryReadStream(alloc_t *pAlloc) : TBase(pAlloc)
	{

	}

	FxMemoryReadStream::FxMemoryReadStream(byte* pBuffer, uint32 nSize, bool bAttach, alloc_t *pAlloc) : TBase(pAlloc)
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
	
	
	FxMemoryWriteStream::FxMemoryWriteStream(alloc_t *pAlloc) : TBase(pAlloc)
	{

	}
	FxMemoryWriteStream::~FxMemoryWriteStream()
	{

	}


	void FxMemoryWriteStream::write_bytes(const byte* buffer, uint32 size)
	{
		if(size > 0)
		{
			::memcpy(m_pBuffer + m_nPos, buffer, size);
			m_nPos += size;
		}
		assert(m_nPos <= m_nSize);
	}
	void FxMemoryWriteStream::write_inverse(const byte* buffer, uint32 size)
	{
		for(size_t i = 0; i < size; m_nPos++, i++)
			m_pBuffer[m_nPos + size - i - 1] = buffer[i];
		assert(m_nPos <= m_nSize);
	}

}