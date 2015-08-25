#include "stdafx.h"
#include "FileStream.h"

namespace CommonLib
{
	CFileStream::CFileStream() 
	{}
	CFileStream::~CFileStream()
	{}
	bool CFileStream::open(const wchar_t *pszFileName,  enOpenFileMode mode, enAccesRights access, enShareMode share)
	{
		return m_File.openFile(pszFileName, mode, access, share);
	}
	void CFileStream::attach(FileHandle handle)
	{
		m_File.attach(handle);
	}
	FileHandle CFileStream::deattach()
	{
		return m_File.deattach();
	}
	int64 CFileStream::size() const
	{
		return m_File.getFileSize();
	}
	bool CFileStream::seek(uint32 position, enSeekOffset offset )
	{
		return m_File.setFilePos(position, offset);
	}
	bool CFileStream::seek64(int64 position, enSeekOffset offset )
	{
		return m_File.setFilePos64(position, offset);
	}
	int64 CFileStream::pos() const
	{
		return m_File.getFilePos();
	}
	void CFileStream::reset()
	{
		m_File.setFilePos(0, soFromBegin);
	}
	void CFileStream::close()
	{
		m_File.closeFile();
	}
	bool CFileStream::isValid() const
	{
		return m_File.isValid();
	}


	CReadFileStream::CReadFileStream()
	{

	}
	CReadFileStream::~CReadFileStream()
	{

	}
	void CReadFileStream::read_bytes(byte* pDst, size_t size)
	{
		uint32 nRead = m_File.readFile(pDst, size);
		assert(nRead == size);
	}

	void CReadFileStream::read_inverse(byte* pDst, size_t size)
	{
		uint32 nRead = m_File.readFile(pDst, size); //To do inverse
		assert(nRead == size);
	}

	bool CReadFileStream::checkRead(uint32 nSize) const
	{
		return (size() - pos()) >= nSize;
	}
	bool CReadFileStream::IsEndOfStream() const
	{
		return size() == pos();
	}
	CWriteFileStream::CWriteFileStream()
	{

	}
	CWriteFileStream::~CWriteFileStream()
	{

	}

	void CWriteFileStream::write_bytes(const byte* buffer, size_t size)
	{
		uint32 nWrite = m_File.writeFile(buffer, size);
		assert(nWrite == size);
	}
	void CWriteFileStream::write_inverse(const byte* buffer, size_t size)
	{
		uint32 nWrite = m_File.writeFile(buffer, size); //To do inverse
		assert(nWrite == size);
	}
	
	

}