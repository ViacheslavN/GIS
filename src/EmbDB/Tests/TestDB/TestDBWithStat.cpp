#include "stdafx.h"



void InsertInDB(embDB::IConnectionPtr& pConnect, int nBegin, int nEnd)
{
	embDB::ITransactionPtr pTran = pConnect->startTransaction(embDB::eTT_MODIFY);
	pTran->begin();

	embDB::IInsertCursorPtr pInsertCursor =	pTran->createInsertCursor(L"testtable1");
	embDB::IRowPtr pRow = pInsertCursor->createRow();

	CommonLib::CVariant value;
	for (int32 i = nBegin; i < nEnd; ++i)
	{
		value = i;
		pRow->set(value, 0);

		pInsertCursor->insert(pRow.get());
	}

	pTran->commit();
}





void UpdateStatistic(embDB::CDatabase& db, embDB::IConnectionPtr& pConnect)
{
	embDB::ISchemaPtr pSchema = pConnect->getSchema();
	embDB::ITablePtr pTable = pSchema->getTableByName(L"testtable1");

	embDB::ITransactionPtr pTran = pConnect->startTransaction(embDB::eTT_MODIFY);
	pTran->begin();

	embDB::IDBTable *pDBTable = dynamic_cast<embDB::IDBTable*>(pTable.get());

	pDBTable->UpdateStatistic(L"Field1", pTran.get());

	pTran->commit();

}


 

void TestDBWithStat()
{
	CommonLib::CString sDBFile = "D:\\db\\testStatDB.emb";
	CommonLib::FileSystem::deleteFile(sDBFile.cwstr());
	embDB::CDatabase db;
	CommonLib::CString sDBPath = CommonLib::FileSystem::FindFilePath(sDBFile.cwstr());

	if (!db.create(sDBFile.cwstr(), embDB::eTMSingleTransactions, sDBPath.cwstr()))
	{
		std::cout << "Error create db";
		return;
	}
	embDB::SFieldProp  fp;
	fp.m_sFieldName = L"Field1";
	fp.m_sFieldAlias = L"dddddddddddddddddddddddddddddddddddddddddaaaaaaaaaaaaaaaaaaaaaaaaaaaaaafffffffffffffffffffffèèèèèèèèèèèèèèèèèèèè";
	fp.m_dataType = embDB::dtInteger32;


	embDB::IConnectionPtr pConnect = db.connect(NULL, NULL);
	embDB::ITransactionPtr pTran = pConnect->startTransaction(embDB::eTT_DDL);
	pTran->begin();
	embDB::ISchemaPtr pSchema = pConnect->getSchema();

	pSchema->addTable(L"testtable1", pTran.get());
	embDB::ITablePtr pTable = pSchema->getTableByName(L"testtable1");


	if (!pTable.get())
		return;

	embDB::IDBTable *pDBTable = dynamic_cast<embDB::IDBTable*>(pTable.get());

	pDBTable->createField(fp, pTran.get());

	embDB::SStatisticInfo si;
	si.m_Statistic = embDB::stFullStatistic;

	pDBTable->createStatistic(fp.m_sFieldName, si, pTran.get());

	pTran->commit();

	pConnect->closeTransaction(pTran.get());
	db.closeConnection(pConnect.get());
	db.close();
	embDB::CDatabase db1;

	if (!db1.open(sDBFile.cwstr(), embDB::eTMSingleTransactions, sDBPath.cwstr()))
	{
		std::cout << "Error open db";
		return;
	}

	embDB::IConnectionPtr pConnect1 = db1.connect();
	if (!pConnect1.get())
	{
		db1.close();
		std::cout << "Error connect";
		return;
	}

	InsertInDB(pConnect1, 0, 1000);
	UpdateStatistic(db, pConnect1);

	InsertInDB(pConnect1, 1000, 10000);

	db.setLogLevel(11);
	UpdateStatistic(db, pConnect1);

	db1.closeConnection(pConnect1.get());
	db1.close();



	embDB::CDatabase db2;

	if (!db2.open(sDBFile.cwstr(), embDB::eTMSingleTransactions, sDBPath.cwstr()))
	{
		std::cout << "Error open db";
		return;
	}

	embDB::IConnectionPtr pConnect2 = db2.connect();
	if (!pConnect2.get())
	{
		db2.close();
		std::cout << "Error connect";
		return;
	}
	UpdateStatistic(db2, pConnect2);

}