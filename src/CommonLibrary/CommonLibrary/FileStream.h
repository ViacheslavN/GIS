#ifndef _COMMON_LIB_I_MEMORY_STERAM_H_
#define _COMMON_LIB_I_MEMORY_STERAM_H_


#include "general.h"
#include "alloc_t.h"
#include "Stream.h"
#include "File.h"

namespace CommonLib
{

	class CFileStream
	{
	public:
		CFileStream();
		~CFileStream();

	 
		virtual bool open(const wchar_t *pszFileName,  enOpenFileMode mode, enAccesRights access, enShareMode share);
		virtual void attach(FileHandle handle);
		virtual FileHandle deattach() ;
		virtual int64 size() const;
		virtual bool seek(uint32 position, enSeekOffset offset );
		virtual bool seek64(int64 position, enSeekOffset offset );
		virtual int64 pos() const;
		virtual void reset();
		virtual void close();
		virtual bool isValid() const;

	protected:
	
		CFile m_File;
	};




	class CReadFileStream : public CFileStream, public IReadStreamBase
	{
	public:

		CReadFileStream();
		~CReadFileStream();

		virtual void  read_bytes(byte* dst, uint32 size);
		virtual void  read_inverse(byte* buffer, uint32 size);
		virtual bool checkRead(uint32 nSize) const;
		virtual bool IsEndOfStream() const;
		virtual bool AttachStream(IStream *pStream, uint32 nSize, bool bSeek);
	};


	class CWriteFileStream : public CFileStream, public IWriteStreamBase
	{
	public:

		CWriteFileStream();
		~CWriteFileStream();
		
		virtual void write_bytes(const byte* buffer, uint32 size);
		virtual void write_inverse(const byte* buffer, uint32 size);

	};
}

#endif