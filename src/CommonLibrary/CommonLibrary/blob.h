#ifndef _EMBEDDED_DATABASE_BLOB_
#define _EMBEDDED_DATABASE_BLOB_


#include "alloc_t.h"
#include "IRefCnt.h"
namespace CommonLib
{

	class CBlob : AutoRefCounter
	{
	public:

		CBlob(alloc_t *pAlloc = NULL, uint32 nSize = 0);
		CBlob(const CBlob& blob);
		CBlob(byte* pBuf, uint32 nSize, bool bAttach = true, alloc_t *pAlloc = NULL);
		CBlob& operator = (const CBlob& blob);
		~CBlob();


		bool operator ==(const CBlob& blob) const;
		bool operator !=(const CBlob& blob) const;
		bool operator <(const CBlob& blob) const;
		bool operator >(const CBlob& blob) const;
		bool operator <=(const CBlob& blob) const;
		bool operator >=(const CBlob& blob) const;

		int  equals(const unsigned char *buffer, size_t _size = (size_t)-1) const;
		int  equals(const CBlob& blob) const;

		int  compare(const unsigned char *buffer, size_t _size = (size_t)-1) const;
		int  compare(const CBlob& blob) const;

		void attach(const byte* pBuf, uint32 nSize);
		byte* deattach();
		void copy(const byte* pBuf, uint32 nSize);

		void  resize(uint32 nSize);
		bool   empty() const;
		uint32 size() const;
		uint32 capacity() const;

		const byte* buffer() const;
        byte* buffer();

		void clear(bool bDel = false);
	private:
		byte* m_pBuffer;
		alloc_t *m_pAlloc;
		simple_alloc_t m_alloc;
		uint32 m_nSize;
		uint32 m_nCapacity;
		bool m_bAttach;
	};
}

#endif