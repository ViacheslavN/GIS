#include "stdafx.h"
#include "FileWin32.h"
namespace CommonLib
{

CFileWin32Impl::CFileWin32Impl() : m_hFile(INVALID_HANDLE_VALUE), m_bAttach(false)
{
}
CFileWin32Impl::~CFileWin32Impl()
{
	if(!m_bAttach)
	{
		if(m_hFile != INVALID_HANDLE_VALUE)
		{
			::CloseHandle(m_hFile);
			m_hFile = INVALID_HANDLE_VALUE;
		}
	}
	
}


bool CFileWin32Impl::attach(FileHandle handle)
{
	if(!m_bAttach)
		closeFile();
	m_hFile = handle;
	return isValid();
}
FileHandle CFileWin32Impl::deattach()
{
	FileHandle handle = m_hFile;
	m_bAttach = false;
	m_hFile = INVALID_HANDLE_VALUE;
	return handle;

}

bool CFileWin32Impl::openFile(const wchar_t *pszFileName, enOpenFileMode mode, enAccesRights access, enShareMode share, enOpenFileType openType)
{
	DWORD win_access = (access & arExecute ? GENERIC_EXECUTE: 0) | (access & arRead ? GENERIC_READ: 0) 
		| (access & arWrite ? GENERIC_WRITE: 0);
	DWORD win_share = (share & smDelete ? FILE_SHARE_DELETE: 0) | (share & smRead ? FILE_SHARE_READ: 0)
		| (share & smWrite ? FILE_SHARE_WRITE: 0);
	DWORD win_mode = mode == ofmCreateAlways  ? CREATE_ALWAYS:  mode == ofmCreateNew  ? CREATE_NEW:
		mode == ofmOpenAlways  ? OPEN_ALWAYS:   OPEN_EXISTING;
	m_hFile = CreateFile(pszFileName, win_access, win_share, NULL, win_mode, NULL, NULL); 
	return isValid();
	
}
int64 CFileWin32Impl::getFileSize() const
{
	if(!isValid())
		return -1;
	LARGE_INTEGER FileSize;
	::GetFileSizeEx(m_hFile, &FileSize);
	return (int64)FileSize.QuadPart;
}
bool CFileWin32Impl::setFilePos64(uint64 nPos, enSeekOffset offset){

	  LARGE_INTEGER lpos;
	  lpos.QuadPart = nPos;
	return 0 != ::SetFilePointerEx(m_hFile, lpos, 0, offset == soFromBegin ? FILE_BEGIN : offset == soFromCurrent ? FILE_CURRENT
 		: FILE_END);
}

bool CFileWin32Impl::setFilePos(uint32 nPos, enSeekOffset offset){
	 return 0 != ::SetFilePointer(m_hFile, nPos, 0, offset == soFromBegin ? FILE_BEGIN : offset == soFromCurrent ? FILE_CURRENT
 		: FILE_END);
}
int64 CFileWin32Impl::getFilePos() const
{
	LARGE_INTEGER lpos;
	lpos.QuadPart = 0;
	 LARGE_INTEGER ret;
	::SetFilePointerEx(m_hFile, lpos, &ret,  FILE_CURRENT);
	return ret.QuadPart;
}
bool CFileWin32Impl::setFileEnd(){
	return FALSE != ::SetEndOfFile(m_hFile);
}
bool CFileWin32Impl::setFileBegin(){
	return setFilePos(0, soFromBegin);
}
uint32 CFileWin32Impl::writeFile(const void* pData, uint32 nSize){
	DWORD dww =0;
	if(::WriteFile(m_hFile, pData, nSize, &dww, NULL)==NULL)
		return 0;
	return dww;

}
uint32 CFileWin32Impl::readFile(void* pData, uint32 nSize){
	DWORD dwl = 0;
	if(::ReadFile(m_hFile, pData, nSize, &dwl ,NULL)== NULL)
		return 0;
	return dwl;
}
bool CFileWin32Impl::closeFile(){
	if(!isValid())
		return false;

	BOOL bRet = ::CloseHandle(m_hFile);
	m_hFile = INVALID_HANDLE_VALUE;
	return bRet == TRUE;
}
bool CFileWin32Impl::isValid()  const
{
	return m_hFile != INVALID_HANDLE_VALUE;
}
bool CFileWin32Impl::Flush()
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
}

}