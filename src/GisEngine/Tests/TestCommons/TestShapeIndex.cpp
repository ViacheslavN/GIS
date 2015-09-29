#include "stdafx.h"
#include "../../EmbDB/DatasetLite/SpatialDataset.h"


void TestShapeIndex()
{

	/*GisEngine::GeoDatabase::IWorkspacePtr pShapeWks  = GisEngine::GeoDatabase::CShapefileWorkspace::Open(L"ShapeTest", L"D:\\test\\GIS\\GIS\\src\\GisEngine\\Tests\\TestData");
	//GisEngine::GeoDatabase::IWorkspacePtr pShapeWks  = GisEngine::GeoDatabase::CShapefileWorkspace::Open(L"ShapeTest", L"d:\\work\\MyProject\\GIS\\src\\GisEngine\\Tests\\TestData");

	GisEngine::GeoDatabase::IFeatureClassPtr pShapeFC = pShapeWks->GetFeatureClass(L"world_adm0.shp");*/
	{
	//	DatasetLite::IShapeFileIndexPtr pShapeIndex = DatasetLite::IShapeFileIndex::create(L"D:\\test\\GIS\\GIS\\src\\GisEngine\\Tests\\TestData\\world_adm0.idx", 8192, L"D:\\test\\GIS\\GIS\\src\\GisEngine\\Tests\\TestData\\world_adm0.shp" );
	}

	{
		DatasetLite::IShapeFileIndexPtr pShapeIndex = DatasetLite::IShapeFileIndex::open(L"D:\\test\\GIS\\GIS\\src\\GisEngine\\Tests\\TestData\\world_adm0.idx");
	
		CommonLib::bbox bbox;
		bbox.xMin = 10;
		bbox.xMax = 20;
		bbox.yMin = 10;
		bbox.yMax = 20;
		DatasetLite::IShapeCursorPtr pCursor = pShapeIndex->spatialQuery(bbox);
		int nId = 0;
		int nCnt = 0;
		CommonLib::bbox Shapebbox;
		while(!pCursor->IsEnd())
		{
			nId = pCursor->row();
			Shapebbox = pCursor->extent();
			pCursor->next();
			nCnt++;
		}
		
	}
	
}