#include "stdafx.h"
#include "../../EmbDB/ZLibCompressor.h"




void ZlibTest()
{

	CommonLib::CWriteMemoryStream stream, stream1, stream2;

	CommonLib::CString str;

	for (int i =0; i < 8000; ++i)
	{
		str.format(L"adrsdfdggdgasd%d", i);

		int nUtf8 = str.calcUTF8Length();
		std::vector<char> vecBuf(nUtf8 + 1);

		str.exportToUTF8(&vecBuf[0], vecBuf.size());
		stream.write((byte*)&vecBuf[0], vecBuf.size());
	}
	embDB::CZlibCompressor compressor;

	compressor.compress(stream.buffer(), stream.pos(), &stream1);


	uint32 nSize = stream.pos();
	uint32 nCompSize = stream1.pos();

	compressor.decompress(stream1.buffer(), stream1.pos(), &stream2);

	int ndd =0;
	ndd++;
}