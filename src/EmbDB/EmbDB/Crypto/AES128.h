#ifndef _EMBEDDED_DATABASE_AES_128_H_
#define _EMBEDDED_DATABASE_AES_128_H_

#include "BaseAES.h"

namespace embDB
{


	namespace Crypto
	{
		class CAES128 : public CBaseAES
		{
			public:
				CAES128(){}
				~CAES128(){}

				virtual uint32 getKeyLength() const {return 16;}
			protected:
				virtual uint32 getNK() const {return 4;}
				virtual uint32 getNR() const {return 10;}
		};
	}
}

#endif