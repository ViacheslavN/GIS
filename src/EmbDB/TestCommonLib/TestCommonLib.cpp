// TestCommonLib.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "CommonLibrary/str_t.h"
#include "CommonLibrary/MemoryStream.h"
int _tmain(int argc, _TCHAR* argv[])
{
	CommonLib::str_t str;
	CommonLib::alloc_t *alloc = new CommonLib::simple_alloc_t();
	CommonLib::MemoryStream stream(alloc);
	str.format("dgf %s", "dd");
	str.wstr();

	int i1 = 666;
	float f1 = 237.4556;
	double d1 = 237.4556;
	bool b1 = true;
	stream.write(i1);
	stream.write(f1);
	stream.write(d1);
	stream.write(b1);

	stream.reset();

	int i2;
	float f2;
	double d2;
	bool b2;

	stream.read(i2);
	stream.read(f2);
	stream.read(d2);
	stream.read(b2);

	return 0;
}

