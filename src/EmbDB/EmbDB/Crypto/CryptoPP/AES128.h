#ifndef _EMBEDDED_DATABASE_AES_128_CRYPTO_PP_H_
#define _EMBEDDED_DATABASE_AES_128_CRYPTO_PP_H_

#include "AESBaseCryptoPP.h"

namespace embDB
{

	namespace Crypto
	{
		namespace CryptoPPWrap
		{


			class CAES128 : public CAESBase
			{
			public:
				CAES128(){}
				~CAES128(){}

				virtual uint32 getKeyLength() const {return 16;}
	 		};
		}
	}
}