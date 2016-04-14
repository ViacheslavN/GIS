#include "stdafx.h"
#include "../../EmbDB/CompressDoubleDiff.h"
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

	embDB::double_cast dVal, dVal1;

	dVal.val = 2.00000000001;
	uint64 nMat = dVal.parts.mantisa;
	uint64 nExp = dVal.parts.exponent;

	uint64 nDiv = (((uint64)2) << 51);
	dVal.parts.exponent = 2;

	uint32 nBits = embDB::TFindMostSigBit::FMSB(nMat);

	uint64 nMat1 = nMat/nDiv;

	uint64 n1 = nDiv/nMat;
	uint64 n2 = nDiv%nMat;


	uint32 nBits1 = embDB::TFindMostSigBit::FMSB(n1);
	uint32 nBits2 = embDB::TFindMostSigBit::FMSB(n2);


	double dVal2 = (1 + (double)nMat/nDiv) * pow((double)2, (double)nExp - 1023);



	embDB::TDoubleDiffCompreessor doubleCompressorDiff;
	embDB::TDoubleCompreessor doubleCompressor;

	embDB::TBPVector<double> vec;
	double dd = 1;
	for (int32 i = 1; i < 100000; ++i)
	{
		vec.push_back(dd);
		doubleCompressorDiff.AddSymbol(vec.size(), vec.size() - 1, dd, vec);

		uint32 nSize = doubleCompressorDiff.GetComressSize();
		if(nSize > 8192)
		{
			int dd = 0;
			dd++;
			break;
		}

		dd += i;


	}


	dd = 1;
	for (int32 i = 1; i < 100000; ++i)
	{
		vec.push_back(dd);
		doubleCompressor.AddSymbol(vec.size(), vec.size() - 1, dd, vec);

		uint32 nSize = doubleCompressor.GetComressSize();
		if(nSize > 8192)
		{
			int dd = 0;
			dd++;
			break;
		}

		dd += i *10.3254;


	}

}