#include "stdafx.h"




static int16 __nCount_16__ = 20000;
static int32 __nCount_32__ = 100000;
static int64 __nCount_64__ = 100000;


void AddField(const wchar_t *pszFieldName, embDB::eDataTypes type, embDB::ITable* pTable, embDB::ITransaction* pTran)
{
	embDB::SFieldProp fp;

	fp.m_sFieldName = pszFieldName;
	fp.m_dataType = type;

	pTable->createField(fp, pTran);

	embDB::SIndexProp ip;

	ip.m_indexType = embDB::itUnique;


	pTable->createIndex(pszFieldName, ip, pTran);
	embDB::SStatisticInfo si;
	si.m_Statistic = embDB::stFullStatistic;

	pTable->createStatistic(pszFieldName, si, pTran);
}

void CreateIntegerDB()
{
	if (CommonLib::FileSystem::isFileExisit(L"D:\\db\\TestIntegerField"))
		CommonLib::FileSystem::deleteFile(L"D:\\db\\TestIntegerField");

	embDB::IDatabasePtr pDatabase = embDB::IDatabase::CreateDatabase();
	pDatabase->create(L"D:\\db\\TestIntegerField");
	embDB::IConnectionPtr pConnect = pDatabase->connect();

	embDB::ITransactionPtr pTran = pConnect->startTransaction(embDB::eTT_DDL);
	pTran->begin();

	embDB::ISchemaPtr pShema = pConnect->getSchema();

	pShema->addTable(L"TableInteger", pTran.get());

	embDB::ITablePtr pTable = pShema->getTableByName(L"TableInteger");


	AddField(L"Integer16", embDB::dtInteger16, pTable.get(), pTran.get());
	AddField(L"IntegerU16", embDB::dtUInteger16, pTable.get(), pTran.get());

	AddField(L"Integer32", embDB::dtInteger32, pTable.get(), pTran.get());
	AddField(L"IntegerU32", embDB::dtUInteger32, pTable.get(), pTran.get());

	AddField(L"Integer64", embDB::dtInteger64, pTable.get(), pTran.get());
	AddField(L"IntegerU64", embDB::dtUInteger64, pTable.get(), pTran.get());


	pTran->commit();
	pConnect->closeTransaction(pTran.get());
	pDatabase->closeConnection(pConnect.get());
	pDatabase->close();
}


template<class Type>
void InsertField(embDB::ITable* pTable, const wchar_t *pszFieldName, embDB::IConnection* pConnect, Type nBegin, Type nEnd)
{
	embDB::ITransactionPtr pTran = pConnect->startTransaction(embDB::eTT_MODIFY);
	pTran->begin();


	embDB::CFieldSet FieldSet;
	FieldSet.AddRef();
	FieldSet.Add(pszFieldName);

	int64 n = 0;
	int64 nCount = nEnd - nBegin;
	int64 nStep = nCount / 100;
	
	embDB::IInsertCursorPtr pCursor = pTran->createInsertCursor(L"TableInteger", &FieldSet);
	embDB::IRowPtr pRow =  pCursor->createRow();
	CommonLib::CVariant var;
	for (Type i = nBegin; i < nEnd; ++i)
	{
		var = i;
		pRow->set(var, 0);

		pCursor->insert(pRow.get());

		n += 1;

		if (i%nStep == 0)
		{
			std::cout << n << "  " << (n * 100) / nCount << " %" << '\r';
		}
	}

	pTable->UpdateStatistic(pszFieldName, pTran.get());
	pTran->commit();
	pConnect->closeTransaction(pTran.get());
}


void InsertIntegerDB()
{
	embDB::IDatabasePtr pDatabase = embDB::IDatabase::CreateDatabase();
	pDatabase->open(L"D:\\db\\TestIntegerField");

	embDB::IConnectionPtr pConnect = pDatabase->connect();
	embDB::ISchemaPtr pShema = pConnect->getSchema();
	embDB::ITablePtr pTable = pShema->getTableByName(L"TableInteger");

	std::cout << "Integer16" << std::endl;
	InsertField<int16>(pTable.get(), L"Integer16", pConnect.get(), 0, __nCount_16__);
	std::cout << "Integer32" << std::endl;
	InsertField<int32>(pTable.get(), L"Integer32", pConnect.get(), 0, __nCount_32__);
	std::cout << "Integer64" << std::endl;
	InsertField<uint64>(pTable.get(), L"IntegerU64", pConnect.get(), 0, __nCount_64__);

	pDatabase->closeConnection(pConnect.get());
	pDatabase->close();
}


