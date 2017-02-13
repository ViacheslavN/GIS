#ifndef _LIB_COMMON_GEO_SHAPE_POINT_COMPRESSOR_2_H_
#define _LIB_COMMON_GEO_SHAPE_POINT_COMPRESSOR_2_H_

#include "IGeoShape.h"
#include "PodVector.h"
#include "GeoShape.h"
#include "SignCompressor2.h"
#include "MemoryStream.h"

namespace CommonLib
{

	 
	class CPointCompressor
	{

		enum eFlags
		{
			compress_point = 0x1
		};

		public:
			CPointCompressor(CommonLib::alloc_t *pAlloc = nullptr);
			~CPointCompressor();

			void clear();

			bool compress(const CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IWriteStream *pStream);
			bool decompress(CGeoShape *pShp, CGeoShape::compress_params *pParams,	CommonLib::IReadStream *pStream);
	private:
		void WriteRawPoint(const CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IWriteStream *pStream);
		void ReadRawPoint( CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IReadStream *pStream);

		typedef TNumLemCompressor2<uint64, TFindMostSigBit, 64> TNumLen32;

		void calc(const CGeoShape *pShp, CGeoShape::compress_params *pParams);
		bool compressImpl(const CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IWriteStream *pStream);

	 
		void PreAddCoord(uint32 nPos, uint64 prev, uint64 next, TSignCompressor2& sign);
		void CompreessCoord(uint32 nPos, uint64 prev, uint64 next, FxBitWriteStream& bitStream,
			TSignCompressor2 &signCpmrpessor);

	private:
			
			byte m_nFlag;

			TSignCompressor2 m_SignX;
			TSignCompressor2 m_SignY;

			TNumLen32 m_Point;


			CWriteMemoryStream m_pCacheSignX;
			CWriteMemoryStream m_pCacheSignY;

			CWriteMemoryStream m_pCacheStreamX;
			CWriteMemoryStream m_pCacheStreamY;


	};
}


#endif