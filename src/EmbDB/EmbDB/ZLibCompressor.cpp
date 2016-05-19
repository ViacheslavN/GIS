#include "stdafx.h"
#include "ZLibCompressor.h"
#include "zlib/zlib.h"
namespace embDB
{

	CZlibCompressor::CZlibCompressor()
	{

	}
	CZlibCompressor::~CZlibCompressor()
	{

	}


	uint32 CZlibCompressor::compress(const byte* pBuf, uint32 nSize, CommonLib::CWriteMemoryStream *pDst)
	{
		const size_t BUFSIZE = 128 * 1024;
		byte temp_buffer[BUFSIZE];

		z_stream strm;
		strm.zalloc = 0;
		strm.zfree = 0;
		strm.next_in = (Bytef*)pBuf;
		strm.avail_in = nSize;
		strm.next_out = temp_buffer;
		strm.avail_out = BUFSIZE;

		deflateInit(&strm, Z_BEST_COMPRESSION);

		while (strm.avail_in != 0)
		{
			int res = deflate(&strm, Z_NO_FLUSH);
			assert(res == Z_OK);
			if (strm.avail_out == 0)
			{
				pDst->write(temp_buffer,  BUFSIZE); 
				strm.next_out = temp_buffer;
				strm.avail_out = BUFSIZE;
			}
		}

		int deflate_res = Z_OK;
		while (deflate_res == Z_OK)
		{
			if (strm.avail_out == 0)
			{
				pDst->write(temp_buffer,  BUFSIZE); 
				strm.next_out = temp_buffer;
				strm.avail_out = BUFSIZE;
			}
			deflate_res = deflate(&strm, Z_FINISH);
		}

		assert(deflate_res == Z_STREAM_END);
		pDst->write(temp_buffer,   BUFSIZE - strm.avail_out); 
		deflateEnd(&strm);
		return 0;
	}
	bool CZlibCompressor::decompress(const byte* pBuf, uint32 nSize, CommonLib::CWriteMemoryStream *pDst)
	{
		const size_t BUFSIZE = 128 * 1024;
		byte temp_buffer[BUFSIZE];


		z_stream strm;
		strm.zalloc = 0;
		strm.zfree = 0;
		strm.next_in = (Bytef*)pBuf;
		strm.avail_in = nSize;
		strm.next_out = temp_buffer;
		strm.avail_out = BUFSIZE;


		inflateInit(&strm);


		while (strm.avail_in != 0)
		{
			int res = inflate(&strm, Z_NO_FLUSH);
			assert(res != Z_STREAM_ERROR);
			if (strm.avail_out == 0)
			{
				pDst->write(temp_buffer,  BUFSIZE); 
				strm.next_out = temp_buffer;
				strm.avail_out = BUFSIZE;
			}
		}

		int inflate_res = Z_OK;
		while (inflate_res == Z_OK)
		{
			if (strm.avail_out == 0)
			{
				pDst->write(temp_buffer,  BUFSIZE); 
				strm.next_out = temp_buffer;
				strm.avail_out = BUFSIZE;
			}
			inflate_res = inflate(&strm, Z_FINISH);
		}

		assert(inflate_res == Z_STREAM_END);
		pDst->write(temp_buffer,   BUFSIZE - strm.avail_out); 
		inflateEnd(&strm);
		return true;
	}
}