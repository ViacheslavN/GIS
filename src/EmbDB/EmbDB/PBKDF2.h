#ifndef _EMBEDDED_DATABASE_PBKDF2_H_
#define _EMBEDDED_DATABASE_PBKDF2_H_


#include "CommonLibrary/general.h"
#include "CommonLibrary/alloc_t.h"

//#define  _USE_CRYPTOPP_LIB_

 
namespace embDB
{
	class CPBKDF2
	{
	public:
		static bool PBKDF2(byte* pPWD, uint32 nPWdSize, byte* pSalt, uint32 nSaltSize, byte* pResult, uint32 nResultLen, uint32 nIterations);
	};
	
}

#endif