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
void SHPObjectToGeometry(ShapeLib::SHPObject* obj, CommonLib::CGeoShape& result)
{
	result.create(SHPTypeToGeometryType(obj->nSHPType, NULL, NULL), obj->nVertices, obj->nParts);

	GisXYPoint* pPt = result.getPoints();
	double* zs = result.getZs();
	double* ms = result.getMs();
	int i;
	for(i = 0; i < obj->nVertices; ++i, ++pPt, zs += zs ? 1 : 0, ms += ms ? 1 : 0)
	{
		pPt->x = obj->padfX[i];
		pPt->y = obj->padfY[i];
		if(obj->padfZ && zs)
			*zs = obj->padfZ[i];
		if(obj->padfM && ms)
			*ms = obj->padfM[i];
	}

	uint32* parts = result.getParts();
	CommonLib::patch_type* partTypes = result.getPartsTypes();
	if ( obj->nParts != 0 )
	{
		for(i = 0; i < obj->nParts; ++i, ++parts, partTypes += partTypes ? 1 : 0)
		{
			*parts = (long)obj->panPartStart[i];
			if(obj->panPartType && partTypes)
			{
				switch(obj->panPartType[i])
				{
				case SHPP_TRISTRIP:
					*partTypes = CommonLib::patch_type_triangle_strip;
					break;
				case SHPP_TRIFAN:
					*partTypes = CommonLib::patch_type_triangle_fan;
					break;
				case SHPP_OUTERRING:
					*partTypes = CommonLib::patch_type_outer_ring;
					break;
				case SHPP_INNERRING:
					*partTypes = CommonLib::patch_type_inner_ring;
					break;
				case SHPP_FIRSTRING:
					*partTypes = CommonLib::patch_type_first_ring;
					break;
				case SHPP_RING:
					*partTypes = CommonLib::patch_type_ring;
					break;
				}
			}
		}
	}

	result.calcBB();
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
		if(!db.create(pszDBName, embDB::eTMSingleTransactions,  sDBPath.wstr()))
		{
			std::cout << "Error create db";
			return;
		}
	}
	else
		return;

	embDB::ISchemaPtr pSchema = db.getSchema();
	embDB::ITablePtr pTable = pSchema->getTableByName(sFileName.cwstr());
	if(pTable.get())
		return;
	
	pSchema->addTable(sFileName.wstr());
	pTable = pSchema->getTableByName(sFileName.wstr());

	if(!pTable.get())
		return;

	embDB::IDBTable *pDBTable = dynamic_cast<embDB::IDBTable*>(pTable.get());

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

	
	int fieldCount = ShapeLib::DBFGetFieldCount(dbf.file);
	int nCreateField = 0;
	for(int fieldNum = 0; fieldNum < fieldCount; ++fieldNum)
	{
		char name[33];
		int width;
		int dec;
		ShapeLib::DBFFieldType shpFieldType = ShapeLib::DBFGetFieldInfo(dbf.file, fieldNum, name, &width, &dec);


		embDB::SFieldProp fp;
	
		fp.m_bNotNull = false;
		fp.m_sFieldName = name;

		switch(shpFieldType)
		{
		case ShapeLib::FTString:
			fp.m_dataType =  embDB::dtString;
			fp.m_nLenField = width;
			break;
		case ShapeLib::FTInteger:
			fp.m_dataType =  embDB::dtInteger32;
			break;
		case ShapeLib::FTDouble:
			fp.m_dataType =  embDB::dtDouble;
			break;			 
		case ShapeLib::FTDate:
			break;
		}
		pDBTable->createField(fp);
		
	}

	pDBTable->createShapeField(sFileName.wstr(), L"", SHPTypeToGeometryType(shapeType, NULL, NULL), bounds, GetGeometryUnits(units), true, 65536 );


	embDB::ITransactionPtr pTran = db.startTransaction(embDB::eTT_MODIFY);
	pTran->begin();
	embDB::IInsertCursorPtr pInsertCursor = pTran->createInsertCursor(pDBTable->getName().cwstr());
	embDB::IRowPtr pRow = pInsertCursor->createRow();
	embDB::IFieldsPtr pFields = pDBTable->getFields();

	ShapeLib::SHPObject*   pCacheObject = NULL;


	CommonLib::CString strVal;
	double dblVal;
	int intVal;
	CommonLib::CGeoShape shape;
	shape.AddRef();


	uint32 nShapeRowSize = 0;
	uint32 nStringRowSize = 0;
	uint32 nDigSize = 0;
	uint32 nDblSize = 0;
	for(size_t row = 0; row < objectCount; ++row)
	{
		for (size_t i = 0; i < pFields->GetFieldCount(); ++i)
		{
			embDB::IFieldPtr pField = pFields->GetField(i);
			CommonLib::CVariant value;
			
			switch(pField->getType())
			{
			case embDB::dtString:
				strVal = ShapeLib::DBFReadStringAttribute(dbf.file, row, i);
				value  = strVal;
				nStringRowSize += strVal.calcUTF8Length();
				break;
			case embDB::dtUInteger32:
				intVal = ShapeLib::DBFReadIntegerAttribute(dbf.file, row, i);
				value  = intVal;
				nDigSize +=4;
				break;
			case embDB::dtDouble:
				dblVal = ShapeLib::DBFReadDoubleAttribute(dbf.file, row, i);
				value  = dblVal;
				nDblSize +=8;
				break;
			case embDB::dtGeometry:
						{
							pCacheObject = ShapeLib::SHPReadObject(shp.file, row);
							SHPObjectToGeometry(pCacheObject, shape);


							//CommonLib::MemoryStream steram;
							// shape.write(&steram);
							//nShapeRowSize += steram.pos();
							if(pCacheObject)
							{
								ShapeLib::SHPDestroyObject(pCacheObject);
								pCacheObject = 0;
							}
							value  = CommonLib::IGeoShapePtr(&shape);
						}
				break;
			}

			pRow->set(value, i);

		}

		pInsertCursor->insert(pRow.get());
	}

	pTran->commit();
}


