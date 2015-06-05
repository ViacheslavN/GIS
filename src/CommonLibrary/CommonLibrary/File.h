#ifndef _LIB_COMMON_FILE_H
#define _LIB_COMMON_FILE_H

#include "general.h"
#include "stream.h"
#include "str_t.h"
namespace CommonLib
{
	enum enAccesRights
	{
		arExecute = 0x00000001,
		arRead    = 0x00000002,
		arWrite   = 0x00000004,
		aeReadWrite = arRead|arWrite
	};

	enum enShareMode
	{
		smNoMode = 0x00,
		smDelete = 0x00000001,
		smRead   = 0x00000002,
		smWrite  = 0x00000004,
	};

	enum enOpenFileMode
	{
		ofmCreateAlways,
		ofmCreateNew,
		ofmOpenAlways,
		ofmOpenExisting,
	};



	class IFile
	{
		public:
			IFile(){};
			virtual ~IFile(){}
			virtual bool openFile(const wchar_t *pszFileName,  enOpenFileMode mode, enAccesRights access, enShareMode share) = 0;
			virtual int64 getFileSize() = 0;
			virtual bool setFilePos64(int64 nPos, enSeekOffset offset) = 0;
			virtual bool setFilePos(uint32 nPos, enSeekOffset offset) = 0;
			virtual int64 getFilePos() = 0;
			virtual bool setFileEnd() = 0;
			virtual bool setFileBegin() = 0;
			virtual uint32 writeFile(const void* pData, uint32 nSize) = 0;
			virtual uint32 readFile(void* pData, uint32 nSize) = 0;
			virtual bool closeFile() = 0;
			virtual bool isValid() const = 0;
			virtual bool Flush() = 0;
	};

	namespace FileSystem
	{
		bool deleteFile(const wchar_t *pszFileName);
		bool isFileExisit(const wchar_t *pszFileName);
	}
}


#ifdef WIN32
#include "FileWin32.h"
#endif

#endif