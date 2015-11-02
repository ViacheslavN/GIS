#include "stdafx.h"
#include "importFromShape.h"
#include "CommonLibrary/File.h"
#include "CommonLibrary/BoundaryBox.h"
#include "../GisEngine/GisGeometry/SpatialReferenceProj4.h"
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
CommonLib::eShapeType SHPTypeToGeometryType(int shpType, bool* hasZ, bool* hasM)
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


        
	         
	         
	           
 



	        

	        
	          
	         
	         
	 
	   
	   
	        
	    
	
	    
          
	          
	  
	    
	    
	         
	     
	 
	    
embDB::eSpatialCoordinatesUnits GetGeometryUnits(GisEngine::GisCommon::Units units)
{
	switch(units)
	{
		case GisEngine::GisCommon::UnitsUnknown:
			return embDB::scuUnknown;
			break;
		case GisEngine::GisCommon::UnitsInches:
			return embDB::scuInches;
			break;
		case GisEngine::GisCommon::UnitsPoints:
			return embDB::scuPoints;
			break;
		case GisEngine::GisCommon::UnitsFeet:
			return embDB::scuFeet;
			break;
		case GisEngine::GisCommon::UnitsYards:
			return embDB::scuYards;
			break;
		case GisEngine::GisCommon::UnitsMiles:
			return embDB::scuMiles;
			break;
		case GisEngine::GisCommon::UnitsNauticalMiles:
			return embDB::scuNauticalMiles;
			break;
		case GisEngine::GisCommon::UnitsMillimeters:
			return embDB::scuMillimeters;
			break;
		case GisEngine::GisCommon::UnitsCentimeters:
			return embDB::scuCentimeters;
			break;
		case GisEngine::GisCommon::UnitsMeters:
			return embDB::scuMeters;
			break;
		case GisEngine::GisCommon::UnitsKilometers:
			return embDB::scuKilometers;
			break;
		case GisEngine::GisCommon::UnitsDecimalDegrees:
			return embDB::scuDecimalDegrees;
			break;
		case GisEngine::GisCommon::UnitsDecimeters:
			return embDB::scuDecimeters;
			break;
		}

	return embDB::scuUnknown;
}


void ImportShapeFile(const wchar_t* pszDBName, const wchar_t* pszShapeFileName)
{
	
	SHPGuard shp;
	DBFGuard dbf;

	CommonLib::CString sFilePath = CommonLib::FileSystem::FindFilePath(pszShapeFileName);
	CommonLib::CString sFileName = CommonLib::FileSystem::FindOnlyFileName(pszShapeFileName);
	CommonLib::CString shpFilePath = sFilePath + sFileName + L".shp";
	CommonLib::CString dbfFilePath = sFilePath + sFileName + L".dbf";
	CommonLib::CString prjFileName = sFilePath + sFileName + L".prj";


	CommonLib::CString sDBPath = CommonLib::FileSystem::FindFilePath(pszDBName);
 
	embDB::CDatabase db;
	if(!db.open(pszDBName, embDB::eTMSingleTransactions, sDBPath.wstr()))
	{
		if(!db.create(pszDBName, 8192, embDB::eTMSingleTransactions,  sDBPath.wstr()))
		{
			std::cout << "Error create db";
			return;
		}
	}


	embDB::CSchema* pSchema = db.getSchema();
	embDB::IDBTable *pTable = pSchema->getTable(sFileName);
	if(pTable)
		return;
	
	pSchema->addTable(sFileName, L"");
	pTable = pSchema->getTable(sFileName);

	if(!pTable)
		return;

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
 
	GisEngine::GisCommon::Units units;
	GisEngine::GisGeometry::CSpatialReferenceProj4* pSpatialReference = new GisEngine::GisGeometry::CSpatialReferenceProj4(prjFileName, GisEngine::GisGeometry::eSPRefTypePRJFilePath);
	if(!pSpatialReference->IsValid())
	{
		pSpatialReference = new GisEngine::GisGeometry::CSpatialReferenceProj4(bounds);
	}
	if(pSpatialReference->IsValid())
	{
		units = pSpatialReference->GetUnits();
	}

	pTable->createShapeField(sFileName.wstr(), L"", SHPTypeToGeometryType(shapeType, NULL, NULL), bounds, GetGeometryUnits(units), true );

	int fieldCount = ShapeLib::DBFGetFieldCount(dbf.file);
	for(int fieldNum = 0; fieldNum < fieldCount; ++fieldNum)
	{
		char name[33];
		int width;
		int dec;
		ShapeLib::DBFFieldType shpFieldType = ShapeLib::DBFGetFieldInfo(dbf.file, fieldNum, name, &width, &dec);


		embDB::SFieldProp fp;
	
		fp.dateTypeExt = embDB::dteSimple;
		fp.sFieldName = name;

		switch(shpFieldType)
		{
		case ShapeLib::FTString:
			fp.dataType =  embDB::dtString;
			fp.nLenField = width;
			break;
		case ShapeLib::FTInteger:
			fp.dataType =  embDB::dtInteger32;
			break;
		case ShapeLib::FTDouble:
			fp.dataType =  embDB::dtDouble;
			break;			 
		case ShapeLib::FTDate:
			break;
		}

		pTable->createField(fp);
	}


}


void testDBFromShape()
{

}