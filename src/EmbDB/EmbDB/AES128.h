#ifndef _EMBEDDED_DATABASE_AES_
#define _EMBEDDED_DATABASE_AES_

#include "BlockClipher.h"

#define _USE_CRYPTOPP_LIB_

#ifdef _USE_CRYPTOPP_LIB_
	#include "cryptopp/modes.h"
	#include "cryptopp/aes.h"
	#include "cryptopp/filters.h"
#endif

namespace embDB
{
	class CAES128 : public IBlockCipher
	{
		public:
			CAES128();
			~CAES128();
			virtual bool setKey(byte *pKey, uint32 nLen);
			virtual bool encrypt(byte* pBuf, uint32 len);
			virtual bool decrypt(byte* pBuf, uint32 len);
			virtual bool encrypt(byte* pBuf, byte* pDst, uint32 len);
			virtual bool decrypt(byte* pBuf, byte* pDst, uint32 len);
			virtual int getKeyLength() const {return 16;}


	private:

#ifndef _USE_CRYPTOPP_LIB_
		void init();
		 uint32 getDec(uint32 t);
		 void encryptBlock(byte *in,byte *out, uint32 off);
		 void decryptBlock(byte* in, byte*out, uint32 off);
#endif
	private:

#ifdef _USE_CRYPTOPP_LIB_


 byte key[ CryptoPP::AES::DEFAULT_KEYLENGTH ];
 byte iv[ CryptoPP::AES::BLOCKSIZE ];

#else
		uint32 encKey[44];
		uint32 decKey[44];
		uint32 RCON[10];
		uint32 FS[256];
		uint32 FT0[256];
		uint32 FT1[256];
		uint32 FT2[256];
		uint32 FT3[256];
		uint32 RS[256];
		uint32 RT0[256];
		uint32 RT1[256];
		uint32 RT2[256];
		uint32 RT3[256];
#endif
	};
}

#endif