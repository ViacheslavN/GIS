#ifndef _EMBEDDED_DATABASE_SHA_512_H_
#define _EMBEDDED_DATABASE_SHA_512_H_

#include "BaseHash.h"

namespace embDB
{
	namespace Crypto
	{

		class CSHA512 : public CBaseHash
		{
		public:
			CSHA512(CommonLib::alloc_t *pAlloc = NULL);
			~CSHA512();

		public:
			virtual bool digest (byte *pData, uint32 nSize);
			virtual uint32 blocksize () const;
		protected:
			virtual void init_sum ();
			virtual void update_sum (byte *pData);
			virtual void last_sum ();
		private:
			uint64 m_sum[8];
		};

	}
}

#endif