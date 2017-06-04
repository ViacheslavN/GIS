#ifndef  _EMBEDDED_DATABASE_Z_LIB_COMPRESSOR_H_
#define  _EMBEDDED_DATABASE_Z_LIB_COMPRESSOR_H_

#include "CommonLibrary/MemoryStream.h"
#include "../../../DB/Fields/StringField/StringVal.h"
#include "zlib/zlib.h"
namespace embDB
{
	class CZlibCompressor
	{
		public:
			CZlibCompressor();
			~CZlibCompressor();


			uint32 compress(const byte* pBuf, uint32 nSize, CommonLib::CWriteMemoryStream *pDst);
			uint32 compress(const sFixedStringVal* pStrings, uint32 nCount, CommonLib::CWriteMemoryStream *pDst);


			bool decompress(const byte* pBuf, uint32 nSize, CommonLib::CWriteMemoryStream *pDst);


			bool BeginDecode(const byte* pBuf, uint32 nSize);
			bool decompressSymbol(const byte* pBuf, uint32 nSize);
			void EndDecode();
	private:
		const static size_t BUFSIZE = 128 * 1024;
		byte temp_buffer[BUFSIZE];
		z_stream m_strm;
	};
}
// deflateInit(&strm, Z_BEST_COMPRESSION);


#endif