#ifndef _EMBEDDED_DATABASE_PAGE_CRYPTO_H_
#define _EMBEDDED_DATABASE_PAGE_CRYPTO_H_


namespace embDB
{

	class IPageCrypto
	{
		public:
			IPageCrypto(){}
			virtual ~IPageCrypto(){}
			virtual bool encrypt(byte* pBuf, size_t len) = 0;
			virtual bool decrypt(byte* pBuf, size_t len) = 0;

			virtual bool encrypt(byte* inBuf, size_t inLen, byte* outBuf, size_t outLen) = 0;
			virtual bool decrypt(byte* inBuf, size_t inLen, byte* outBuf, size_t outLen) = 0;

	};
}
#endif