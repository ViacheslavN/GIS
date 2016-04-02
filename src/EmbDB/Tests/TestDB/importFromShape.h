#pragma once

#include "ShapeLib/shapefil.h"


void ImportShapeFile(const wchar_t* pszDBName, const wchar_t* pszShapeFileName);
void testDBFromShape();

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
static CommonLib::eShapeType SHPTypeToGeometryType(int shpType, bool* hasZ, bool* hasM)
{
	CommonLib::eShapeType geomType = CommonLib::shape_type_null;

	switch((size_t)shpType)
	{
	case SHPT_NULL:
		geomType = CommonLib::shape_type_null;
		break;
	case SHPT_POINT:
	case SHPT_POINTZ:
	case SHPT_POINTM:
		geomType = CommonLib::shape_type_point;
		break;
	case SHPT_ARC:
	case SHPT_ARCZ:
	case SHPT_ARCM:
		geomType = CommonLib::shape_type_polyline;
		break;
	case SHPT_POLYGON:
	case SHPT_POLYGONZ:
	case SHPT_POLYGONM:
		geomType = CommonLib::shape_type_polygon;
		break;
	case SHPT_MULTIPOINT:
	case SHPT_MULTIPOINTZ:
	case SHPT_MULTIPOINTM:
		geomType = CommonLib::shape_type_multipoint;
		break;
	case SHPT_MULTIPATCH:
		geomType = CommonLib::shape_type_multipatch;
		break;
	}

	if(hasZ)
	{ 
		if(shpType == SHPT_POINTZ || shpType == SHPT_ARCZ || 
			shpType == SHPT_POLYGONZ || shpType == SHPT_MULTIPOINTZ || 
			shpType == SHPT_MULTIPOINTZ)
			*hasZ = true;
		else
			*hasZ = false;
	}

	if(hasM)
	{ 
		if(shpType == SHPT_POINTM || shpType == SHPT_ARCM || 
			shpType == SHPT_POLYGONM || shpType == SHPT_MULTIPOINTM)
			*hasM = true;
		else
			*hasM = false;
	}

	return geomType;
}

void SHPObjectToGeometry(ShapeLib::SHPObject* obj, CommonLib::CGeoShape& result);