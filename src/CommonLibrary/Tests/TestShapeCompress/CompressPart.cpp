#include "stdafx.h"
#include "..\..\CommonLibrary\GeoShape.h"
#include "..\..\CommonLibrary\PartCompressor2.h"
#include "..\..\CommonLibrary\MemoryStream.h"


void TestCompressPart()
{
	CommonLib::CPartCompressor partCmp;
	CommonLib::CReadMemoryStream readStream;
	CommonLib::CWriteMemoryStream writeStream;
	CommonLib::CWriteMemoryStream writeStreamCache;



	CommonLib::CGeoShape nullPartShape;
	CommonLib::CGeoShape noCompressPartShape;
	CommonLib::CGeoShape CompressPartShape;


	CommonLib::CGeoShape nullPartShapeDecom;
	CommonLib::CGeoShape noCompressPartShapeDecom;
	CommonLib::CGeoShape CompressPartShapeDecom;



	nullPartShape.create(CommonLib::shape_type_polygon, 1, 1);
	nullPartShape.getParts()[0] = 100;

	noCompressPartShape.create(CommonLib::shape_type_polygon, 1, 9);
	for (uint32 i =0, sz = noCompressPartShape.getPartCount();i < sz ; ++i)
	{
		noCompressPartShape.getParts()[i] = i;
	}

	CompressPartShape.create(CommonLib::shape_type_polygon, 10000, 100);
	 
	for (uint32 i =0, sz = CompressPartShape.getPartCount(); i < sz ;++i)
	{
		CompressPartShape.getParts()[i] = i * 10 + i;
	}

	writeStreamCache.resize(100000);
	partCmp.compress(&nullPartShape, &writeStreamCache);
	writeStream.write(writeStreamCache.buffer(), writeStreamCache.pos());

	writeStreamCache.seek(0, CommonLib::soFromBegin);
	partCmp.clear();
	partCmp.compress(&noCompressPartShape, &writeStreamCache);
	writeStream.write(writeStreamCache.buffer(), writeStreamCache.pos());

	writeStreamCache.seek(0, CommonLib::soFromBegin);
	partCmp.clear();
	partCmp.compress(&CompressPartShape, &writeStreamCache);
	writeStream.write(writeStreamCache.buffer(), writeStreamCache.pos());


	readStream.attachBuffer(writeStream.buffer(), writeStream.pos());

	partCmp.decompress(&nullPartShapeDecom, &readStream);
	partCmp.decompress(&noCompressPartShapeDecom, &readStream);
	partCmp.decompress(&CompressPartShapeDecom, &readStream);

	int dd = 0;
	dd++;
}