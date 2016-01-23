#include "stdafx.h"
#include "FilePosix.h"
#include "String.h"
#include <dirent.h>

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
			file_flags |=  O_CREAT | O_TRUNC;
		}
		else if(mode == ofmCreateNew || mode == ofmOpenAlways )
		{
			file_flags |= O_CREAT;
		}

		CString sFielName = pszFileName;
		m_hFile = open(sFielName.cstr(), file_flags);
		return isValid();

	}
	  int64 CFilePosixImpl::getFileSize() const
    {
        if(!isValid())
            return -1;
       struct stat64 st;
       fstat64(m_hFile, &st);
       return st.st_size;
    }

    bool CFilePosixImpl::setFilePos64(int64 nPos, enSeekOffset offset){


        int64 nRet = lseek64(m_hFile, nPos, offset == soFromBegin ? SEEK_SET : offset == soFromCurrent ? SEEK_CUR
                                                                                                      : SEEK_END);
        return nRet != -1;
    }

    bool CFilePosixImpl::setFilePos(int32 nPos, enSeekOffset offset){
        int nRet = lseek(m_hFile, nPos, offset == soFromBegin ? SEEK_SET : offset == soFromCurrent ? SEEK_CUR
                                                                                                      : SEEK_END);
        return nRet != -1;
    }
    int64 CFilePosixImpl::getFilePos() const
    {
        return lseek64(m_hFile, 0, SEEK_CUR);
    }
    bool CFilePosixImpl::setFileEnd(){
        return setFilePos(0, soFromEnd);
    }
    bool CFilePosixImpl::setFileBegin(){
        return setFilePos(0, soFromBegin);
    }
    uint32 CFilePosixImpl::writeFile(const void* pData, uint32 nSize){


        int32 nWriteBytes = 0;
        nWriteBytes = write(m_hFile, pData, nSize);
        if(nWriteBytes == -1)
            return 0;
        return nWriteBytes;
    }
    uint32 CFilePosixImpl::readFile(void* pData, uint32 nSize)
    {
        int32 nReadBytes = 0;
        nReadBytes = read(m_hFile, pData, nSize);
        if(nReadBytes == -1)
            return 0;
        return nReadBytes;
    }
    bool CFilePosixImpl::closeFile(){
        if(!isValid())
            return false;
       int nRet =  close(m_hFile);
       m_hFile = -1;
       return nRet == 0;
        /*BOOL bRet = ::CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
        return bRet == TRUE;*/
    }
    bool CFilePosixImpl::isValid()  const
    {
        return m_hFile != -1;
    }
    bool CFilePosixImpl::Flush()
    {
        return fsync(m_hFile) == 0;
    }


    namespace FileSystem
    {
        bool deleteFile(const wchar_t* pszFileName)
        {
			CString sFielName = pszFileName;
            remove(sFielName.cstr()) == 0;
        }
        bool isFileExisit(const wchar_t* pszFileName)
        {
			CString sFielName = pszFileName;
            return  access( sFielName.cstr(), F_OK ) != -1;
        }
        void getFiles(const CString& path, std::vector<CString>& vecFiles)
        {
			DIR           *d;
			struct dirent *dir;
			d = opendir(path.cstr());
			if (d)
			{
				while ((dir = readdir(d)) != NULL)
				{
					vecFiles.push_back(CString(dir->d_name));
				}

				closedir(d);
			}
        }
    }


}