#include "stdafx.h"
#include "..\..\CommonLibrary\GeoShape.h"
#include "..\..\CommonLibrary\PartCompressor2.h"
#include "..\..\CommonLibrary\MemoryStream.h"

#include "..\..\CommonLibrary\PartEncoder.h"


void TestPart(CommonLib::Private::CPartEncoder& partCmp, CommonLib::IReadStream *pSteram)
{
	partCmp.clear();
	partCmp.InitDecode(pSteram);
	uint32 nCnt = 0;
	uint32 nPart = 0;
	{
		nCnt = partCmp.getPartCnt();
		for (int i = 0; i < nCnt; ++i)
		{
			nPart = partCmp.GetNextPart(i);
		}

		partCmp.Reset();

		nCnt = partCmp.getPartCnt();
		for (int i = 0; i < nCnt; ++i)
		{
			nPart = partCmp.GetNextPart(i);
		}
	}
}



void TestEncodePart()
{
	CommonLib::Private::CPartEncoder partCmp;


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
	for (uint32 i = 0, sz = noCompressPartShape.getPartCount(); i < sz; ++i)
	{
		noCompressPartShape.getParts()[i] = i;
	}

	CompressPartShape.create(CommonLib::shape_type_polygon, 10000, 100);

	for (uint32 i = 0, sz = CompressPartShape.getPartCount(); i < sz; ++i)
	{
		CompressPartShape.getParts()[i] = i * 10 + i;
	}

	writeStreamCache.resize(100000);
	partCmp.Encode(nullPartShape.getParts(), nullPartShape.getPartCount(), &writeStreamCache);
	writeStream.write(writeStreamCache.buffer(), writeStreamCache.pos());

	writeStreamCache.seek(0, CommonLib::soFromBegin);
	partCmp.clear();
	partCmp.Encode(noCompressPartShape.getParts(), noCompressPartShape.getPartCount(), &writeStreamCache);
	writeStream.write(writeStreamCache.buffer(), writeStreamCache.pos());

	writeStreamCache.seek(0, CommonLib::soFromBegin);
	partCmp.clear();
	partCmp.Encode(CompressPartShape.getParts(), CompressPartShape.getPartCount(), &writeStreamCache);
	writeStream.write(writeStreamCache.buffer(), writeStreamCache.pos());


	readStream.attachBuffer(writeStream.buffer(), writeStream.pos());


	TestPart(partCmp, &readStream);
	TestPart(partCmp, &readStream);
	TestPart(partCmp, &readStream);

	int dd = 0;
	dd++;
}