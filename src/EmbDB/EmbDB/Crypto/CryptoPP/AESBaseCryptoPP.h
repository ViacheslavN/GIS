#ifndef _EMBEDDED_DATABASE_BASE_AES_CRYPTO_PP_H_
#define _EMBEDDED_DATABASE_BASE_AES_CRYPTO_PP_H_

#define  _USE_CRYPTOPP_LIB_

#ifdef _USE_CRYPTOPP_LIB_



#include "../PageCipher.h"


#include "cryptopp/modes.h"
#include "cryptopp/aes.h"
#include "cryptopp/filters.h"


namespace embDB
{

	namespace Crypto
	{
		namespace CryptoPPWrap
		{
			class CAESBase : public IBlockCipher
			{
				public:
					CAESBase();
					~CAESBase();



					virtual bool setEncryptKey(byte *pKey, uint32 nSize);
					virtual bool setDecryptKey(byte *pKey, uint32 nSize);
					virtual bool encrypt(byte* pPlain, uint32 len);
					virtual bool decrypt(byte* pSecret, uint32 len);
					virtual bool encrypt(byte* pPlain, byte* pSecret, uint32 len);
					virtual bool decrypt(byte* pSecret, byte* pPlain, uint32 len);

					virtual uint32 getBlockSize() const {return CryptoPP::AES::BLOCKSIZE; }
					 


				private:
					byte iv[ CryptoPP::AES::BLOCKSIZE ];


					std::auto_ptr<CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption> m_pEncryption;
					std::auto_ptr<CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption> m_pDecryption;
			};
		}

	}


}

#endif

#endif
