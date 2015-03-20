#include "stdafx.h"
#include "CreateFields.h"


void CreateField(const CommonLib::str_t& sName, const CommonLib::str_t& sAlias,  uint32 nFieldType,
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
	CommonLib::str_t sTableName; 
	embDB::ITransactions *pTableTran = pDb->startTransaction(embDB::eTT_DDL);
	pTableTran->begin();
	pSchema->addTable("Table", "", 	(embDB::IDBTransactions*)pTableTran);
	pTableTran->commit();
	pDb->closeTransaction(pTableTran);
	embDB::CTable *pTable = pSchema->getTable("Table");


	CreateField("Field64", "AliasField64", embDB::ftInteger64, pTable);
	CreateField("FieldU64", "AliasFieldU64", embDB::ftUInteger64, pTable);
	CreateField("Field32", "AliasField32", embDB::ftInteger32, pTable);
	CreateField("FieldU32", "AliasFieldU32", embDB::ftUInteger32, pTable);
	CreateField("Field16", "AliasField16", embDB::ftInteger16, pTable);
	CreateField("FieldU16", "AliasFieldU16", embDB::ftUInteger16, pTable);
	CreateField("Field8", "AliasField8",	embDB::ftInteger8, pTable);
	CreateField("FieldU8", "AliasFieldU8",	embDB::ftUInteger8, pTable);
	CreateField("FieldDouble", "AliasDouble", embDB::ftDouble, pTable);
	CreateField("FieldFloat", "AliasFloat", embDB::ftFloat, pTable);

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

	CommonLib::str_t sFieldName; 
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