#ifndef _EMBEDDED_DATABASE_SHA_256_H_
#define _EMBEDDED_DATABASE_SHA_256_H_
#include "CommonLibrary/general.h"

namespace embDB
{

 class SHA256 
 {

   

 public:
	 SHA256();
	 ~SHA256();

    void getHash(byte* pData, uint32 nLen ); 
	byte *Result() {return &result[0];}
 private:
	 void calculateHash(byte* pData, uint32 nLen,
		byte* byteBuff, uint32* intBuff);

	 uint32 readInt(byte* b, int i) ;
	 void writeInt(byte* b, int i, uint32 value);
	 int getIntCount(int byteCount) const;

 private:
	 static const uint32 K[64];
	 static const uint32 HH[8];

	 byte result[32];
	 uint32 w[64];
	 uint32 hh[8];
 };
   
}

#endif