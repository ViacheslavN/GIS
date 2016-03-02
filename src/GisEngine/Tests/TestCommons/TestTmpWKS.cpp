#include "stdafx.h"
#include "TestSQLiteWks.h"
#include "../../GeoDatabase/SQLiteWorkspace.h"
#include "../../GeoDatabase/ShapefileWorkspace.h"
#include "../../GeoDatabase/QueryFilter.h"
#include "../../GeoDatabase/SQLiteInsertCursor.h"
#include "../../GeoDatabase/SQLiteFeatureClass.h"
#include "CommonLibrary/MemoryStream.h"
#include "CommonLibrary/File.h"

#include "../../GeoDatabase/EmbDBWorkspace.h"
#include "../../GeoDatabase/EmbDBInsertCursor.h"
#include "../../GeoDatabase/EmbDBFeatureClass.h"
#include "../../GeoDatabase/Field.h"
#include "../../GeoDatabase/Fields.h"
#include "../../GeoDatabase/Feature.h"




void TableFromShape()
{
	CommonLib::FileSystem::deleteFile(L"d:\\db\\testTableShape.embdb");
	CommonLib::FileSystem::deleteFile(L"d:\\db\\testTableShape.sqlite");

	GisEngine::GeoDatabase::IWorkspacePtr pShapeWks  = GisEngine::GeoDatabase::CShapefileWorkspace::Open(L"ShapeTest", L"d:\\work\\MyProject\\GIS\\src\\GisEngine\\Tests\\TestData");

	GisEngine::GeoDatabase::IFeatureClassPtr pShapeFC = pShapeWks->OpenFeatureClass(L"building.shp");
	if(!pShapeFC.get())
		return;

	GisEngine::GeoDatabase::IWorkspacePtr pEmbDBWks  = GisEngine::GeoDatabase::CEmbDBWorkspace::Create( L"testTableShape.embdb", L"d:\\db");
	GisEngine::GeoDatabase::IWorkspacePtr pSQLiteWks  = GisEngine::GeoDatabase::CSQLiteWorkspace::Create( L"testTableShape.sqlite", L"d:\\db");


	GisEngine::GeoDatabase::IFieldsPtr pFields = pShapeFC->GetFields()->clone();

	int nIndex = pFields->FindField(pShapeFC->GetShapeFieldName());
	if(nIndex != -1)
		pFields->RemoveField(nIndex);
	
	GisEngine::GeoDatabase::ITablePtr pEmbDBTable = pEmbDBWks->CreateTable(L"testTableShape", pFields.get());
	GisEngine::GeoDatabase::ITablePtr pSqliteTable = pSQLiteWks->CreateTable(L"testTableShape", pFields.get());

	GisEngine::GeoDatabase::IRowPtr pRow(new GisEngine::GeoDatabase::CFeature(NULL, pEmbDBTable->GetFields().get()));
	{
		GisEngine::GeoDatabase::ITransactionPtr pTran = pEmbDBWks->startTransaction(GisEngine::GeoDatabase::ttModify);
		if(!pTran.get())
			return;
		GisEngine::GeoDatabase::IInsertCursorPtr pInsertCursor = pTran->CreateInsertCusor(pEmbDBTable.get());


		GisEngine::GeoDatabase::CQueryFilter filter;
		filter.AddRef();
		filter.SetOutputSpatialReference(pShapeFC->GetSpatialReference().get());
		filter.SetSpatialRel(GisEngine::GeoDatabase::srlIntersects);
		filter.SetBB(pShapeFC->GetExtent()->GetBoundingBox());
		filter.SetShapeField(pShapeFC->GetShapeFieldName());

		GisEngine::GeoDatabase::IFieldSetPtr pFieldSet = filter.GetFieldSet();
		for (size_t i = 0, sz = pEmbDBTable->GetFields()->GetFieldCount(); i < sz; ++i)
		{
			pFieldSet->Add(pEmbDBTable->GetFields()->GetField(i)->GetName());
		}

		GisEngine::GeoDatabase::ICursorPtr pCursor = pShapeFC->Search(&filter, true);
		GisEngine::GeoDatabase::IRowPtr pRow;

		uint32 nRow = 0;
		int64 nRowID = 0;
		int nError = 0;
		while(pCursor->NextRow(&pRow))
		{
			nRow++;

			nRowID = pInsertCursor->InsertRow(pRow.get());
			if(nRowID == -1)
				nError++;

		}

		pTran->commit();
	}
	{
		GisEngine::GeoDatabase::ITransactionPtr pTran = pSQLiteWks->startTransaction(GisEngine::GeoDatabase::ttModify);
		if(!pTran.get())
			return;

		GisEngine::GeoDatabase::IFieldPtr pField = pSqliteTable->GetFields()->GetField(L"ObjectID");
		pField->SetType(GisEngine::GeoDatabase::dtOid32);

		GisEngine::GeoDatabase::IInsertCursorPtr pInsertCursor = pTran->CreateInsertCusor(pSqliteTable.get());

		GisEngine::GeoDatabase::CQueryFilter filter;
		filter.AddRef();
		filter.SetOutputSpatialReference(pShapeFC->GetSpatialReference().get());
		filter.SetSpatialRel(GisEngine::GeoDatabase::srlIntersects);
		filter.SetBB(pShapeFC->GetExtent()->GetBoundingBox());
		filter.SetShapeField(pShapeFC->GetShapeFieldName());

		GisEngine::GeoDatabase::IFieldSetPtr pFieldSet = filter.GetFieldSet();
		for (size_t i = 0, sz = pSqliteTable->GetFields()->GetFieldCount(); i < sz; ++i)
		{
			pFieldSet->Add(pSqliteTable->GetFields()->GetField(i)->GetName());
		}

		GisEngine::GeoDatabase::ICursorPtr pCursor = pShapeFC->Search(&filter, true);
		GisEngine::GeoDatabase::IRowPtr pRow;

		uint32 nRow = 0;
		int64 nRowID = 0;
		int nError = 0;
		while(pCursor->NextRow(&pRow))
		{
			nRow++;

			nRowID = pInsertCursor->InsertRow(pRow.get());
			if(nRowID == -1)
				nError++;

		}

		pTran->commit();
	}


}