template<class Type>
void FindField(embDB::ITable* pTable, const wchar_t *pszFieldName, embDB::IConnection* pConnect, Type nBegin, Type nEnd)
{
	embDB::ITransactionPtr pTran = pConnect->startTransaction(embDB::eTT_SELECT);
	pTran->begin();


	embDB::CFieldSet FieldSet;
	FieldSet.AddRef();
	FieldSet.Add(pszFieldName);


	int64 n = 0;
	int64 nCount = nEnd - nBegin;
	int64 nStep = nCount / 100;

	CommonLib::CVariant var;
	CommonLib::CVariant FindVar;
	embDB::IRowPtr pRow;
	for (Type i = nBegin; i < nEnd; ++i)
	{
		var = i;
 

		embDB::ICursorPtr pCursor = pTran->executeSelectQuery(pTable->getName().cwstr(), &FieldSet, pszFieldName, var, embDB::OpEqual);

		int64 nRow = pCursor->GetRowID();
		if (nRow != -1)
		{
			if (!pCursor->value(&FindVar, 0))
			{
				std::cout << "Not found val " << i << std::endl;
				continue;
			}
 
			if (FindVar.Get<Type>() != i)
			{
				std::cout << "Not found val " << i << std::endl;
			}
		}
		

		n += 1;

		if (i%nStep == 0)
		{
			std::cout << n << "  " << (n * 100) / nCount << " %" << '\r';
		}
	}

	pConnect->closeTransaction(pTran.get());
}

void FindIntegerDB()
{
	embDB::IDatabasePtr pDatabase = embDB::IDatabase::CreateDatabase();
	pDatabase->open(L"D:\\db\\TestIntegerField");

	embDB::IConnectionPtr pConnect = pDatabase->connect();
	embDB::ISchemaPtr pShema = pConnect->getSchema();
	embDB::ITablePtr pTable = pShema->getTableByName(L"TableInteger");

	std::cout << "Find Integer16" << std::endl;
	FindField<int16>(pTable.get(), L"Integer16", pConnect.get(), 0, __nCount_16__);
	std::cout << "Find Integer32" << std::endl;
	FindField<int32>(pTable.get(), L"Integer32", pConnect.get(), 0, __nCount_32__);
	std::cout << "Find Integer64" << std::endl;
	FindField<uint64>(pTable.get(), L"IntegerU64", pConnect.get(), 0, __nCount_64__);
	

	pDatabase->closeConnection(pConnect.get());
	pDatabase->close();
}


template<class Type>
void RemoveField(embDB::ITable* pTable, const wchar_t *pszFieldName, embDB::IConnection* pConnect, Type nBegin, Type nEnd)
{
	embDB::ITransactionPtr pTran = pConnect->startTransaction(embDB::eTT_MODIFY);
	pTran->begin();


	embDB::CFieldSet FieldSet;
	FieldSet.AddRef();
	FieldSet.Add(pszFieldName);


	int64 n = 0;
	int64 nCount = nEnd - nBegin;
	int64 nStep = nCount / 100;

	CommonLib::CVariant var;
	CommonLib::CVariant FindVar;
	embDB::IRowPtr pRow;

	embDB::IDeleteCursorPtr pDelCursor = pTran->createDeleteCursor(pTable->getName().cwstr());
	for (Type i = nBegin; i < nEnd; ++i)
	{
		var = i;


		embDB::ICursorPtr pCursor = pTran->executeSelectQuery(pTable->getName().cwstr(), &FieldSet, pszFieldName, var, embDB::OpEqual);

		int64 nRow = pCursor->GetRowID();
		if (nRow != -1)
		{
			if (!pCursor->value(&FindVar, 0))
			{
				std::cout << "Not found val " << i << std::endl;
				continue;
			}

			if (FindVar.Get<Type>() != i)
			{
				std::cout << "Not found val " << i << std::endl;
				continue;
			}
			pDelCursor->remove(nRow);
		}
		else
		{
			std::cout << "Not found val RowID == -1" << i << std::endl;
		}


		n += 1;

		if (i%nStep == 0)
		{
			std::cout << n << "  " << (n * 100) / nCount << " %" << '\r';
		}
	}
	pTable->UpdateStatistic(pszFieldName, pTran.get());
	pTran->commit();
	pConnect->closeTransaction(pTran.get());
}




void RemoveFromIntegerDB()
{
	embDB::IDatabasePtr pDatabase = embDB::IDatabase::CreateDatabase();
	pDatabase->open(L"D:\\db\\TestIntegerField");

	embDB::IConnectionPtr pConnect = pDatabase->connect();
	embDB::ISchemaPtr pShema = pConnect->getSchema();
	embDB::ITablePtr pTable = pShema->getTableByName(L"TableInteger");


	std::cout << "Remove Integer16" << std::endl;
	RemoveField<int16>(pTable.get(), L"Integer16", pConnect.get(), 0, __nCount_16__/2);
	std::cout << "Remove Integer32" << std::endl;
	RemoveField<int32>(pTable.get(), L"Integer32", pConnect.get(), 0, __nCount_32__ / 2);
	std::cout << "Remove Integer64" << std::endl;
	RemoveField<uint64>(pTable.get(), L"IntegerU64", pConnect.get(), 0, __nCount_64__ / 2);



	pDatabase->closeConnection(pConnect.get());
	pDatabase->close();
 
}


void TestIntegerField()
{
	CreateIntegerDB();
	InsertIntegerDB();
	FindIntegerDB();
	RemoveFromIntegerDB();
}