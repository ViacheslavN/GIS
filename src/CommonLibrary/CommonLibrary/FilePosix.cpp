#include "stdafx.h"
#include "FilePosix.h"
#include "String.h"
namespace CommonLib
{

	CFilePosixImpl::CFilePosixImpl() : m_hFile(-1), m_bAttach(false)
	{
	}
	CFilePosixImpl::~CFilePosixImpl()
	{
		if(!m_bAttach)
		{
			if(m_hFile != -1)
			{
				close(m_hFile);
				m_hFile = -1;
			}
		}

	}


	bool CFilePosixImpl::attach(FileHandle handle)
	{
		if(!m_bAttach)
			closeFile();
		m_hFile = handle;
		return isValid();
	}
	FileHandle CFilePosixImpl::deattach()
	{
		FileHandle handle = m_hFile;
		m_bAttach = false;
		m_hFile = -1;
		return handle;

	}

	bool CFilePosixImpl::openFile(const wchar_t *pszFileName, enOpenFileMode mode, enAccesRights access, enShareMode share, enOpenFileType openType)
	{
		long file_flags = 0;
		if((access & arRead) && (access & arWrite ))
		{
			file_flags = O_RDWR;
		}
		else
			file_flags = /*(access & arExecute ? GENERIC_EXECUTE: 0) |*/ (access & arRead ? O_RDONLY: 0) | (access & arWrite ? O_WRONLY: 0);

		//file_flags  |= (openType & oftBinary ? O_BINARY: O_TEXT) 

		if(mode == ofmCreateAlways )
		{
			file_flags |=  (access & arRead ? O_RDWR : O_WRONLY) | O_CREAT | O_TRUNC;
		}
		else if(mode == ofmCreateNew || ofmOpenAlways )
		{
			file_flags |= O_CREAT;
		}
		CString sFielName = pszFileName;
		m_hFile = open(sFielName.cstr(), file_flags);
		return isValid();

	}
	/*int64 CFilePosixImpl::getFileSize() const
	{
		if(!isValid())
			return -1;
		LARGE_INTEGER FileSize;
		::GetFileSizeEx(m_hFile, &FileSize);
		return (int64)FileSize.QuadPart;
	}
	bool CFilePosixImpl::setFilePos64(uint64 nPos, enSeekOffset offset){

		LARGE_INTEGER lpos;
		lpos.QuadPart = nPos;
		return 0 != ::SetFilePointerEx(m_hFile, lpos, 0, offset == soFromBegin ? FILE_BEGIN : offset == soFromCurrent ? FILE_CURRENT
			: FILE_END);
	}

	bool CFilePosixImpl::setFilePos(uint32 nPos, enSeekOffset offset){
		return 0 != ::SetFilePointer(m_hFile, nPos, 0, offset == soFromBegin ? FILE_BEGIN : offset == soFromCurrent ? FILE_CURRENT
			: FILE_END);
	}
	int64 CFilePosixImpl::getFilePos() const
	{
		LARGE_INTEGER lpos;
		lpos.QuadPart = 0;
		LARGE_INTEGER ret;
		::SetFilePointerEx(m_hFile, lpos, &ret,  FILE_CURRENT);
		return ret.QuadPart;
	}
	bool CFilePosixImpl::setFileEnd(){
		return FALSE != ::SetEndOfFile(m_hFile);
	}
	bool CFilePosixImpl::setFileBegin(){
		return setFilePos(0, soFromBegin);
	}
	uint32 CFilePosixImpl::writeFile(const void* pData, uint32 nSize){
		DWORD dww =0;
		if(::WriteFile(m_hFile, pData, nSize, &dww, NULL)==NULL)
			return 0;
		return dww;

	}
	uint32 CFilePosixImpl::readFile(void* pData, uint32 nSize){
		DWORD dwl = 0;
		if(::ReadFile(m_hFile, pData, nSize, &dwl ,NULL)== NULL)
			return 0;
		return dwl;
	}
	bool CFilePosixImpl::closeFile(){
		if(!isValid())
			return false;

		BOOL bRet = ::CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
		return bRet == TRUE;
	}
	bool CFilePosixImpl::isValid()  const
	{
		return m_hFile != INVALID_HANDLE_VALUE;
	}
	bool CFilePosixImpl::Flush()
	{
		return::FlushFileBuffers(m_hFile) == TRUE;
	}


	namespace FileSystem
	{
		bool deleteFile(const wchar_t* pszFileName)
		{
			return ::DeleteFile(pszFileName) == TRUE;
		}
		bool isFileExisit(const wchar_t* pszFileName)
		{
			DWORD dwAttr = ::GetFileAttributes(pszFileName);
			return ((dwAttr != INVALID_FILE_ATTRIBUTES) && !(dwAttr & FILE_ATTRIBUTE_DIRECTORY));
		}
		void getFiles(const CString& path, std::vector<CString>& vecFiles)
		{
			WIN32_FIND_DATAW findData;
			HANDLE hFind;

			hFind = FindFirstFileW(path.cwstr(), &findData);
			if(hFind != INVALID_HANDLE_VALUE)
			{
				do
				{ 
					vecFiles.push_back(findData.cFileName);
				}
				while(::FindNextFileW(hFind, &findData));
			}
		}
	}*/

}