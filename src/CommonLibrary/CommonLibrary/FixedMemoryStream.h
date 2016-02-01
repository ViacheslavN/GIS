#ifndef _EMBEDDED_COMMON_I_FIXED_MEMORY_STERAM_H_
#define _EMBEDDED_COMMON_I_FIXED_MEMORY_STERAM_H_

#include "general.h"
#include "alloc_t.h"
#include "Stream.h"
namespace CommonLib
{

	 
	class FxMemoryReadStream : public TMemoryStreamBase<IReadStreamBase>
	{
	public:
		typedef  TMemoryStreamBase<IReadStreamBase> TBase;
		FxMemoryReadStream(alloc_t *pAlloc = NULL);
		FxMemoryReadStream(byte* pBuffer, uint32 nSize, bool bAttach = true, alloc_t *pAlloc = NULL);
		~FxMemoryReadStream();


		virtual void read_bytes(byte* dst, uint32 size);
		virtual void read_inverse(byte* buffer, uint32 size);
		virtual void readStream(IStream *pStream, bool bAttach = false);
		virtual bool SaveReadStream(IStream *pStream, bool bAttach = false);
	};

	class FxMemoryWriteStream : public TMemoryStreamBase<IWriteStreamBase> 
	{
	public:
		typedef  TMemoryStreamBase<IWriteStreamBase> TBase;
		FxMemoryWriteStream(alloc_t *pAlloc = NULL);
		~FxMemoryWriteStream();

		virtual void write_bytes(const byte* buffer, uint32 size);
		virtual void write_inverse(const byte* buffer, uint32 size);
		virtual void writeStream(IStream *pStream, int32 nPos = -1, int32 nSize = -1);
		
	};

}

#endif