void TestTableWks()
{

	 TableFromShape();
	 return;
 
	CommonLib::FileSystem::deleteFile(L"d:\\db\\testTable.embdb");
	CommonLib::FileSystem::deleteFile(L"d:\\db\\testTable.sqlite");

	GisEngine::GeoDatabase::IFieldPtr pField(new GisEngine::GeoDatabase::CField());

	pField->SetType(GisEngine::GeoDatabase::dtInteger64);
	pField->SetName(L"testIntegerField");
	pField->SetAliasName(L"testIntegerField Alias");

	GisEngine::GeoDatabase::IFieldPtr pField2(new GisEngine::GeoDatabase::CField());

	pField2->SetType(GisEngine::GeoDatabase::dtInteger64);
	pField2->SetName(L"testIntegerField2");
	pField2->SetAliasName(L"testIntegerField2 Alias");


	GisEngine::GeoDatabase::IFieldPtr pField3(new GisEngine::GeoDatabase::CField());

	pField3->SetType(GisEngine::GeoDatabase::dtString);
	pField3->SetName(L"testStringrField2");
	pField3->SetLength(64);
	pField3->SetAliasName(L"testStringrField2 Alias");

	GisEngine::GeoDatabase::IFieldsPtr pFields(new GisEngine::GeoDatabase::CFields());

	pFields->AddField(pField.get());
	pFields->AddField(pField2.get());
	pFields->AddField(pField3.get());

	GisEngine::GeoDatabase::IWorkspacePtr pEmbDBWks  = GisEngine::GeoDatabase::CEmbDBWorkspace::Create( L"testTable.embdb", L"d:\\db");
	GisEngine::GeoDatabase::IWorkspacePtr pSQLiteWks  = GisEngine::GeoDatabase::CSQLiteWorkspace::Create( L"testTable.sqlite", L"d:\\db");


	GisEngine::GeoDatabase::ITablePtr pEmbDBTable = pEmbDBWks->CreateTable(L"testTable", pFields.get());
	GisEngine::GeoDatabase::ITablePtr pSqliteTable = pSQLiteWks->CreateTable(L"testTable", pFields.get());



	int64 nBegin = (int64)2 << 33; 
	int64 nEnd = (int64)nBegin + 100000; 

	GisEngine::GeoDatabase::IRowPtr pRow(new GisEngine::GeoDatabase::CFeature(NULL, pEmbDBTable->GetFields().get()));
	{
		GisEngine::GeoDatabase::ITransactionPtr pTran = pEmbDBWks->startTransaction(GisEngine::GeoDatabase::ttModify);
		if(!pTran.get())
			return;
		GisEngine::GeoDatabase::IInsertCursorPtr pInsertCursor = pTran->CreateInsertCusor(pEmbDBTable.get());

		CommonLib::CVariant var(int64(0));
		CommonLib::CString sString;
		for (int64 i = nBegin; i < nEnd; ++i)
		{
			var = i;

			
			
			sString.format(L"ssssss_%I64d", i);
			CommonLib::CVariant str(sString);
			pRow->SetValue(0, var);
			pRow->SetValue(1, var);
			pRow->SetValue(2, str);
			pInsertCursor->InsertRow(pRow.get());
		}

		pTran->commit();
	}


	{
		GisEngine::GeoDatabase::ITransactionPtr pTran = pSQLiteWks->startTransaction(GisEngine::GeoDatabase::ttModify);
		if(!pTran.get())
			return;
		GisEngine::GeoDatabase::IInsertCursorPtr pInsertCursor = pTran->CreateInsertCusor(pSqliteTable.get());

		CommonLib::CVariant var(int64(0));
		CommonLib::CString sString;
		for (int64 i = nBegin; i < nEnd; ++i)
		{
			sString.format(L"ssssss_%I64d", i);
			CommonLib::CVariant str(sString);
			var = i;
			pRow->SetValue(0, var);
			pRow->SetValue(1, var);
			pRow->SetValue(2, str);
			pInsertCursor->InsertRow(pRow.get());
		}

		pTran->commit();
	}


}