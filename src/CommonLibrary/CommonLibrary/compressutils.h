#ifndef _LIB_COMMON_COMPRESS_UTILS_CODER_H_
#define _LIB_COMMON_COMPRESS_UTILS_CODER_H_
#include "stream.h"
namespace CommonLib
{

	template<class _TType>
	struct TDefSign { typedef _TType TSignType; };

	template<> struct TDefSign< byte > { typedef char TSignType; };
	template<> struct TDefSign< uint16 > { typedef int16 TSignType; };
	template<> struct TDefSign< uint32 > { typedef int32 TSignType; };
	template<> struct TDefSign< uint64 > { typedef int64 TSignType; };

	enum eCompressDataType
	{
		dtType8 =  0,
		dtType16 = 1,
		dtType32 = 2,
		dtType64 = 3
	};


	template<class TValue>
	double CalcRowBitSize(const TValue *pFreq, uint32 nFreqCount, uint32 nDiffsLen, uint32 nCount)
	{
		double dBitRowSize = 0;
		if (nDiffsLen > 1)
		{
			for (uint32 i = 0; i < nFreqCount; ++i)
			{
				if (pFreq[i] == 0)
					continue;
				double dFreq = pFreq[i];
				double dLog2 = mathUtils::Log2((double)nCount / dFreq);
				dBitRowSize += (dFreq* dLog2);

			}
		}

		return dBitRowSize;
	}

	template<class TValue>
	eCompressDataType GetCompressType(TValue nValue)
	{
		if(nValue < 0xFF + 1)
			return dtType8;
		else if(nValue - 1 < 0xFFFF)
			return dtType16;
		else if(nValue - 1 < 0xFFFFFFFF)
			return dtType32;
		return dtType64;
	}


	template<class TValue>
	void WriteValue(TValue nValue, eCompressDataType type, CommonLib::IWriteStream *pStream)
	{
		switch(type)
		{
			case dtType8:
				pStream->write((byte)nValue);
				break;
			case dtType16:
				pStream->write((uint16)nValue);
				break;
			case dtType32:
				pStream->write((uint32)nValue);
				break;
			case dtType64:
				pStream->write((uint64)nValue);
				break;
			default:
				assert(false);
				break;
		}

	}

	template<class TValue>
	TValue ReadValue(eCompressDataType type, CommonLib::IReadStream *pStream)
	{
		switch(type)
		{
		case dtType8:
			return (TValue)pStream->readByte();
			break;
		case dtType16:
			return (TValue)pStream->readintu16();
			break;
		case dtType32:
			return (TValue)pStream->readIntu32();
			break;
		case dtType64:
			return (TValue)pStream->readIntu64();
			break;
		}

		assert(false);
		return TValue();
	}

	static uint32 GetSizeTypeValue(eCompressDataType type)
	{
		switch(type)
		{
		case dtType8:
			return 1;
			break;
		case dtType16:
			return 2;
			break;
		case dtType32:
			return 4;
			break;
		case dtType64:
			return 8;
			break;
		}
		return 0;
	}

	static eCompressDataType GetTypeFromBitsLen(uint32 nLenBits)
	{
		if(nLenBits > 31)
			return dtType64;
		if(nLenBits > 15)
			return dtType32;
		if(nLenBits > 7)
			return dtType16;

		return dtType8;
	}


   // static int bits_lens[] = {0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3};
	  static int bits_lens[] = {0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4};
	struct TFindMostSigBit
	{

		
/*#ifdef _WIN32
		uint16 FMSB(uint32 x)
		{
			int32 y;
			__asm
			{
				bsr eax, x
				mov y, eax
			}
			return (uint16)y;
		}
#endif*/
	static	int FMSB(uint16 val16)
		{
			int bits = 0;
			if(val16 > 0xff){
				bits += 8;
				val16 >>= 8;
			}
			if(val16 > 0xf){
				bits += 4;
				val16 >>= 4;
			}
			bits += bits_lens[val16];
			return bits;
		}


	static	int FMSB(uint32 val32)
		{
			int bits = 0;
		 
			if(val32 > 0xffff){
				bits = 16;
				val32 >>= 16;
			}
			if(val32 > 0xff){
				bits += 8;
				val32 >>= 8;
			}
			if(val32 > 0xf){
				bits += 4;
				val32 >>= 4;
			}
			bits += bits_lens[val32];
			return bits;
		}


	static	int FMSB(uint64 val64)
		{
			int bits = 0;
			uint32 val32;

			if(val64 > 0xffffffff)
			{
				val32 = (uint32)(val64 >> 32);
				bits = 32;
			}
			else
				val32 = (unsigned int)val64;
			bits += FMSB(val32);
			return bits;
		}

	static	int FMSB(int64 val64)
		{
			 return FMSB(uint64(val64) );
		}

	};


	struct TSignedFindMostSigBit : public TFindMostSigBit
	{
		int FMSB(uint16 val16)
		{
			 
			return TFindMostSigBit::FMSB(val16) + 1;
		}


		int FMSB(uint32 val32)
		{
			return TFindMostSigBit::FMSB(val32) + 1;
		}


		int FMSB(uint64 val64)
		{
			return TFindMostSigBit::FMSB(val64) + 1;
		}

		int FMSB(int64 val64)
		{
			return FMSB(uint64(val64 < 0 ? -val64 : val64));
		}
		int FMSB(int32 val32)
		{
			return FMSB(uint32(val32 < 0 ? -val32 : val32));
		}
		int FMSB(int16 val16)
		{
			return FMSB(uint16(val16 < 0 ? -val16 : val16));
		}
	};

	static uint32 GetLenForDiffLen(eCompressDataType nTypeFreq, uint32 nDiffsLen) 
	{
		switch(nTypeFreq)
		{
		case dtType8:
			return nDiffsLen;
			break;
		case dtType16:
			return nDiffsLen * sizeof(uint16);
			break;
		case dtType32:
			return nDiffsLen * sizeof(uint32);
			break;
		case dtType64:
			return nDiffsLen * sizeof(uint64);
			break;
		}
		assert(false);
		return nDiffsLen;
	}


	struct shape_compress_params
	{

		shape_compress_params() : m_PointType(dtType64), m_dOffsetX(0), m_dOffsetY(0), m_nScaleX(8), m_nScaleY(8)
		{

		}

		eCompressDataType m_PointType;
		double m_dOffsetX;
		double m_dOffsetY;
		byte m_nScaleX;
		byte m_nScaleY;
	};


}

#endif