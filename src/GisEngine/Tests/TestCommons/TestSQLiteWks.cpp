#include "stdafx.h"
#include "TestSQLiteWks.h"
#include "../../GeoDatabase/SQLiteWorkspace.h"
#include "../../GeoDatabase/ShapefileWorkspace.h"
void TestSQLiteWks()
{
	GisEngine::GeoDatabase::IWorkspacePtr pShapeWks  = GisEngine::GeoDatabase::CShapefileWorkspace::Open(L"ShapeTest", L"D:\\test\\GIS\\GIS\\src\\GisEngine\\Tests\\TestData");
	GisEngine::GeoDatabase::IFeatureClassPtr pShapeFC = pShapeWks->GetFeatureClass(L"world_adm0.shp");
	if(!pShapeFC.get())
		return;

	
	GisEngine::GeoDatabase::IWorkspacePtr pWks = GisEngine::GeoDatabase::CSQLiteWorkspace::Create( L"test", L"D:\\xml");
	if(!pWks.get())
	{
		pWks = GisEngine::GeoDatabase::CSQLiteWorkspace::Open( L"test", L"D:\\xml", true);
	}

	GisEngine::GeoDatabase::IFeatureClassPtr pSQLFC = pWks->GetFeatureClass(L"TestFeatureClass");
	if(!pSQLFC.get())
	{
		pSQLFC = pWks->CreateFeatureClass(L"TestTable", pShapeFC->GetFields().get(), pShapeFC->GetOIDFieldName(), 
			pShapeFC->GetShapeFieldName());
	}
 
	
}