#include "stdafx.h"
#include "TestSQLiteWks.h"
#include "../../GeoDatabase/SQLiteWorkspace.h"
void TestSQLiteWks()
{
	GisEngine::GeoDatabase::IWorkspacePtr pWks = GisEngine::GeoDatabase::CSQLiteWorkspace::Create( L"test", L"D:\\xml");
	if(!pWks.get())
	{
		pWks = GisEngine::GeoDatabase::CSQLiteWorkspace::Open( L"test", L"D:\\xml", true);
	}

}