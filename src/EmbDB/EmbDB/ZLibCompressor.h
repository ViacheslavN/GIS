#ifndef  _EMBEDDED_DATABASE_Z_LIB_COMPRESSOR_H_
#define  _EMBEDDED_DATABASE_Z_LIB_COMPRESSOR_H_

#include "CommonLibrary/MemoryStream.h"

namespace embDB
{
	class CZlibCompressor
	{
		public:
			CZlibCompressor();
			~CZlibCompressor();


			uint32 compress(const byte* pBuf, uint32 nSize, CommonLib::CWriteMemoryStream *pDst);
			bool decompress(const byte* pBuf, uint32 nSize, CommonLib::CWriteMemoryStream *pDst);
	private:
	};
}
// deflateInit(&strm, Z_BEST_COMPRESSION);


#endif