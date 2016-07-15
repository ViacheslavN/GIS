#ifndef _EMBEDDED_DATABASE_XOR_PAGE_CRYPTO_H_
#define _EMBEDDED_DATABASE_XOR_PAGE_CRYPTO_H_

#include "PageCrypto.h"

namespace embDB
{
	class CXorCrypto : public IPageCrypto
	{
		public:
			CXorCrypto();
			~CXorCrypto();
	};
}

#endif