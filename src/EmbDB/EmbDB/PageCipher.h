#ifndef _EMBEDDED_DATABASE_PAGE_CRYPTO_H_
#define _EMBEDDED_DATABASE_PAGE_CRYPTO_H_

#include "FilePage.h"
#include <vector>
namespace embDB
{
	class IBlockCipher;
	class CPageCipher
	{
		public:
			CPageCipher(byte* pPWD, uint32 nLenPwd, byte* pSalt, QryptoALG qryptoAlg);
			~CPageCipher();
			bool encrypt(CFilePage *pFilePage);
			bool decrypt(CFilePage *pFilePage);
			bool encrypt(CFilePage *pFilePage, byte* pDstBuf, uint32 len);
			bool decrypt(CFilePage *pFilePage, byte* pDstBuf, uint32 len);
		private:
			std::vector<byte> m_vecKey;
			std::vector<byte> m_vecInitVector;
			std::auto_ptr<IBlockCipher> m_pCipher;
			std::auto_ptr<IBlockCipher>  m_pCipherForInitVector;

			QryptoALG m_qryptoAlg;
	};
}
#endif