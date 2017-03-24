#ifndef _EMBEDDED_DATABASE_COMPRESS_UTILS_H_
#define _EMBEDDED_DATABASE_COMPRESS_UTILS_H_
#include "MathUtils.h"

namespace embDB
{

	enum eCompressDataType
	{
		ectByte = 0,
		ectUInt16 = 1,
		ectUInt32 = 2,
		ectUInt64 = 3
	};


	static uint32 GetSizeTypeValue(eCompressDataType type)
	{
		switch (type)
		{
		case ectByte:
			return 1;
		case ectUInt16:
			return 2;
		case ectUInt32:
			return 4;
		case ectUInt64:
			return 8;
		}
		return 0;
	}

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
	eCompressDataType GetCompressType(T nValue)
	{
		if(nValue < 0xFF)
			return ectByte;
		else if(nValue - 1 < 0xFFFF)
			return ectUInt16;
		else if(nValue - 1 < 0xFFFFFFFF)
			return ectUInt32;
		return ectUInt64;

	}


	static uint32 GetLenForDiffLen(eCompressDataType nTypeFreq, uint32 nDiffsLen) 
	{
		switch(nTypeFreq)
		{
		case ectByte:
			return nDiffsLen;
			break;
		case ectUInt16:
			return nDiffsLen * sizeof(uint16);
			break;
		case ectUInt32:
			return nDiffsLen * sizeof(uint32);
			break;
		case ectUInt64:
			return nDiffsLen * sizeof(uint64);
			break;
		}
		assert(false);
		return nDiffsLen;
	}



	template<class TValue>
	void WriteCompressValue(eCompressDataType nType, TValue value, CommonLib::IWriteStream* pStream)
	{
		switch(nType)
		{
		case ectByte:
			pStream->write((byte)value);
			break;
		case ectUInt16:
			pStream->write((uint16)value);
			break;
		case ectUInt32:
			pStream->write((uint32)value);
			break;
		case ectUInt64:
			pStream->write((uint64)value);
			break;
		}
	}

	template<class TValue>
	void ReadCompressValue(eCompressDataType nType, TValue& value, CommonLib::IReadStream* pStream)
	{
		switch(nType)
		{
		case ectByte:
			pStream->readByte(value);
			break;
		case ectUInt16:
			pStream->readintu16(value);
			break;
		case ectUInt32:
			pStream->readIntu32(value);
			break;
		case ectUInt64:
			pStream->readIntu64(value);
			break;
		}
	}


	template<class TValue>
	TValue ReadCompressValue(eCompressDataType nType, CommonLib::IReadStream* pStream)
	{
		switch(nType)
		{
		case ectByte:
			return (TValue)pStream->readByte();
			break;
		case ectUInt16:
			return (TValue)pStream->readintu16();
			break;
		case ectUInt32:
			return (TValue)pStream->readIntu32();
			break;
		case ectUInt64:
			return (TValue)pStream->readIntu64();
			break;
		}

		assert(false);
		return 0;
	}
}
#endif