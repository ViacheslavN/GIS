#include "stdafx.h"
#include "general.h"
#include "BitStream.h"
#include "WriteBitStream.h"

namespace CommonLib
{
	WriteBitStream::WriteBitStream(alloc_t *pAlloc) : TBase(pAlloc)
	{
	}
	WriteBitStream::~WriteBitStream()
	{

	}

	bool  WriteBitStream::resize(uint32 nSize)
	{
		uint32 newSize = m_nSize;

		while(m_nPos + nSize > newSize)
			newSize = uint32(newSize * 1.5) + 1;
		if(newSize > m_nSize)
		{
			assert(!m_bAttach);
			m_nSize = newSize;
			byte* buffer =  (byte*)this->m_pAlloc->alloc(sizeof(byte) * newSize);
			memset(buffer, 0, newSize);
			if(this->m_pBuffer)
			{
				memcpy(buffer, this->m_pBuffer, this->m_nPos);
				if(!m_bAttach)
				{
					this->m_pAlloc->free(m_pBuffer);
				}
			}
			this->m_pBuffer = buffer;

		}

		return this->m_pBuffer != NULL;
	}

	void WriteBitStream::writeBit(bool bBit)
	{
		
		if (m_nCurrBit > m_nBitBase)
		{
			m_nPos++;
			if(m_nPos == m_nSize)
			{
				assert(!m_bAttach);
				uint32 newSize = uint32(m_nSize * 1.5) + 1;
				m_nSize = newSize;
				byte* buffer =  (byte*)m_pAlloc->alloc(sizeof(byte) * newSize);
				if(m_pBuffer)
				{
					memcpy(buffer, m_pBuffer, m_nPos);
					if(!m_bAttach)
						m_pAlloc->free(m_pBuffer);
				}
				m_pBuffer = buffer;

			}
			m_nCurrBit = 0;
			m_pBuffer[m_nPos] = 0;
		}
		if(bBit)
			m_pBuffer[m_nPos] |= (1 << m_nCurrBit);
		m_nCurrBit++;
	}
}