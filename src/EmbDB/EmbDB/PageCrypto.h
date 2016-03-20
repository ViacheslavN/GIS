#ifndef _EMBEDDED_DATABASE_PAGE_CRYPTO_H_
#define _EMBEDDED_DATABASE_PAGE_CRYPTO_H_


namespace embDB
{

	class IPageCrypto
	{
		public:
			IPageCrypto(){}
			virtual ~IPageCrypto(){}
			virtual bool encrypt(byte* pBuf, uint32 len) = 0;
			virtual bool decrypt(byte* pBuf, uint32 len) = 0;

			virtual bool encrypt(byte* inBuf, uint32 inLen, byte* outBuf, uint32 outLen) = 0;
			virtual bool decrypt(byte* inBuf, uint32 inLen, byte* outBuf, uint32 outLen) = 0;

	};
}
#endif