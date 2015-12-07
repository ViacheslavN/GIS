// TestCompress.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CommonLibrary/MemoryStream.h"
#include "CommonLibrary/FixedBitStream.h"
#include "TestAC.h"

int _tmain(int argc, _TCHAR* argv[])
{

	ACComp comp;
	CommonLib::MemoryStream stream;
	CommonLib::MemoryStream stream1;
	comp.compress("йнб.йнпнбю", &stream);
	comp.compressInteger("йнб.йнпнбю", &stream1);
	stream.seek(0, CommonLib::soFromBegin);
	CommonLib::CString str;
	comp.decompress(&stream, str);
	return 0;
}

