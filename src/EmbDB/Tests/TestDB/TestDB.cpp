// TestDB.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../../EmbDB/Database.h"
#include "../../EmbDB/VariantField.h"
#include "../../EmbDB/IField.h"
#include "../../EmbDB/Table.h"
#include "../../EmbDB/OIDField.h"
#include "../../EmbDB/Transactions.h"

#include <iostream>
#include "../../EmbDB/BitMap.h"

void TestSpatialField(embDB::CDatabase& db);
int _tmain(int argc, _TCHAR* argv[])
{
	
	embDB::TFieldINT32 int32key;

	//CommonLib::alloc_t* alloc;
	//embDB::BNodeVector<embDB::TFieldINT32> vec(NULL);


	embDB::TFieldINT64 int64key;

	embDB::IFieldVariant *pField = &int32key;
	pField->set((double)34.566);
	pField->set((int)100);
	int nType = pField->getType();
	bool bNew = false;
	embDB::CDatabase db;
	if(!db.open("d:\\db\\db1.emb", "d:\\db"))
	{
		if(!db.create("d:\\db\\db1.emb", 8192, "d:\\db"))
		{
			std::cout << "Error create db";
			return 0;
		}
		else
		{
			bNew = true;
			embDB::CSchema* pSchema = db.getSchema();
			CommonLib::str_t sTableName; 
			embDB::ITransactions *pTableTran = db.startTransaction(embDB::eTT_DDL);
			pTableTran->begin();
			for (int i = 0; i < 1000; ++i)
			{				
				sTableName.format(_T("table_%d"), i);
				pSchema->addTable(sTableName, "", 	(embDB::IDBTransactions*)pTableTran);
			}
			pTableTran->commit();
			db.closeTransaction(pTableTran);
			embDB::CTable *pTable = pSchema->getTable("table_1");
		
			embDB::ITransactions *pFieldTran = db.startTransaction(embDB::eTT_DDL);
			pFieldTran->begin();

			CommonLib::str_t sFieldName; 
			for (int i = 0; i < 1000; ++i)
			{				
			
				sFieldName.format(_T("field_%d"), i);
				embDB::sFieldInfo fi;
				fi.m_sFieldName = sFieldName;
				fi.m_sFieldAlias = "test field 1";
				fi.m_nFieldType = embDB::FT_VALUE_FIELD;
				fi.m_nFieldDataType = embDB::ftInteger32;
				pTable->addField(fi, (embDB::IDBTransactions*)pFieldTran);
			}
			pFieldTran->commit();
			db.closeTransaction(pFieldTran);
		}

		
	}

	embDB::CSchema* pSchema = db.getSchema();
	CommonLib::str_t sTableName; 
	for (int i = 0; i < 1000; ++i)
	{
		sTableName.format(_T("table_%d"), i);
		embDB::CTable *pTable = pSchema->getTable(sTableName);
		if(!pTable)
			std::cout << "Table not found: " << sTableName.cstr();
	}
	embDB::CTable *pTable = pSchema->getTable("table_1");

	CommonLib::str_t sFieldName; 
	for (int i = 0; i < 1000; ++i)
	{				

		sFieldName.format(_T("field_%d"), i);
		embDB::IDBFieldHandler *pField =pTable->getField(sFieldName);
		if(!pField)
			std::cout << "Field not found: " << sFieldName.cstr();
	}
	

	embDB::IDBFieldHandler *pTestField =pTable->getField(_T("field_0"));
	if(!pTestField)								   
		return 0;
	embDB::TOIDFieldINT32 *pOIDFieldHandler = (embDB::TOIDFieldINT32*)pTestField;
	if(!pOIDFieldHandler)
		return 0;

	int nCount = 1000000;
	if(bNew)
	{
		embDB::ITransactions *pTran = db.startTransaction(embDB::eTT_INSERT);
		pTran->begin();
		embDB::IOIDFiled* pOIDField = pOIDFieldHandler->getOIDField((embDB::IDBTransactions*)pTran, (embDB::IDBStorage*)db.getMainStorage());
		

		uint64 nOID = 0;
		embDB::IFieldIterator *pRetIter = NULL;
		embDB::IFieldIterator *pFromIter = NULL;
		for (int i = 0; i < nCount; ++i)
		{
			int32key.set(i);
			
			nOID = pOIDField->insert(&int32key, pFromIter, &pRetIter);
		 
			if(nOID == 0)
			{
				std::cout << "error insert" << i << std::endl;
			}
			pFromIter = pRetIter;
			
		}

		pOIDField->commit();
		pTran->commit();
		db.closeTransaction(pTran);
	}
	else
	{

		embDB::ITransactions *pTran = db.startTransaction(embDB::eTT_SELECT);
		pTran->begin();
		embDB::IOIDFiled* pOIDField = pOIDFieldHandler->getOIDField((embDB::IDBTransactions*)pTran, (embDB::IDBStorage*)db.getMainStorage());


		for (int64 i = 1; i < nCount; ++i)
		{
			if(!pOIDField->find(i, &int32key))
				std::cout << "Not found " << i << std::endl;

			embDB::FieldIteratorPtr pIter = pOIDField->find(i);
		}
		db.closeTransaction(pTran);
	}

	TestSpatialField(db);
	/*embDB::CSchema* pSchema = db.getSchema();
	embDB::CTable *pTable = pSchema->getTable("table1");
	embDB::IDBFieldHandler *pField1 = pTable->getField("field1");
	if(!pField)								   
		return 0;
	embDB::TOIDFieldINT32 *pOIDFieldHandler = (embDB::TOIDFieldINT32*)pField1;
	if(!pOIDFieldHandler)
		return 0;

	embDB::ITransactions *pTran = db.createTransactions();
	embDB::IOIDFiled* pOIDField = pOIDFieldHandler->getOIDField((embDB::IDBTransactions*)pTran, (embDB::IDBStorage*)db.getMainStorage());
	int nCount = 1000000;
	
	if(bNew)
	{
		pTran->begin();
		for (int i = 0; i < nCount; ++i)
		{
			int32key.set(i);
			pOIDField->insert(i, &int32key);
		}
		pOIDField->commit();
		pTran->commit();
	}
	embDB::IDBTransactions *pDBTran = (embDB::IDBTransactions*)pTran;
//	pDBTran->setType(embDB::eTT_UNDEFINED);
	for (int64 i = 0; i < nCount; ++i)
	{
		if(!pOIDField->find(i, &int32key))
			std::cout << "Not found " << i << std::endl;
	}*/
	return 0;
}

