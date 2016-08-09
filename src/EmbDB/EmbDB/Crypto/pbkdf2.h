#ifndef _EMBEDDED_DATABASE_PBKDF_2_H_
#define _EMBEDDED_DATABASE_PBKDF_2_H_
 
#include "HMAC.h"

namespace embDB
{
	namespace Crypto
	{
		template<class PRF>
		void
			pbkdf2 (const byte *password, uint32 nPwdSize, const byte * salt, uint32 nSaltSize, 
				uint32  rounds, byte *key,  uint32 keylen)
		{
			PRF prf (password, nPwdSize);
			uint32 i = 0;
			uint32 nPos = 0;
			while (keylen > 0) {
				++i;
				CommonLib::CBlob block_number;
				block_number.push_back ((i >> 24) & 0xff);
				block_number.push_back ((i >> 16) & 0xff);
				block_number.push_back ((i >>  8) & 0xff);
				block_number.push_back (i & 0xff);


				CommonLib::CBlob u;
				CommonLib::CBlob t;
				prf.add(salt, nSaltSize);
				prf.add(block_number);
				prf.digest (u);
				t.copy(u.buffer(), u.size());

				for (std::size_t j = 1; j < rounds; ++j) 
				{
					prf.add (u);
					prf.digest(u);
					for (std::size_t k = 0; k < u.size (); ++k)
						t[k] ^= u[k];
				}
				uint32 n = min (keylen, t.size ());
				memcpy(key + nPos, t.buffer(), n);
				nPos += n;
				keylen -= n;
			}
		}
	}
}

#endif