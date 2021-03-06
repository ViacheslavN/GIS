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

	void FxMemoryReadStream::read_bytes(byte* dst, uint32 size)
	{
		::memcpy(dst, this->m_pBuffer + this->m_nPos, size);
		this->m_nPos += size;
		assert(this->m_nPos <= this->m_nSize);
	}
	void FxMemoryReadStream::read_inverse(byte* buffer, uint32 size)
	{
		for(uint32 i = 0; i < size; m_nPos++, i++)
			buffer[i] = this->m_pBuffer[m_nPos + size - i - 1];
		this->m_nPos += size;
		assert(this->m_nPos <= this->m_nSize);
	}
	void FxMemoryReadStream::readStream(IStream *pStream, bool bAttach)
	{
		IMemoryStream *pMemStream = dynamic_cast<IMemoryStream *>(pStream);
		if(pMemStream)
		{
			uint32 nStreamSize = readIntu32();
			if(bAttach)
			{
				pMemStream->attachBuffer(buffer() + pos(), nStreamSize);
				seek(nStreamSize, soFromCurrent);
			}
			else
			{
				pMemStream->resize(nStreamSize);
				if(nStreamSize)
					read(pMemStream->buffer() + pStream->pos(), nStreamSize);
			}

		}
	}
	bool FxMemoryReadStream::SaveReadStream(IStream *pStream, bool bAttach)
	{
		IMemoryStream *pMemStream = dynamic_cast<IMemoryStream *>(pStream);
		if(!pMemStream)
			return false;
		uint32 nStreamSize = 0;
		if(!save_read(nStreamSize))
			return false;
		if(bAttach)
		{
			if((size() - pos())  < nStreamSize)
				return false;
			pMemStream->attachBuffer(buffer() + pos(), nStreamSize);
			seek(nStreamSize, soFromCurrent);
			return true;
		}
		else
		{
			if(nStreamSize)
			{
				pMemStream->resize(nStreamSize);
			}
			return save_read(pMemStream->buffer() + pStream->pos(), nStreamSize);
		}
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

	void FxMemoryWriteStream::writeStream(IStream *pStream, int32 nPos, int32 nSize)
	{
		IMemoryStream *pMemStream = dynamic_cast<IMemoryStream *>(pStream);
		if(pMemStream)
		{
			uint32 _nPos = (nPos != -1 ? nPos : 0);
			uint32 _nSize= (nSize != -1 ? nSize : pStream->size());

			write(_nSize);
			write(pMemStream->buffer() + _nPos, _nSize);
		}

	}

}