#ifndef _EMBEDDED_DATABASE_PAGE_CRYPTO_H_
#define _EMBEDDED_DATABASE_PAGE_CRYPTO_H_


namespace embDB
{

	class IPageCipher
	{
		public:
			IPageCipher(){}
			virtual ~IPageCipher(){}
			virtual bool encrypt(byte* pBuf, uint32 len) = 0;
			virtual bool decrypt(byte* pBuf, uint32 len) = 0;
			virtual bool encrypt(byte* pSrcBuf, byte* pDstBuf, uint32 len) = 0;
			virtual bool decrypt(byte* pSrcBuf, byte* pDstBuf, uint32 len) = 0;
	};
}
#endif