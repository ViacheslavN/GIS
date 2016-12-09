#ifndef _LIB_COMMON_GEO_SHAPE_XY_I_COMPRESSOR_H_
#define _LIB_COMMON_GEO_SHAPE_XY_I_COMPRESSOR_H_
#include "NumLenCompressor.h"
#include "BitStream.h"
#include "GeoShape.h"
#include "PointZOrder.h"
#include <set>
#include "ByteCompressor.h"
#include "SignCompressor.h"

namespace CommonLib
{


	class IXYComressor
	{
	public:
		virtual ~IXYComressor() {}
		IXYComressor(){}

		virtual void PreCompress(const GisXYPoint *pPoint, uint32 nCount) = 0;
		virtual uint32 GetCompressSize() const= 0;
		virtual uint32 GetCount() const = 0;
		virtual uint32 GetPointCount() const = 0;
		virtual void WriteHeader(IWriteStream *pStream) = 0;
		virtual bool  compress(const GisXYPoint *pPoint, uint32 nCount, IWriteStream *pStream) = 0;

		virtual void ReadHeader(IReadStream *pStream) = 0;
		virtual bool  decompress(GisXYPoint *pPoint, uint32 nCount, IReadStream *pStream) = 0;

		virtual eCompressDataType GetCompressDataType() const = 0;
		virtual void clear(const CGeoShape::compress_params* pParams = NULL) = 0;
	};

	typedef TNumLemCompressor<uint16, TFindMostSigBit, 16> TPointNumLen16;
	typedef TNumLemCompressor<uint32, TFindMostSigBit, 32> TPointNumLen32;
	typedef TNumLemCompressor<uint64, TFindMostSigBit, 64> TPointNumLen64;
}

#endif