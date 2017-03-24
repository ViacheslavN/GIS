#include "stdafx.h"
#include "CommonLibrary/BoundaryBox.h"
struct SHPGuard
{
	ShapeLib::SHPHandle file;
	SHPGuard()
		: file(NULL)
	{}
	~SHPGuard()
	{
		clear();
	}
	void clear()
	{
		if (file)
			ShapeLib::SHPClose(file);
		file = NULL;
	}
};

struct DBFGuard
{
	ShapeLib::DBFHandle file;
	DBFGuard()
		: file(NULL)
	{}
	~DBFGuard()
	{
		clear();
	}
	void clear()
	{
		if (file)
			ShapeLib::DBFClose(file);
		file = NULL;
	}
};




uint32 getCoordScale(double dVal, uint32 nMaxScale = 15)
{
	uint64 nPow = 1;
	dVal = fabs(dVal);


	for (uint32 i = 0; i < nMaxScale; ++i)
	{
		uint64 nVal = uint64(dVal * nPow);
		double dBack = (double)nVal / nPow;
		if (fabs(dVal - dBack) == 0.)
			return i;
		nPow *= 10;
	}
	return nMaxScale;
}



void GetScale(const char *pszShpFilePath, uint32& nScaleX, uint32& nScaleY)
{

	int objectCount;
	int shapeType;
	double minBounds[4];
	double maxBounds[4];
	CommonLib::bbox bounds;


	SHPGuard shp;
	DBFGuard dbf;
	shp.file = ShapeLib::SHPOpen(pszShpFilePath, "rb");
	if (!shp.file)
		return;
	dbf.file = ShapeLib::DBFOpen(pszShpFilePath, "rb");
	if (!dbf.file)
		return;





	ShapeLib::SHPGetInfo(shp.file, &objectCount, &shapeType, &minBounds[0], &maxBounds[0]);


	if (objectCount > 0)
	{
		bounds.type = CommonLib::bbox_type_normal;
		bounds.xMin = minBounds[0];
		bounds.xMax = maxBounds[0];
		bounds.yMin = minBounds[1];
		bounds.yMax = maxBounds[1];
		bounds.zMin = minBounds[2];
		bounds.zMax = maxBounds[2];
		bounds.mMin = minBounds[3];
		bounds.mMax = maxBounds[3];
	}
	ShapeLib::SHPObject*   pCacheObject = NULL;

	uint32 nMaxXScale = 0;
	uint32 nMaxYScale = 0;

	for (size_t row = 0; row < objectCount; ++row)
	{
		pCacheObject = ShapeLib::SHPReadObject(shp.file, row);

		for (uint32 i = 0; i < pCacheObject->nVertices; ++i)
		{
			double x = pCacheObject->padfX[i];
			double y = pCacheObject->padfY[i];

			uint32 nScaleX = getCoordScale(x);
			uint32 nScaleY = getCoordScale(y);

			if (nMaxXScale < nScaleX)
				nMaxXScale = nScaleX;

			if (nMaxYScale < nScaleY)
				nMaxYScale = nScaleY;
		}


	}
	nScaleX = nMaxXScale;
	nScaleY = nMaxYScale;
}


void TestCompressShape()
{

	uint32 nX, nY;
	uint32 nX1, nY1;
	GetScale("D:\\db\\10m_cultural\\ne_10m_roads_north_america.shp", nX1, nY1);
	GetScale("d:\\db\\building.shp", nX, nY);

}