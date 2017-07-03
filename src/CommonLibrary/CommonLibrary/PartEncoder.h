#ifndef _LIB_COMMON_GEO_SHAPE_PART_COMPRESSOR_2_H_
#define _LIB_COMMON_GEO_SHAPE_PART_COMPRESSOR_2_H_
#include "SignedNumLenEncoder.h"
namespace CommonLib
{
 

	class CPartEncoder
	{
	public:
		CPartEncoder();
		~CPartEncoder();

		 
		void clear();
		void Reset();

		void BeginDecode(const CBlob& blob);
	private:
		void WriteFlag(uint32 nPos, CommonLib::IWriteStream* pStream);
		void ReadFlag(CommonLib::IReadStream* pStream);
		void EncodePart(const CGeoShape *pShp, CommonLib::IWriteStream* pStream);
		void DecodePart(CGeoShape *pShp, CommonLib::IReadStream* pStream);
	private:
		eCompressDataType m_nDataType;
		bool m_bNullPart;
		bool m_bCompressPart;
		typedef TNumLemCompressor2<uint64, TFindMostSigBit, 64> TNumLen64;
		TNumLen64 m_NumLen;

	};


}

#endif