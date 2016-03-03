#include "stdafx.h"
#include "ShapeCompressor.h"


namespace CommonLib
{
	ShapeCompressor::ShapeCompressor(CommonLib::alloc_t *pAlloc)
	{
	}
	ShapeCompressor::~ShapeCompressor()
	{

	}

	/*
	-----------------
	shape type 1 byte
	-----------------
	compress flags
	3 bit type coord
	3 bit type parts
	1 bit type pats 1/0
	-----------------
	type part count parts
	-----------------
	parts
	----------------
	type parts
	count
	---
	---
	----------------
	compress numlen points
	----------------
	not compressed
	Z
	M

	*/




	
	*/
	bool ShapeCompressor::compress(CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IWriteStream *pStream)
	{
		CGeoShape::compress_params params;




		return true;
	}
	bool ShapeCompressor::decompress(CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IReadStream *pStream)
	{
		return true;
	}
}