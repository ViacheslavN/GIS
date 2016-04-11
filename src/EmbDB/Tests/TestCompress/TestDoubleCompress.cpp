#include "stdafx.h"
#include "../../EmbDB/CompressDouble.h"



struct SDoubleCast 
{
	bool m_bSign;
	uint32 m_nExponent;
	uint64 m_nFraction;
};


union doublebits {
	double d;
	struct {
		unsigned long long mantissa1:52;
		unsigned long long exponent:11;
		unsigned long long negative:1;
		
	};
};

void TestDoubleCompress()
{
	/*
#if __BYTE_ORDER == __LITTLE_ENDIAN
# if    __FLOAT_WORD_ORDER == __BIG_ENDIAN
	unsigned int mantissa0:20;
	unsigned int exponent:11;
	unsigned int negative:1;
	unsigned int mantissa1:32;
# else
	 /*Together these comprise the mantissa.  
	unsigned int mantissa1:32;
	unsigned int mantissa0:20;
	unsigned int exponent:11;
	unsigned int negative:1;
# endif*/


	bool bIsBigEndign = CommonLib::IStream::isBigEndian();

	int n = 1;
	// little endian if true
	bool blittleEndian = false;
	if(*(char *)&n == 1) 
	{
		blittleEndian = true;
	}

	 embDB::double_cast dc, dc1;
	 dc.d = 0;
	 dc1.d = 0;
	 dc.d = -1.456766778;
	 dc1.d = 1.0;

	 double dd = 112564.0;
	

	 boolean isNegative = ((uint64&)dd & 0x8000000000000000L) != 0; 
	 long exponent      = ((uint64&)dd & 0x7ff0000000000000L) >> 52;
	 uint64 mantissa      =  (uint64&)dd & 0x000fffffffffffffL;

	 dc1.d = dd;

	 doublebits db;
	 db.d = dd;


	 db.mantissa1 = 12;

	 int dd1 = 0;
	 dd1++;

}