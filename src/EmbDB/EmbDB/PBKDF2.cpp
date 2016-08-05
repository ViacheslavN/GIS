#include "stdafx.h"
#include "PBKDF2.h"
#include "SHA25.h"
#ifdef _USE_CRYPTOPP_LIB_
	#include "cryptopp/pwdbased.h"
#endif

namespace embDB
{




	bool CPBKDF2::PBKDF2(byte* pPWD, uint32 nPWdSize, byte* pSalt, uint32 nSaltSize, byte* pResult, uint32 nResultLen, uint32 nIterations)
	{
#ifndef _USE_CRYPTOPP_LIB_

	//TO DO need true PBKDF2
		byte *pMessage = new byte[nPWdSize + nSaltSize];
		memcpy(pMessage, pPWD, nPWdSize);
		memcpy(pMessage + nPWdSize, pSalt, nSaltSize);

		SHA256 sha;
		byte result[32];
		sha.getHash(pMessage, nPWdSize + nSaltSize, result);
		
		 

		for (int i = 0; i < nIterations; i++)
		{
			sha.getHash(result, 32, result);
		}
		memcpy(pResult, result, min(nPWdSize + nSaltSize, nResultLen));
		delete pMessage;

#else

		CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA256> pbkdf2;
		pbkdf2.DeriveKey(
			pResult, 
			nResultLen, 
			0, 
			pPWD, 
			nPWdSize,
			pSalt, 
			nSaltSize,
			nIterations
			);

 

#endif

		return true;
	}


}