#ifndef _EMBEDDED_DATABASE_BASE_AES_H_
#define _EMBEDDED_DATABASE_BASE_AES_H_

#include "CommonLibrary/general.h"
#include "BlockClipher.h"
 

namespace embDB
{


	namespace Crypto
	{
		class CBaseAES : public IBlockCipher
		{
			public:
				CBaseAES();
				~CBaseAES();

					enum { BLOCKSIZE = 16 };

				virtual bool setEncryptKey(byte *pKey, uint32 nSize);
				virtual bool setDecryptKey(byte *pKey, uint32 nSize);
				virtual uint32 getBlockSize() const {return BLOCKSIZE;}
				virtual uint32 getKeyLength() const  = 0;
				
				virtual bool encrypt(byte* pPlain, uint32 len);
				virtual bool decrypt(byte* pSecret, uint32 len);
				virtual bool encrypt(byte* pPlain, byte* pSecret, uint32 len);
				virtual bool decrypt(byte* pSecret, byte* pPlain, uint32 len);
			protected:
				void encryptBlock(byte *pPlain,byte *pSecret);
				void decryptBlock(byte* pSecret, byte*pPlain);


				void schedule_encrypt_keys (int const nk, int const nr, uint32 *rk);
				void schedule_decrypt_keys (int const nk, int const nr, uint32 *rk);

				virtual uint32 getNK() const = 0; 
				virtual uint32 getNR() const = 0; 
			protected:
				int m_nRounds;
				uint32 m_keys[60];
				uint32 m_ikeys[60];
		};
	}
}

#endif