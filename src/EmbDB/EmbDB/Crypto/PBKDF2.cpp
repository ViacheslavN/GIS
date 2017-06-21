#include "stdafx.h"
#include "PBKDF2.h"
 
#ifdef _USE_CRYPTOPP_LIB_
	#include "cryptopp/pwdbased.h"
#else
	#include "SHA256.h"
	#include "pbkdf2Impl.h"
#endif

namespace embDB
{




	bool CPBKDF2::PBKDF2(const byte* pPWD, uint32 nPWdSize, const byte* pSalt, uint32 nSaltSize, byte* pResult, uint32 nResultLen, uint32 nIterations)
	{
#ifndef _USE_CRYPTOPP_LIB_
		Crypto::pbkdf2<Crypto::HMAC<Crypto::CSHA256> >(pPWD, nPWdSize, pSalt, nSaltSize, nIterations, pResult, nResultLen);

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