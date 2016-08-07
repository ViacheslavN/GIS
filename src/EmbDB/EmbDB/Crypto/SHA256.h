#ifndef _EMBEDDED_DATABASE_SHA_256_H_
#define _EMBEDDED_DATABASE_SHA_256_H_

#include "BaseHash.h"

namespace embDB
{
	namespace Crypto
	{

		class CSHA256 : public CBaseHash
		{
		  public:
				CSHA256(CommonLib::alloc_t *pAlloc = NULL);
				~CSHA256();
	
		  public:
				virtual bool digest (byte *pData, uint32 nSize);
				virtual uint32 blocksize () const;
			protected:
				virtual void init_sum ();
				virtual void update_sum (byte *pData);
				virtual void last_sum ();
			private:
				uint32 m_sum[8];
		};

	}
}

#endif