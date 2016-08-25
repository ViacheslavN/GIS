#include "stdafx.h"
#include "CommonLibrary/File.h"

void CreateDB(const wchar_t *pszDBName)
{
	embDB::CDatabase db;
	CommonLib::CString sDBPath = CommonLib::FileSystem::FindFilePath(pszDBName);
	if(db.open(pszDBName, embDB::eTMSingleTransactions, sDBPath.wstr()))
		return;


	if(!db.create(pszDBName, embDB::eTMSingleTransactions,  sDBPath.wstr()))
	{
		std::cout << "Error create db";
		return;
	}

	embDB::SFieldProp  fp;
	fp.m_sFieldName = L"Öåëî÷èñëåííîå ïîëå integer";
	fp.m_sFieldAlias = L"dddddddddddddddddddddddddddddddddddddddddaaaaaaaaaaaaaaaaaaaaaaaaaaaaaafffffffffffffffffffffèèèèèèèèèèèèèèèèèèèè";
	fp.m_dataType = embDB::dtInteger32;



	embDB::IConnectionPtr pConnect = db.connect(NULL, NULL);
	embDB::ITransactionPtr pTran = pConnect->startTransaction(embDB::eTT_DDL) ;

	embDB::ISchemaPtr pSchema =  pConnect->getSchema();

	pSchema->addTable(L"testtable1", pTran.get());
	embDB::ITablePtr pTable = pSchema->getTableByName(L"testtable1");

	if(!pTable.get())
		return;

	embDB::IDBTable *pDBTable = dynamic_cast<embDB::IDBTable*>(pTable.get());

	pDBTable->createField(fp, pTran.get());


	embDB::SFieldProp  blobfp;
	blobfp.m_sFieldName = L"Blob field dddddddddddddddddddddddddddddddddrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr";
	blobfp.m_sFieldAlias = L"ddddddddddddddddddddddddddddddddddddddddddddddddddddaaaaaaaaaaaaaaaaaaaaaaaaaaaaaafffffffffffffffffffffèèèèèèèèèèèèèèèèèèèè";
	blobfp.m_dataType = embDB::dtBlob;
	
	pDBTable->createField(blobfp, pTran.get());

	pTran->commit();
}


void TestAddFieldDB()
{
	CreateDB(L"D:\\db\\testDB.embdb");
}