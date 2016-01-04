#ifndef _EMBEDDED_COMMON_I_MEMORY_STERAM_H_
#define _EMBEDDED_COMMON_I_MEMORY_STERAM_H_

#include "general.h"
#include "alloc_t.h"
#include "Stream.h"
namespace CommonLib
{
	class MemoryStream : public IStream, public IReadStreamBase, public IWriteStreamBase
	{

	private:

		MemoryStream(const MemoryStream& stream);
		MemoryStream& operator=(const MemoryStream& stream);
	public:
		MemoryStream(alloc_t *pAlloc = NULL);
		~MemoryStream();

		//IStream
		
		virtual void attach(byte* pBuffer, uint32 nSize, bool bCopy = false);
		virtual byte* deattach() ;
		virtual uint32 size() const;
		virtual bool seek(uint32 position, enSeekOffset offset );
		virtual uint32 pos() const;
		virtual void reset();
		virtual void close();
		virtual void create(uint32 nSize);
		virtual byte* buffer();
		virtual const byte* buffer() const;

		//IReadStream
		virtual bool checkRead(uint32 nSize) const;
		virtual bool IsEndOfStream() const;
		virtual void read(IStream *pStream, bool bAttach = true);
		virtual bool AttachStream(IStream *pStream, uint32 nSize, bool bSeek = true);
		

		
		virtual void read_bytes(byte* dst, uint32 size);
		virtual void read_inverse(byte* buffer, uint32 size);
		virtual void write_bytes(const byte* buffer, uint32 size);
		virtual void write_inverse(const byte* buffer, uint32 size);
	
	

		void resize(uint32 nSize);

		byte* m_pBuffer;
		uint32  m_nPos;
		uint32  m_nSize;
		bool m_bIsBigEndian;
		alloc_t *m_pAlloc;
		bool m_bAttach;
		simple_alloc_t m_alloc;
	};


}
#endif