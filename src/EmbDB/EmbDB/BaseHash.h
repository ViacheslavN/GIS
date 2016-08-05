#ifndef _EMBEDDED_DATABASE_BASE_HASH_H_
#define _EMBEDDED_DATABASE_BASE_HASH_H_


#include "CommonLibrary/general.h"
#include "CommonLibrary/alloc_t.h"
#include "CommonLibrary/blob.h"
#include "CommonLibrary/PodVector.h"
 
namespace embDB
{
	namespace Crypto
	{

		class CHashBase  
		{
		protected:
			enum { INIT, ADD, FINISH } m_state;
		public:
			CHashBase (CommonLib::alloc_t* pAlloc) : m_state (INIT), m_buf(pAlloc), m_nLen(0) {}
			virtual ~CHashBase () {}
			virtual CHashBase& reset ();
			virtual CHashBase& add (byte *pData, uint32 nSize);
			virtual CHashBase& finish ();

			virtual bool digest (CommonLib::CBlob& blob) = 0;
			virtual bool digest (byte *pData, uint32 nSize) = 0;
			virtual uint32 blocksize () const = 0;
		protected:
			virtual void init_sum () = 0;
			virtual void update_sum (byte *pData, uint32 nSize) = 0;
			virtual void last_sum () = 0;
		protected:
			CommonLib::CBlob m_buf;
			uint32 m_nLen;
		};

	}
};

#endif