#ifndef _EMBEDDED_DATABASE_PAGE_CRYPTO_H_
#define _EMBEDDED_DATABASE_PAGE_CRYPTO_H_
#include "embDB.h"
#include "FilePage.h"
#include <vector>
#include "BlockClipher.h"
namespace embDB
{
	 
	class CPageCipher
	{
		public:
			CPageCipher(byte* pPWD, uint32 nLenPwd, byte* pSalt, byte* pIVSalt, uint32 nLenSalt, QryptoALG qryptoAlg);
			~CPageCipher();
			bool encrypt(CFilePage *pFilePage);
			bool decrypt(CFilePage *pFilePage);
			bool encrypt(CFilePage *pFilePage, byte* pDstBuf, uint32 len);
			bool decrypt(CFilePage *pFilePage, byte* pDstBuf, uint32 len);
		private:
			void init(byte* pPWD, uint32 nLenPwd, byte* pSalt,  byte* pIVSalt,uint32 nLenSalt);
			void CreateCiphers();
			void xorInitVector(byte* b, int off, int len, int64 p);
		private:
			std::vector<byte> m_vecKey;
			byte m_vecInitVector[CIPHER_FILE_BLOCK_SIZE];
			std::auto_ptr<IBlockCipher> m_pCipher;
			std::auto_ptr<IBlockCipher>  m_pCipherForInitVector;

			QryptoALG m_qryptoAlg;
	};
}
#endif