#ifndef _EMBEDDED_DATABASE_AES_256_CRYPTO_PP_H_
#define _EMBEDDED_DATABASE_AES_256_CRYPTO_PP_H_

#include "AESBaseCryptoPP.h"

namespace embDB
{

	namespace Crypto
	{
		namespace CryptoPPWrap
		{
			class CAES256 : public CAESBase
			{
			public:
				CAES256(){}
				~CAES256(){}

				virtual uint32 getKeyLength() const {return 32;}
			};
		}
	}
}