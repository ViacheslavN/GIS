#ifndef _COMMON_LIB_I_MEMORY_STERAM_H_
#define _COMMON_LIB_I_MEMORY_STERAM_H_


#include "general.h"
#include "alloc_t.h"
#include "Stream.h"
#include "File.h"

namespace CommonLib
{

	class IFileStream
	{
	public:
		IFileStream(){}
		virtual ~IFileStream(){}

	 
		virtual bool open(const wchar_t *pszFileName,  enOpenFileMode mode, enAccesRights access, enShareMode share) = 0;
		virtual void attachFile(FileHandle handle)  = 0;
		virtual FileHandle deattachFile() = 0;
		virtual bool isValid() const = 0;
		virtual bool flush() = 0;
	protected:
	
		CFile m_File;
	};



	template<class I>
	class TFileStreamBase : public I, public IFileStream
	{


	private:

		TFileStreamBase(const TFileStreamBase& stream) : m_pAttachStream(NULL) {}
		TFileStreamBase& operator=(const TFileStreamBase& stream){}

	public:
		TFileStreamBase() 
		{}
		~TFileStreamBase()
		{}

		//IFileStream
		virtual bool open(const wchar_t *pszFileName,  enOpenFileMode mode, enAccesRights access, enShareMode share)
		{
			return m_File.openFile(pszFileName, mode, access, share);
		}
		virtual void attachFile(FileHandle handle)
		{
			m_File.attach(handle);
		}
		virtual FileHandle deattachFile()
		{
			return m_File.deattach();
		}
		virtual bool isValid() const
		{
			return m_File.isValid();
		}
		virtual bool flush()
		{
			return m_File.Flush();
		}


		virtual int32 size() const
		{
			return (int32)m_File.getFileSize();
		}
		virtual int64 size64() const
		{
			return m_File.getFileSize();
		}
		virtual bool seek(uint32 position, enSeekOffset offset )
		{
			return m_File.setFilePos(position, offset);
		}
		virtual bool seek64(uint64 position, enSeekOffset offset )
		{
			return m_File.setFilePos64(position, offset);
		}
		virtual int32 pos() const
		{
			return (int32)m_File.getFilePos();
		}
		virtual int64 pos64() const
		{
			return m_File.getFilePos();
		}
		virtual void reset()
		{
			m_File.setFilePos(0, soFromBegin);
		}
		virtual void close()
		{
			m_File.closeFile();
		}
	
		virtual bool attach(IStream *pStream, int32 nPos = -1, int32 nSize = -1)
		{

			IFileStream *pFileStream = dynamic_cast<IFileStream *>(pFileStream);
			return true;

		}
		virtual bool attach64(IStream *pStream, int64 nPos = -1, int64 nSize  = -1) 
		{
			return true;
		}
		virtual IStream * deattach()
		{

		}
	protected:
		CFile m_File;
		IStream *m_pAttachStream;
	};




	class CReadFileStream : public TFileStreamBase<IReadStreamBase>
	{
	public:

		CReadFileStream();
		~CReadFileStream();

		virtual void  read_bytes(byte* dst, uint32 size);
		virtual void  read_inverse(byte* buffer, uint32 size);
  	};


	class CWriteFileStream : public TFileStreamBase<IWriteStreamBase>
	{
	public:

		CWriteFileStream();
		~CWriteFileStream();
		
		virtual void write_bytes(const byte* buffer, uint32 size);
		virtual void write_inverse(const byte* buffer, uint32 size);

	};
}

#endif