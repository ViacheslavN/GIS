#ifndef _LIB_COMMON_GEO_SHAPE_COMPRESSOR_H_
#define _LIB_COMMON_GEO_SHAPE_COMPRESSOR_H_
#include "IGeoShape.h"
#include "PodVector.h"
#include "GeoShape.h"
namespace CommonLib
{

	class CGeoShape;

	class ShapeCompressor
	{
		public:
			ShapeCompressor(CommonLib::alloc_t *pAlloc);
			~ShapeCompressor();


			bool compress(CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IWriteStream *pStream);
			bool decompress(CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IReadStream *pStream);
		private:
			CommonLib::alloc_t *m_pAlloc;
	};

}

#endif