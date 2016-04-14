#ifndef _EMBEDDED_DATABASE_COMPRESS_UTILS_H_
#define _EMBEDDED_DATABASE_COMPRESS_UTILS_H_
#include "MathUtils.h"

namespace embDB
{

	enum eTypeFreq
	{
		etfByte = 0,
		etfShort = 1,
		etfInt32 = 2,
		etFInt64 = 3
	};

	template<class TValue>
	double CalcRowBitSize(const TValue *pFreq, uint32 nFreqCount, uint32 nDiffsLen, uint32 nCount)
	{
		double dBitRowSize  = 0;
		if(nDiffsLen > 1)
		{
			for (uint32 i = 0; i < nFreqCount; ++i)
			{
				if(pFreq[i] == 0)
					continue;
				double dFreq = pFreq[i];
				double dLog2 = mathUtils::Log2((double)nCount/dFreq); 
				dBitRowSize += (dFreq* dLog2);

			}
		}

		return dBitRowSize;
	}

	template<class T>
	eTypeFreq GetTypeFromValue(T nValue)
	{
		if(nValue < 0xFF)
			return etfByte;
		else if(nValue < 0xFFFF)
			return etfShort;
		else if(nValue < 0xFFFFFFFF)
			return etfInt32;
		return etFInt64;

	}


	static uint32 GetLenForDiffLen(eTypeFreq nTypeFreq, uint32 nDiffsLen) 
	{
		switch(nTypeFreq)
		{
		case etfByte:
			return nDiffsLen;
			break;
		case etfShort:
			return nDiffsLen * sizeof(uint16);
			break;
		case etfInt32:
			return nDiffsLen * sizeof(uint32);
			break;
		case etFInt64:
			return nDiffsLen * sizeof(uint64);
			break;
		}
		assert(false);
		return nDiffsLen;
	}
}
#endif