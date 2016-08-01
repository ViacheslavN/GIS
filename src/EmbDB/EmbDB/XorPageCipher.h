#ifndef _EMBEDDED_DATABASE_XOR_PAGE_CRYPTO_H_
#define _EMBEDDED_DATABASE_XOR_PAGE_CRYPTO_H_

#include "PageCipher.h"

namespace embDB
{
	class CXorCipher : public IPageCipher
	{
		public:
			CXorCipher();
			~CXorCipher();

			virtual bool encrypt(byte* pBuf, uint32 len);
			virtual bool decrypt(byte* pBuf, uint32 len);
			virtual bool encrypt(byte* pSrcBuf, byte* pDstBuf, uint32 len);
			virtual bool decrypt(byte* pSrcBuf, byte* pDstBuf, uint32 len);
	};
}

#endif