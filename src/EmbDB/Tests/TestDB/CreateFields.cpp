#include "stdafx.h"
#include "CreateFields.h"


void CreateField(const CommonLib::CString& sName, const CommonLib::CString& sAlias,  uint32 nFieldType,
	embDB::CTable *pTable)
{

	embDB::SFieldProp fp;
	fp.dataType = (embDB::eDataTypes)nFieldType;
	fp.dateTypeExt = embDB::dteSimple;
	fp.sFieldName = sName;
	fp.sFieldAlias = sAlias;
	pTable->createField(fp);


	embDB::SIndexProp indexProp;
	indexProp.indexType = embDB::itMultiRegular;
	pTable->createIndex(sName, indexProp);
}

void CreateTables(embDB::CDatabase* pDb)
{
	embDB::CSchema* pSchema = pDb->getSchema();
	CommonLib::CString sTableName; 
	embDB::ITransactionPtr pTableTran = pDb->startTransaction(embDB::eTT_DDL);
	pTableTran->begin();
	pSchema->addTable("Table", "", 	(embDB::IDBTransaction*)pTableTran.get());
	pTableTran->commit();
	pDb->closeTransaction(pTableTran.get());
	embDB::CTable *pTable = pSchema->getTable("Table");


	CreateField("Field64", "AliasField64", embDB::dtInteger64, pTable);
	CreateField("FieldU64", "AliasFieldU64", embDB::dtUInteger64, pTable);
	CreateField("Field32", "AliasField32", embDB::dtInteger32, pTable);
	CreateField("FieldU32", "AliasFieldU32", embDB::dtUInteger32, pTable);
	CreateField("Field16", "AliasField16", embDB::dtInteger16, pTable);
	CreateField("FieldU16", "AliasFieldU16", embDB::dtUInteger16, pTable);
	CreateField("Field8", "AliasField8",	embDB::dtInteger8, pTable);
	CreateField("FieldU8", "AliasFieldU8",	embDB::dtUInteger8, pTable);
	CreateField("FieldDouble", "AliasDouble", embDB::dtDouble, pTable);
	CreateField("FieldFloat", "AliasFloat", embDB::dtFloat, pTable);

/*	for (int i = 0; i < 1000; ++i)
	{				
		sTableName.format(_T("table_%d"), i);
		pSchema->addTable(sTableName, "", 	(embDB::IDBTransactions*)pTableTran);
	}
	pTableTran->commit();
	pDb->closeTransaction(pTableTran);
	embDB::CTable *pTable = pSchema->getTable("table_1");

	embDB::ITransactions *pFieldTran = pDb->startTransaction(embDB::eTT_DDL);
	pFieldTran->begin();

	CommonLib::CString sFieldName; 
	for (int i = 0; i < 1000; ++i)
	{				

		sFieldName.format(_T("field_%d"), i);
		embDB::sFieldInfo fi;
		fi.m_sFieldName = sFieldName;
		fi.m_sFieldAlias = "test field 1";
		fi.m_nFieldType = embDB::dteSimple;
		fi.m_nFieldDataType = embDB::ftInteger32;
		pTable->addField(fi, (embDB::IDBTransactions*)pFieldTran);

		embDB::SIndexProp indexProp;
		indexProp.indexType = embDB::itMultiRegular;
		pTable->createIndex(sFieldName, indexProp);
	}
	pFieldTran->commit();
	pDb->closeTransaction(pFieldTran);*/
}
 
void CreateFields(embDB::CTable *pTable)
{

}