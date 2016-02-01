#ifndef _EMBEDDED_COMMON_I_MEMORY_STERAM_H_
#define _EMBEDDED_COMMON_I_MEMORY_STERAM_H_

#include "general.h"
#include "alloc_t.h"
#include "Stream.h"
namespace CommonLib
{


 


	class CReadMemoryStream : public TMemoryStreamBase<IReadStreamBase>
	{

	private:

		CReadMemoryStream(const CReadMemoryStream& stream);
		CReadMemoryStream& operator=(const CReadMemoryStream& stream);
	public:
		CReadMemoryStream(alloc_t *pAlloc = NULL);
		~CReadMemoryStream();

		typedef TMemoryStreamBase<IReadStreamBase> TBase;
 
		
		virtual void read_bytes(byte* dst, uint32 size);
		virtual void read_inverse(byte* buffer, uint32 size);
		virtual void readStream(IStream *pStream, bool bAttach = false);
		virtual bool SaveReadStream(IStream *pStream, bool bAttach = false);

 
		 
	};



	class CWriteMemoryStream : public TMemoryStreamBase<IWriteStreamBase>
	{

	private:

		CWriteMemoryStream(const CWriteMemoryStream& stream);
		CWriteMemoryStream& operator=(const CWriteMemoryStream& stream);
	public:
		CWriteMemoryStream(alloc_t *pAlloc = NULL);
		~CWriteMemoryStream();

		typedef TMemoryStreamBase<IWriteStreamBase> TBase;



		virtual void write_bytes(const byte* buffer, uint32 size);
		virtual void write_inverse(const byte* buffer, uint32 size);
		virtual bool  resize(uint32 nSize);

		virtual void writeStream(IStream *pStream, int32 nPos = -1, int32 nSize = -1);
	private:
		
	};

}
#endif