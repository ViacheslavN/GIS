#ifndef _LIB_COMMON_GEO_SHAPE_COMPRESSOR_H_
#define _LIB_COMMON_GEO_SHAPE_COMPRESSOR_H_
#include "IGeoShape.h"
#include "PodVector.h"
#include "GeoShape.h"
#include "stream.h"
#include "MemoryStream.h"
namespace CommonLib
{

	class CGeoShape;
	class IXYComressor;
	class IPartComressor;

	
	

	class ShapeCompressor
	{

	
		public:
			ShapeCompressor(CommonLib::alloc_t *pAlloc);
			~ShapeCompressor();


			bool compress(const CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IWriteStream *pStream, CWriteMemoryStream *pCacheStream = 0);
			bool decompress(CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IReadStream *pStream);
		private:
			void compressPart(eCompressDataType nPartType, const CGeoShape *pShp, CommonLib::IWriteStream *pStream);


			template<class TPartCompress>
			void CompressPart(TPartCompress& partCompress, uint32 *pParts, uint32 nCount, CommonLib::IWriteStream *pStream)
			{
				partCompress.compress(pParts, nCount,  pStream);
			}

			void CreateCompressXY( CGeoShape::compress_params *pParams);
			void CreatePartCompressor(eCompressDataType nPartType);


			uint32 CalcCompressSize(const CGeoShape *pShp, CGeoShape::compress_params *pParams);
		private:
			CommonLib::alloc_t *m_pAlloc;
			std::auto_ptr<IXYComressor> m_xyCompressor;
			std::auto_ptr<IPartComressor> m_PartCompressor;

			bool m_bWriteParams;
			bool m_bNullPart;
			bool m_bCompressPart;
			bool m_bCompressPoint;
			eCompressDataType m_partType;
			CGeoShape::compress_params m_CompressParams;
	};

}

#endif