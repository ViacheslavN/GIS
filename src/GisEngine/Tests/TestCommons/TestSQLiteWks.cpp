#include "stdafx.h"
#include "TestSQLiteWks.h"
#include "../../GeoDatabase/SQLiteWorkspace.h"
#include "../../GeoDatabase/ShapefileWorkspace.h"
#include "../../GeoDatabase/QueryFilter.h"
#include "../../GeoDatabase/SQLiteInsertCursor.h"
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
		pSQLFC = pWks->CreateFeatureClass(L"TestFeatureClass", pShapeFC->GetFields().get(), pShapeFC->GetOIDFieldName(), 
			pShapeFC->GetShapeFieldName());
	}


	GisEngine::GeoDatabase::IShapeField *pShapeField = (GisEngine::GeoDatabase::IShapeField*)pShapeFC->GetFields()->GetField(pShapeFC->GetShapeFieldName()).get();
	GisEngine::GeoDatabase::IShapeField *pSQLLiteShapeField = (GisEngine::GeoDatabase::IShapeField*)pSQLFC->GetFields()->GetField(pShapeFC->GetShapeFieldName()).get();

	GisEngine::GeoDatabase::IField *pOIDField = pShapeFC->GetFields()->GetField(pShapeFC->GetOIDFieldName()).get();
	GisEngine::GeoDatabase::IField *pSQLOIDField = pSQLFC->GetFields()->GetField(pShapeFC->GetOIDFieldName()).get();


	pSQLOIDField->SetType(pOIDField->GetType());


	pSQLLiteShapeField->SetType(pShapeField->GetType());
	pSQLLiteShapeField->SetGeometryDef(pShapeField->GetGeometryDef().get());
 
	GisEngine::GeoDatabase::CQueryFilter filter;
	filter.AddRef();
	filter.SetOutputSpatialReference(pShapeFC->GetSpatialReference().get());
	filter.SetSpatialRel(GisEngine::GeoDatabase::srlEnvelopeIntersects);
	filter.SetBB(pShapeFC->GetExtent()->GetBoundingBox());
	filter.SetShapeField(pShapeFC->GetShapeFieldName());

	GisEngine::GeoDatabase::IFieldSetPtr pFieldSet = filter.GetFieldSet();
	for (size_t i = 0, sz = pShapeFC->GetFields()->GetFieldCount(); i < sz; ++i)
	{
		pFieldSet->Add(pShapeFC->GetFields()->GetField(i)->GetName());
	}

	GisEngine::GeoDatabase::ICursorPtr pCursor = pShapeFC->Search(&filter, true);
	GisEngine::GeoDatabase::IRowPtr pRow;

	GisEngine::GeoDatabase::CSQLiteWorkspace *pSQLWke = (GisEngine::GeoDatabase::CSQLiteWorkspace *)pWks.get();
	GisEngine::GeoDatabase::SQLiteInsertCursor SQLiteCusor((GisEngine::GeoDatabase::ITable*)pSQLFC.get(), NULL, pSQLWke->GetConnections());
	uint32 nRow = 0;
	int64 nRowID;
	int nError = 0;

	while(pCursor->NextRow(&pRow))
	{
		nRow++;
		nRowID = SQLiteCusor.InsertRow(pRow.get());
		if(nRowID == -1)
			nError++;

	}

	int i = 0;
	i++;

}