void SearchShapeFile(const wchar_t* pszDBName)
{
	embDB::CDatabase db;
	CommonLib::CString sFilePath = CommonLib::FileSystem::FindFilePath(pszDBName);
	if(!db.open(pszDBName, embDB::eTMSingleTransactions, sFilePath.wstr()))
	{
		return;
	}
	embDB::ITransactionPtr pTran = db.startTransaction(embDB::eTT_SELECT);
	pTran->begin();
	CommonLib::bbox bbox;
	bbox.type =CommonLib::bbox_type_normal;
	bbox.xMin = 597461.43000000005;
	bbox.yMin =	6072242.5499999998;
	bbox.xMax =  652299.90000000002;
	bbox.yMax = 6123549.2000000002;
	embDB::ICursorPtr pCursor = pTran->executeSpatialQuery(bbox, L"building", L"building");
	int nObj = 0;
	embDB::IRowPtr pRow;
	if(pCursor.get())
	{
	
		
		while(pCursor->NextRow(&pRow))
		{
			++nObj;
		}
	}

	int  i = 0;
	i++;

	pCursor = pTran->executeSelectQuery( L"building");
	int nObj2 = 0;
	while(pCursor->NextRow(&pRow))
	{
		++nObj2;
	}

	i = 0;
	i++;
	 
}

void testDBFromShape()
{
	ImportShapeFile(L"d:\\db\\importShapeFile.embDB", L"d:\\work\\MyProject\\GIS\\src\\GisEngine\\Tests\\TestData\\building.shp");
	//ImportShapeFile(L"d:\\db\\importShapeFile.embDB", L"d:\\test\\GIS\\GIS\\src\\GisEngine\\Tests\\TestData\\building.shp");
	SearchShapeFile(L"d:\\db\\importShapeFile.embDB");
}