#ifndef _LIB_COMMON_FILE_POSIX_H
#define _LIB_COMMON_FILE_POSIX_H

#include "File.h"

namespace CommonLib
{
	class CFilePosixImpl : public IFile
	{
	public:
		CFilePosixImpl();
		~CFilePosixImpl();

		virtual bool attach(FileHandle handle);
		virtual FileHandle deattach();
		virtual bool openFile(const wchar_t* pszFileName, enOpenFileMode mode, enAccesRights access, enShareMode share, enOpenFileType openType = oftBinary);
		virtual int64 getFileSize() const;
		virtual bool setFilePos64(uint64 nPos, enSeekOffset offset);
		virtual bool setFilePos(uint32 nPos, enSeekOffset offset);
		virtual int64 getFilePos() const;
		virtual bool setFileEnd();
		virtual bool setFileBegin();
		virtual uint32 writeFile(const void* pData, uint32 nSize);
		virtual uint32 readFile(void* pData, uint32 nSize);
		virtual bool closeFile();
		virtual bool isValid() const;
		virtual bool Flush();
	private:
		FileHandle m_hFile;
		bool m_bAttach;
	};

	typedef CFilePosixImpl CFile;
}

#endif

//SetEndOfFile(
//SetFilePointerEx
//GetFileSize