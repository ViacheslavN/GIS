#include "stdafx.h"
static uint32 _nCount_string_ = 100000;
void CreateStringDataBase()
{
	if (CommonLib::FileSystem::isFileExisit(L"D:\\db\\TestStringField"))
		CommonLib::FileSystem::deleteFile(L"D:\\db\\TestStringField");

	embDB::IDatabasePtr pDatabase = embDB::IDatabase::CreateDatabase();
	pDatabase->create(L"D:\\db\\TestStringField");
	embDB::IConnectionPtr pConnect = pDatabase->connect();

	embDB::ITransactionPtr pTran = pConnect->startTransaction(embDB::eTT_DDL);
	pTran->begin();

	embDB::ISchemaPtr pShema = pConnect->getSchema();

	pShema->addTable(L"StringTable", pTran.get());

	embDB::ITablePtr pTable = pShema->getTableByName(L"StringTable");

	embDB::SFieldProp fp;

	fp.m_sFieldName = "StringField";
	fp.m_dataType = embDB::dtString;
	fp.m_nLenField = 100;

	pTable->createField(fp, pTran.get());


	pTran->commit();
	pConnect->closeTransaction(pTran.get());
	pDatabase->closeConnection(pConnect.get());
	pDatabase->close();
}


void InsertStringData()
{
	embDB::IDatabasePtr pDatabase = embDB::IDatabase::CreateDatabase();
	pDatabase->open(L"D:\\db\\TestStringField");

	embDB::IConnectionPtr pConnect = pDatabase->connect();
	embDB::ISchemaPtr pShema = pConnect->getSchema();
	embDB::ITablePtr pTable = pShema->getTableByName(L"StringTable");

	embDB::ITransactionPtr pTran = pConnect->startTransaction(embDB::eTT_MODIFY);
	pTran->begin();


	embDB::CFieldSet FieldSet;
	FieldSet.AddRef();
	FieldSet.Add("StringField");

	embDB::IInsertCursorPtr pCursor = pTran->createInsertCursor(L"StringTable", &FieldSet);
	embDB::IRowPtr pRow = pCursor->createRow();
	CommonLib::CVariant var;

	int64 n = 0;
	int64 nCount = _nCount_string_;
	int64 nStep = nCount / 100;

	for (uint32 i = 0; i < _nCount_string_; ++i)
	{
		CommonLib::CString str;
		str.format("TestString %d", i);

		if (i%nStep == 0)
		{
			std::cout << n << "  " << (n * 100) / nCount << " %" << '\r';
		}
		var = str;
		pRow->set(var, 0);
		pCursor->insert(pRow.get());
		
	}

	pTran->commit();

	pDatabase->closeConnection(pConnect.get());
	pDatabase->close();
}

void SelectStringData()
{
	embDB::IDatabasePtr pDatabase = embDB::IDatabase::CreateDatabase();
	pDatabase->open(L"D:\\db\\TestStringField");

	embDB::IConnectionPtr pConnect = pDatabase->connect();
 

	embDB::ITransactionPtr pTran = pConnect->startTransaction(embDB::eTT_SELECT);
	pTran->begin();
 

	embDB::ICursorPtr pCursor = pTran->executeSelectQuery(L"StringTable");
 
 
	uint32 i = 0;
	embDB::IRowPtr pRow;
	while (pCursor->NextRow(&pRow))
	{
	 
		CommonLib::CString str;
		str.format("TestString %d", i);

		CommonLib::CVariant* pVar = pRow->value(0);

		if (pVar->Get<CommonLib::CString>() != str)
		{
			std::cout << "Not found string i " << i << std::endl;
		 }

		++i;
	}

 
	pDatabase->closeConnection(pConnect.get());
	pDatabase->close();
}

void TestStringField()
{
	CreateStringDataBase();
	InsertStringData();
	SelectStringData();
}