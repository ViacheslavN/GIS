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
	
	

}