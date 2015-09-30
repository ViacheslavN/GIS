#include "stdafx.h"
#include "../../EmbDB/DatasetLite/SpatialDataset.h"


void TestShapeIndex()
{

	/*GisEngine::GeoDatabase::IWorkspacePtr pShapeWks  = GisEngine::GeoDatabase::CShapefileWorkspace::Open(L"ShapeTest", L"D:\\test\\GIS\\GIS\\src\\GisEngine\\Tests\\TestData");
	//GisEngine::GeoDatabase::IWorkspacePtr pShapeWks  = GisEngine::GeoDatabase::CShapefileWorkspace::Open(L"ShapeTest", L"d:\\work\\MyProject\\GIS\\src\\GisEngine\\Tests\\TestData");

	GisEngine::GeoDatabase::IFeatureClassPtr pShapeFC = pShapeWks->GetFeatureClass(L"world_adm0.shp");*/
	{
	//	DatasetLite::IShapeFileIndexPtr pShapeIndex = DatasetLite::IShapeFileIndex::create(L"D:\\test\\GIS\\GIS\\src\\GisEngine\\Tests\\TestData\\world_adm0.shapeidx", 8192, L"D:\\test\\GIS\\GIS\\src\\GisEngine\\Tests\\TestData\\world_adm0.shp" );
	}

	{
		DatasetLite::IShapeFileIndexPtr pShapeIndex = DatasetLite::IShapeFileIndex::open(L"D:\\test\\GIS\\GIS\\src\\GisEngine\\Tests\\TestData\\world_adm0.shapeidx");
	
		CommonLib::bbox bbox;
		bbox.type = CommonLib::bbox_type_normal;
		/*bbox.xMin = 10;
		bbox.xMax = 20;
		bbox.yMin = 10;
		bbox.yMax = 20;*/
		
		//bbox.xMin = bbox.xMax = 55.749250903707818;
		//bbox.yMin = bbox.yMax = 29.934119151360076;

		bbox.xMin = 54.572874219027376;
		bbox.xMax = 54.846686887944934;
		bbox.yMin = 32.753136385462348;
		bbox.yMax = 32.918637080915680;

		DatasetLite::IShapeCursorPtr pCursor = pShapeIndex->spatialQuery(bbox);
		int nId = 0;
		int nCnt = 0;
		CommonLib::bbox Shapebbox;
		while(!pCursor->IsEnd())
		{
			nId = pCursor->row();
		//	Shapebbox = pCursor->extent();
			pCursor->next();
			nCnt++;
		}
		
	}
	
}