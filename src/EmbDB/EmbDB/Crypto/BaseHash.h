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

		class CBaseHash  
		{
		protected:
			enum { INIT, ADD, FINISH } m_state;
		public:
			CBaseHash (CommonLib::alloc_t* pAlloc = NULL) : m_state (INIT), m_buf(pAlloc), m_nLen(0) {}
			virtual ~CBaseHash () {}
			virtual void reset ();
			virtual void add (const CommonLib::CBlob& blob);
			virtual void add (const byte *pData, uint32 nSize);
			virtual void finish ();

			virtual void digest (CommonLib::CBlob& blob);
			virtual bool digest (byte *pData, uint32 nSize) = 0;
			virtual uint32 blocksize () const = 0;
			virtual uint32 length() const = 0;
		protected:
			virtual void init_sum () = 0;
			virtual void update_sum (const byte *pData) = 0;
			virtual void last_sum () = 0;
		protected:
			CommonLib::CBlob m_buf;
			uint32 m_nLen;
		};

	}
};

#endif