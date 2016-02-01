#include "stdafx.h"
#include "FileStream.h"

namespace CommonLib
{

	CReadFileStream::CReadFileStream()
	{

	}
	CReadFileStream::~CReadFileStream()
	{

	}
	void CReadFileStream::read_bytes(byte* pDst, uint32 size)
	{
		uint32 nRead = this->m_File.readFile(pDst, size);
		assert(nRead == size);
	}

	void CReadFileStream::read_inverse(byte* pDst, uint32 size)
	{
		uint32 nRead = this->m_File.readFile(pDst, size); //To do inverse
		assert(nRead == size);
	}
	void CReadFileStream::readStream(IStream *pStream, bool bAttach)
	{		
		if(IMemoryStream *pMemStream = dynamic_cast<IMemoryStream *>(pStream))
		{
			if(bAttach)
			{
				assert(false);
				return;
			}

			uint32 nStreamSize = readIntu32();
			if(nStreamSize)
			{
				pMemStream->resize(nStreamSize);
				read(pMemStream->buffer() + pStream->pos(), nStreamSize);
			}
		}
		else if(IFileStream *pFileStream = dynamic_cast<IFileStream *>(pStream))
		{
			assert(false); //TO DO as the need to make
		}
	}
	bool CReadFileStream::SaveReadStream(IStream *pStream, bool bAttach)
	{

		if(IMemoryStream *pMemStream = dynamic_cast<IMemoryStream *>(pStream))
		{
			if(bAttach)
			{
				assert(false);
				return false;
			}

			uint32 nStreamSize = 0;
			if(!save_read(nStreamSize))
				return false;
			
			if(nStreamSize)
				pMemStream->resize(nStreamSize);
			
			return save_read(pMemStream->buffer() + pStream->pos(), nStreamSize);
		}
		else if(IFileStream *pFileStream = dynamic_cast<IFileStream *>(pStream))
		{
			assert(false); //TO DO as the need to make
		}
		return false;
	}
	 
	 
	CWriteFileStream::CWriteFileStream()
	{

	}
	CWriteFileStream::~CWriteFileStream()
	{

	}

	void CWriteFileStream::write_bytes(const byte* buffer, uint32 size)
	{
		uint32 nWrite = this->m_File.writeFile(buffer, size);
		assert(nWrite == size);
	}
	void CWriteFileStream::write_inverse(const byte* buffer, uint32 size)
	{
		uint32 nWrite = this->m_File.writeFile(buffer, size); //To do inverse
		assert(nWrite == size);
	}
	
	void CWriteFileStream::writeStream(IStream *pStream, int32 nPos, int32 nSize)
	{
		
		if(IMemoryStream *pMemStream = dynamic_cast<IMemoryStream *>(pStream))
		{
			uint32 _nPos = (nPos != -1 ? nPos : 0);
			uint32 _nSize= (nSize != -1 ? nSize : pStream->size());

			write(_nSize);
			write(pMemStream->buffer() + _nPos, _nSize);
			 
		}
		else if(IFileStream *pFileStream = dynamic_cast<IFileStream *>(pStream))
		{

			assert(false); //TO DO as the need to make


		}

		

	}

}