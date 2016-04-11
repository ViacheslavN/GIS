#ifndef _EMBEDDED_DATABASE_DOUBLE_COMPRESS_H_
#define _EMBEDDED_DATABASE_DOUBLE_COMPRESS_H_
#include "CommonLibrary/FixedBitStream.h"
#include <map>
#include "MathUtils.h"
#include "BPVector.h"



namespace embDB
{
	typedef union {
		float f;
		struct {
			uint32 mantisa : 23;
			uint32 exponent : 8;
			uint32 sign : 1;
		} parts;
	} float_cast;


	typedef union {
		double d;
		struct {
	
			uint64 mantisa : 52;
			uint64 exponent : 11;
			uint64 sign : 1;
			
		
		} parts;
	} double_cast;

	template <class TDouble, class TDoubleCast, uint32 nMantisaLen, uint32 nExponentLen>
	class TDoubleCompress
	{

	};
}

#endif


/*#include <stdio.h>
typedef union {
	float f;
	struct {
		unsigned int mantisa : 23;
		unsigned int exponent : 8;
		unsigned int sign : 1;
	} parts;
} double_cast;

int main() {
	double_cast d1;
	d1.f = 0.15625;
	printf("sign = %x\n",d1.parts.sign);
	printf("exponent = %x\n",d1.parts.exponent);
	printf("mantisa = %x\n",d1.parts.mantisa);
	return 0;
}*/