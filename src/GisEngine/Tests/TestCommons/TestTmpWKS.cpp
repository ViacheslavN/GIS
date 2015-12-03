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



void TestTableWks()
{
 
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

	GisEngine::GeoDatabase::IFieldsPtr pFields(new GisEngine::GeoDatabase::CFields());

	pFields->AddField(pField.get());
	pFields->AddField(pField2.get());

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
		for (int64 i = nBegin; i < nEnd; ++i)
		{
			var = i;
			pRow->SetValue(0, var);
			pRow->SetValue(1, var);
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
		for (int64 i = nBegin; i < nEnd; ++i)
		{
			var = i;
			pRow->SetValue(0, var);
			pRow->SetValue(1, var);
			pInsertCursor->InsertRow(pRow.get());
		}

		pTran->commit();
	}


}