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


#ifdef WIN32
typedef   HANDLE  FileHandle;
#endif

	class IFile
	{
		public:
			IFile(){};
			virtual ~IFile(){}
			virtual bool attach(FileHandle handle) = 0;
			virtual FileHandle deattach() = 0;
			virtual bool openFile(const wchar_t *pszFileName,  enOpenFileMode mode, enAccesRights access, enShareMode share) = 0;
			virtual int64 getFileSize() const= 0;
			virtual bool setFilePos64(uint64 nPos, enSeekOffset offset) = 0;
			virtual bool setFilePos(uint32 nPos, enSeekOffset offset) = 0;
			virtual int64 getFilePos() const = 0;
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
		void getFiles(const str_t& path, std::vector<str_t>& vecFiles);
		str_t  FindFileName(const str_t& sFullFileName);
		str_t  FindOnlyFileName(const str_t& sFullFileName);
		str_t  FindFileExtension(const str_t& sFullFileName);
		str_t  FindFilePath(const str_t& sFullFileName);
	}
}


#ifdef WIN32
#include "FileWin32.h"
#endif

#endif