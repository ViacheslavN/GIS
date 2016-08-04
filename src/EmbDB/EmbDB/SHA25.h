#ifndef _EMBEDDED_DATABASE_SHA_256_H_
#define _EMBEDDED_DATABASE_SHA_256_H_


#include "CommonLibrary/general.h"
#include "CommonLibrary/alloc_t.h"

//#define  _USE_CRYPTOPP_LIB_

#ifdef _USE_CRYPTOPP_LIB_
	#include "cryptopp/sha.h"
#endif
namespace embDB
{

 class SHA256 
 {

   

 public:
	 SHA256(CommonLib::alloc_t *pAlloc = NULL);
	 ~SHA256();

    void getHash(byte* pData, uint32 nLen, byte finalHash[32] ); 
 
 private:
	 struct s_hashValues
	 {
		 uint32 h0;
		 uint32 h1;
		 uint32 h2;
		 uint32 h3;
		 uint32 h4;
		 uint32 h5;
		 uint32 h6;
		 uint32 h7;

		 s_hashValues()
		 {
			 // Big-endian constants
			h0 = 0x6a09e667;
			h1 = 0xbb67ae85;
			h2 = 0x3c6ef372;
			h3 = 0xa54ff53a;
			h4 = 0x510e527f;
			h5 = 0x9b05688c;
			h6 = 0x1f83d9ab;
			h7 = 0x5be0cd19;

			 // Little-endian constants
			//h0 = 0x67e6096a;
			//h1 = 0x85ae67bb;
			//h2 = 0x72f36e3c;
			//h3 = 0x3af54fa5;
			//h4 = 0x7f520e51;
			//h5 = 0x8c68059b;
			//h6 = 0xabd9831f;
			//h7 = 0x19cde05b;
		 }
	 };

	 uint32 preProcess(byte* &ppMessage, byte* message, uint32 len);


	 void scheduler(byte* messageChunk, uint32* schedulerArray);
	 void compressor(uint32* schedulerArray, s_hashValues &hValues);
 

 private:


	 static const uint32 k[64];
	CommonLib::alloc_t *m_pAlloc;
	CommonLib::simple_alloc_t m_alloc;
	 
 
 };
   
}

#endif