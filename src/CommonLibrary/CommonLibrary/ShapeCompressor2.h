#ifndef _LIB_COMMON_GEO_SHAPE_COMPRESSOR_2_H_
#define _LIB_COMMON_GEO_SHAPE_COMPRESSOR_2_H_
#include "IGeoShape.h"
#include "PodVector.h"
#include "GeoShape.h"
#include "stream.h"
#include "MemoryStream.h"
#include "PartCompressor2.h"
#include "PointCompressor.h"
namespace CommonLib
{

	class CGeoShape;
	class IXYComressor;
	class IPartComressor;
	class IXYComressor2;



	class ShapeCompressor2
	{
		enum eFlags
		{
			WriteCompressParams = 0x01
		};

	public:
		ShapeCompressor2(CommonLib::alloc_t *pAlloc);
		~ShapeCompressor2();

		bool compress(const CGeoShape *pShp, CGeoShape::compress_params *pParams, CWriteMemoryStream *pStream);
		bool compress(const CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IWriteStream *pStream, CWriteMemoryStream *pCacheStream = 0);
		bool decompress(CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IReadStream *pStream);
	private:


		uint32 CalcCompressSize(const CGeoShape *pShp, CGeoShape::compress_params *pParams);
	private:
		CommonLib::alloc_t *m_pAlloc;
 
		bool m_bWriteParams;
		bool m_bNullPart;
		bool m_bCompressPart;
		bool m_bCompressPoint;
		eCompressDataType m_partType;
		CGeoShape::compress_params m_CompressParams;
		CPartCompressor m_PartCompressor;
		CPointCompressor m_PointCompressor;
	};

}

#endif