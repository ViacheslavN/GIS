#include "stdafx.h"
#include "exportFromShape.h"
#include "CommonLibrary/File.h"
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
		if(file)
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
		if(file)
			ShapeLib::DBFClose(file);
		file = NULL;
	}
};


void CreateDBFromShape(const wchar_t* pszShapeFileName)
{
	SHPGuard shp;
	DBFGuard dbf;

	CommonLib::CString sFilePath = CommonLib::FileSystem::FindFilePath(pszShapeFileName);
	CommonLib::CString sFileName = CommonLib::FileSystem::FindOnlyFileName(pszShapeFileName);
	CommonLib::CString shpFilePath = sFilePath + sFileName + L".shp";
	CommonLib::CString dbfFilePath = sFilePath + sFileName + L".dbf";
	CommonLib::CString prjFileName = sFilePath + sFileName + L".prj";
	CommonLib::CString EmbDBName =  sFilePath + sFileName + L".embDB";
	

	shp.file = ShapeLib::SHPOpen(shpFilePath.cstr(), "rb");
	if(!shp.file)
		return;
	dbf.file = ShapeLib::DBFOpen(dbfFilePath.cstr(), "rb");
	if(!dbf.file)
		return; 

	int objectCount;
	int shapeType;
	double minBounds[4];
	double maxBounds[4];
	ShapeLib::SHPGetInfo(shp.file, &objectCount, &shapeType, &minBounds[0], &maxBounds[0]);
	CommonLib::bbox bounds;

	if(objectCount > 0)
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

	int fieldCount = ShapeLib::DBFGetFieldCount(dbf.file);
	for(int fieldNum = 0; fieldNum < fieldCount; ++fieldNum)
	{
		char name[33];
		int width;
		int dec;
		ShapeLib::DBFFieldType shpFieldType = ShapeLib::DBFGetFieldInfo(dbf.file, fieldNum, name, &width, &dec);

	}


}