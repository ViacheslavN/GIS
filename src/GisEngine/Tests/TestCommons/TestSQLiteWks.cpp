#include "stdafx.h"
#include "TestSQLiteWks.h"
#include "../../GeoDatabase/SQLiteWorkspace.h"
#include "../../GeoDatabase/ShapefileWorkspace.h"
#include "../../GeoDatabase/QueryFilter.h"
#include "../../GeoDatabase/SQLiteInsertCursor.h"
#include "../../GeoDatabase/SQLiteFeatureClass.h"
#include "CommonLibrary/MemoryStream.h"
void TestSQLiteWks()
{
																													 
	//GisEngine::GeoDatabase::IWorkspacePtr pShapeWks  = GisEngine::GeoDatabase::CShapefileWorkspace::Open(L"ShapeTest", L"D:\\test\\GIS\\GIS\\src\\GisEngine\\Tests\\TestData");
	//GisEngine::GeoDatabase::IWorkspacePtr pShapeWks  = GisEngine::GeoDatabase::CShapefileWorkspace::Open(L"ShapeTest", L"d:\\work\\MyProject\\GIS\\src\\GisEngine\\Tests\\TestData");
	GisEngine::GeoDatabase::IWorkspacePtr pShapeWks  = GisEngine::GeoDatabase::CShapefileWorkspace::Open(L"ShapeTest", L"d:\\db\\10m_cultural\\");
	//GisEngine::GeoDatabase::IFeatureClassPtr pShapeFC = pShapeWks->OpenFeatureClass(L"building.shp");
	GisEngine::GeoDatabase::IFeatureClassPtr pShapeFC = pShapeWks->OpenFeatureClass(L"ne_10m_roads_north_america.shp");
	if(!pShapeFC.get())
		return;

	
	//GisEngine::GeoDatabase::IWorkspacePtr pWks  = GisEngine::GeoDatabase::CSQLiteWorkspace::Open( L"TestSpatialDB.sqlite", L"d:\\work\\MyProject\\GIS\\src\\GisEngine\\Tests\\TestData", true, false);
		
	//GisEngine::GeoDatabase::IWorkspacePtr pWks  = GisEngine::GeoDatabase::CSQLiteWorkspace::Open( L"building.sqlite", L"d:\\db", true, true);
	GisEngine::GeoDatabase::IWorkspacePtr pWks  = GisEngine::GeoDatabase::CSQLiteWorkspace::Open( L"ne_10m_roads_north_america.sqlite", L"d:\\db", true, true);
	if(!pWks.get())
	{
		//pWks = GisEngine::GeoDatabase::CSQLiteWorkspace::Create( L"TestSpatialDB.sqlite", L"d:\\work\\MyProject\\GIS\\src\\GisEngine\\Tests\\TestData");
		//pWks = GisEngine::GeoDatabase::CSQLiteWorkspace::Create( L"building.sqlite", L"d:\\db");
		pWks = GisEngine::GeoDatabase::CSQLiteWorkspace::Create( L"ne_10m_roads_north_america.sqlite", L"d:\\db");
	}

	GisEngine::GeoDatabase::IFeatureClassPtr pSQLFC = pWks->OpenFeatureClass(L"ne_10m_roads_north_america");
	if(!pSQLFC.get())
	{
		GisEngine::GeoDatabase::ITransactionPtr pTran = pWks->startTransaction(GisEngine::GeoDatabase::ttModify);
		if(!pTran.get())
			return;

		pSQLFC = pWks->CreateFeatureClass(L"ne_10m_roads_north_america", pShapeFC->GetFields().get(), true/*, pShapeFC->GetOIDFieldName(), 	pShapeFC->GetShapeFieldName()*/);



		/*GisEngine::GeoDatabase::IShapeField *pShapeField = (GisEngine::GeoDatabase::IShapeField*)pShapeFC->GetFields()->GetField(pShapeFC->GetShapeFieldName()).get();
		GisEngine::GeoDatabase::IShapeField *pSQLLiteShapeField = (GisEngine::GeoDatabase::IShapeField*)pSQLFC->GetFields()->GetField(pShapeFC->GetShapeFieldName()).get();

		GisEngine::GeoDatabase::IField *pOIDField = pShapeFC->GetFields()->GetField(pShapeFC->GetOIDFieldName()).get();
		GisEngine::GeoDatabase::IField *pSQLOIDField = pSQLFC->GetFields()->GetField(pShapeFC->GetOIDFieldName()).get();


		pSQLOIDField->SetType(pOIDField->GetType());


		pSQLLiteShapeField->SetType(pShapeField->GetType());
		pSQLLiteShapeField->SetGeometryDef(pShapeField->GetGeometryDef().get());*/
 
		GisEngine::GeoDatabase::CQueryFilter filter;
		filter.AddRef();
		filter.SetOutputSpatialReference(pShapeFC->GetSpatialReference().get());
		filter.SetSpatialRel(GisEngine::GeoDatabase::srlIntersects);
		filter.SetBB(pShapeFC->GetExtent()->GetBoundingBox());
		filter.SetShapeField(pShapeFC->GetShapeFieldName());

		GisEngine::GeoDatabase::IFieldSetPtr pFieldSet = filter.GetFieldSet();
		for (size_t i = 0, sz = pShapeFC->GetFields()->GetFieldCount(); i < sz; ++i)
		{
			pFieldSet->Add(pShapeFC->GetFields()->GetField(i)->GetName());
		}

		GisEngine::GeoDatabase::ICursorPtr pCursor = pShapeFC->Search(&filter, true);
		GisEngine::GeoDatabase::IRowPtr pRow;


		GisEngine::GeoDatabase::IInsertCursorPtr pInsertCursor = pTran->CreateInsertCusor((GisEngine::GeoDatabase::ITable*)pSQLFC.get());

		//GisEngine::GeoDatabase::CSQLiteWorkspace *pSQLWke = (GisEngine::GeoDatabase::CSQLiteWorkspace *)pWks.get();
		//GisEngine::GeoDatabase::SQLiteInsertCursor SQLiteCusor((GisEngine::GeoDatabase::ITable*)pSQLFC.get(), NULL, pSQLWke->GetDB());
		uint32 nRow = 0;
		int64 nRowID = 0;
		int nError = 0;
		std::set<int64> nOID;

		while(pCursor->NextRow(&pRow))
		{
			nRow++;
		
			nOID.insert(pRow->GetOID());
			nRowID = pInsertCursor->InsertRow(pRow.get());
			if(nRowID == -1)
				nError++;

		}
		pTran->commit();
	}
	else
	{
		std::set<int64> setSQLLiteRow, setShapeRow;
		CommonLib::bbox ShapeBB, SQLiteBB; 
		{

			SQLiteBB = pSQLFC->GetExtent()->GetBoundingBox();
			GisEngine::GeoDatabase::CQueryFilter filter;
			filter.AddRef();
			filter.SetOutputSpatialReference(pSQLFC->GetSpatialReference().get());
			filter.SetSpatialRel(GisEngine::GeoDatabase::srlIntersects);
		
			filter.SetShapeField(pSQLFC->GetShapeFieldName());

			SQLiteBB.xMin -= 1;
			SQLiteBB.xMax += 1;

			SQLiteBB.yMin -= 1;
			SQLiteBB.yMax += 1;

			filter.SetBB(SQLiteBB);

			GisEngine::GeoDatabase::IFieldSetPtr pFieldSet = filter.GetFieldSet();
			for (size_t i = 0, sz = pSQLFC->GetFields()->GetFieldCount(); i < sz; ++i)
			{
				pFieldSet->Add(pSQLFC->GetFields()->GetField(i)->GetName());
			}
			int nRow = 0;
			int nError = 0;
			GisEngine::GeoDatabase::ICursorPtr pCursor = pSQLFC->Search(&filter, true);
			GisEngine::GeoDatabase::IRowPtr pRow;
			while(pCursor->NextRow(&pRow))
			{
				nRow++;
				setSQLLiteRow.insert(pRow->GetOID());
			}
		}
		{
			GisEngine::GeoDatabase::CQueryFilter filter;
			filter.AddRef();
			filter.SetOutputSpatialReference(pShapeFC->GetSpatialReference().get());
			filter.SetSpatialRel(GisEngine::GeoDatabase::srlIntersects);
			filter.SetBB(pShapeFC->GetExtent()->GetBoundingBox());
			filter.SetShapeField(pShapeFC->GetShapeFieldName());
			ShapeBB = pShapeFC->GetExtent()->GetBoundingBox();
			GisEngine::GeoDatabase::IFieldSetPtr pFieldSet = filter.GetFieldSet();
			for (size_t i = 0, sz = pShapeFC->GetFields()->GetFieldCount(); i < sz; ++i)
			{
				pFieldSet->Add(pShapeFC->GetFields()->GetField(i)->GetName());
			}

			GisEngine::GeoDatabase::ICursorPtr pCursor = pShapeFC->Search(&filter, true);
			GisEngine::GeoDatabase::IRowPtr pRow;

			uint32 nRow = 0;
			while(pCursor->NextRow(&pRow))
			{
				nRow++;
				setShapeRow.insert(pRow->GetOID());
			}
		}
	
		for (auto it = setSQLLiteRow.begin(); it != setSQLLiteRow.end(); ++it)
		{
			setShapeRow.erase(*it);
		}

		for (auto it = setShapeRow.begin(); it != setShapeRow.end(); ++it)
		{
			GisEngine::GeoDatabase::IRowPtr pRow = pSQLFC->GetRow(*it);
			GisEngine::GeoDatabase::IFeature* pFeature = (GisEngine::GeoDatabase::IFeature*)pRow.get();
			CommonLib::IGeoShapePtr pShape = pFeature->GetShape();
			CommonLib::bbox bb = pShape->getBB();
		}

	}
	
}