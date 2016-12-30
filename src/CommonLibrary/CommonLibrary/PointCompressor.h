#ifndef _LIB_COMMON_GEO_SHAPE_POINT_COMPRESSOR_2_H_
#define _LIB_COMMON_GEO_SHAPE_POINT_COMPRESSOR_2_H_

#include "IGeoShape.h"
#include "PodVector.h"
#include "GeoShape.h"
#include "SignCompressor2.h"

namespace CommonLib
{

	class CPointCompressor
	{
		public:
			CPointCompressor();
			~CPointCompressor();

			bool compress(const CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IWriteStream *pStream);
			bool decompress(CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IReadStream *pStream);
	private:
			
			typedef TNumLemCompressor2<uint64, TFindMostSigBit, 64> TNumLen32;

			TSignCompressor2 m_SignX;
			TSignCompressor2 m_SignY;

			TNumLen32 m_PointX;
			TNumLen32 m_PointY;
	};
}


#endif