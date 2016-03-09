#ifndef _LIB_COMMON_GEO_SHAPE_COMPRESSOR_H_
#define _LIB_COMMON_GEO_SHAPE_COMPRESSOR_H_
#include "IGeoShape.h"
#include "PodVector.h"
#include "GeoShape.h"
#include "stream.h"
namespace CommonLib
{

	class CGeoShape;

	

	class ShapeCompressor
	{

	
		public:
			ShapeCompressor(CommonLib::alloc_t *pAlloc);
			~ShapeCompressor();


			bool compress(const CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IWriteStream *pStream);
			bool decompress(CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IReadStream *pStream);
		private:
			void compressPart(eDataType nPartType, const CGeoShape *pShp, CommonLib::IWriteStream *pStream);


			template<class TPartCompress>
			void CompressPart(TPartCompress& partCompress, uint32 *pParts, uint32 nCount, CommonLib::IWriteStream *pStream)
			{
				partCompress.compress(pParts, nCount,  pStream);
			}

			void CompressXY(const CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IWriteStream *pStream);

		private:
			CommonLib::alloc_t *m_pAlloc;
	};

}

#endif