#include "stdafx.h"
#include "MemoryStream.h"
namespace CommonLib
{

	CReadMemoryStream::CReadMemoryStream(alloc_t *pAlloc) : TBase(pAlloc)
		{}
		CReadMemoryStream::~CReadMemoryStream()
		{}

		void CReadMemoryStream::read_bytes(byte* dst, uint32 size)
		{
			::memcpy(dst, this->m_pBuffer + this->m_nPos, size);
			this->m_nPos += size;
			assert(this->m_nPos <= this->m_nSize);
		}

		void CReadMemoryStream::read_inverse(byte* buffer, uint32 size)
		{
			for(uint32 i = 0; i < size; m_nPos++, i++)
				buffer[i] = this->m_pBuffer[m_nPos + size - i - 1];
			this->m_nPos += size;
			assert(this->m_nPos <= this->m_nSize);
		}
		void CReadMemoryStream::read(IStream *pStream, bool bAttach)
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
						IReadStream::read(pMemStream->buffer() + pStream->pos(), nStreamSize);
				}

			}
		}
		bool CReadMemoryStream::save_read(IStream *pStream, bool bAttach)
		{
			IMemoryStream *pMemStream = dynamic_cast<IMemoryStream *>(pStream);
			if(!pMemStream)
				return false;
			uint32 nStreamSize = 0;
			if(!IReadStream::save_read(nStreamSize))
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
				pMemStream->resize(nStreamSize);
				return IReadStream::save_read(pMemStream->buffer() + pStream->pos(), nStreamSize);
			}
		}



		CWriteMemoryStream::CWriteMemoryStream(alloc_t *pAlloc): TBase(pAlloc)
		{

		}
		CWriteMemoryStream::~CWriteMemoryStream()
		{

		}

		void CWriteMemoryStream::write_bytes(const byte* buffer, uint32 size)
		{
			if(size > 0)
			{
				resize(size);
				::memcpy(this->m_pBuffer + m_nPos, buffer, size);
				m_nPos += size;
			}
			assert(m_nPos <= m_nSize);
		}
		void CWriteMemoryStream::write_inverse(const byte* buffer, uint32 size)
		{
			resize(size);
			for(size_t i = 0; i < size; m_nPos++, i++)
				this->m_pBuffer[m_nPos + size - i - 1] = buffer[i];
			assert(m_nPos <= m_nSize);
		}
		bool  CWriteMemoryStream::resize(uint32 nSize)
		{
			uint32 newSize = m_nSize;

			while(m_nPos + nSize > newSize)
				newSize = uint32(newSize * 1.5) + 1;
			if(newSize > m_nSize)
			{
				assert(!m_bAttach);
				m_nSize = newSize;
				byte* buffer =  (byte*)this->m_pAlloc->alloc(sizeof(byte) * newSize);
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
		void CWriteMemoryStream::write(IStream *pStream, uint32 nPos, uint32 nSize)
		{
			IMemoryStream *pMemStream = dynamic_cast<IMemoryStream *>(pStream);
			if(pMemStream)
			{
				IWriteStream::write(nSize ? nSize : (uint32)pStream->size());
				IWriteStream::write(pMemStream->buffer() + nPos, nSize ? nSize : pStream->size());
			}

		}
}