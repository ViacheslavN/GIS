#include "stdafx.h"
#include "ZLibCompressor.h"

namespace embDB
{

	CZlibCompressor::CZlibCompressor()
	{

	}
	CZlibCompressor::~CZlibCompressor()
	{

	}
	void CZlibCompressor::BeginEncode(CommonLib::CWriteMemoryStream *pDst)
	{ 
		m_encode_strm.zalloc = 0;
		m_encode_strm.zfree = 0;
		m_encode_strm.next_out = temp_buffer;
		m_encode_strm.avail_out = BUFSIZE;

		deflateInit(&m_encode_strm, Z_BEST_COMPRESSION);
	}
	void CZlibCompressor::EndEncode(CommonLib::CWriteMemoryStream *pDst)
	{
		int deflate_res = Z_OK;
		while (deflate_res == Z_OK)
		{
			if (m_encode_strm.avail_out == 0)
			{
				pDst->write(temp_buffer, BUFSIZE);
				m_encode_strm.next_out = temp_buffer;
				m_encode_strm.avail_out = BUFSIZE;
			}
			deflate_res = deflate(&m_encode_strm, Z_FINISH);
		}

		assert(deflate_res == Z_STREAM_END);
		pDst->write(temp_buffer, BUFSIZE - m_encode_strm.avail_out);
		deflateEnd(&m_encode_strm);
	}

	uint32 CZlibCompressor::compress(const byte* pBuf, uint32 nSize, CommonLib::CWriteMemoryStream *pDst)
	{
		m_encode_strm.next_in = (Bytef*)pBuf;
		m_encode_strm.avail_in = nSize;

		while (m_encode_strm.avail_in != 0)
		{
			int res = deflate(&m_encode_strm, Z_NO_FLUSH);
			assert(res == Z_OK);
			if (m_encode_strm.avail_out == 0)
			{
				pDst->write(temp_buffer,  BUFSIZE); 
				m_encode_strm.next_out = temp_buffer;
				m_encode_strm.avail_out = BUFSIZE;
			}
		}
		return 0;
	}

	uint32 CZlibCompressor::compress(const sFixedStringVal* pStrings, uint32 nCount, CommonLib::CWriteMemoryStream *pDst)
	{
		z_stream strm;
		strm.zalloc = 0;
		strm.zfree = 0;
		strm.next_out = temp_buffer;
		strm.avail_out = BUFSIZE;

		deflateInit(&strm, Z_BEST_COMPRESSION);

		for (uint32 i = 0; i < nCount; ++i)
		{

			strm.next_in = (Bytef*)pStrings[i].m_pBuf;
			strm.avail_in = pStrings[i].m_nLen - 1;

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

	bool CZlibCompressor::BeginDecode(const byte* pBuf, uint32 nSize)
	{
	
		m_strm.zalloc = 0;
		m_strm.zfree = 0;
		m_strm.next_in = (Bytef*)pBuf;
		m_strm.avail_in = nSize;
		inflateInit(&m_strm);

		return true;
	}
	bool CZlibCompressor::decompressSymbol(const byte* pBuf, uint32 nSize)
	{
		m_strm.next_out = (Bytef*)pBuf;
		m_strm.avail_out = nSize;

		while (m_strm.avail_in != 0)
		{
			int res = inflate(&m_strm, Z_NO_FLUSH);
			assert(res != Z_STREAM_ERROR);
			
				 return m_strm.avail_in != 0;
			
		}
		return false;
	}
	void CZlibCompressor::EndDecode()
	{
		inflateEnd(&m_strm);
	}
}