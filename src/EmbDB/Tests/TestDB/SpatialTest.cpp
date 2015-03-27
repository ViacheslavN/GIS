#include "stdafx.h"


#include "../../EmbDB/Database.h"
#include "../../EmbDB/VariantField.h"
#include "../../EmbDB/IField.h"
#include "../../EmbDB/Table.h"
#include "../../EmbDB/ValueField.h"
#include "../../EmbDB/Transactions.h"
#include "../../EmbDB/SpatialVariant.h"
#include "../../EmbDB/SpatialOIDField.h"


 




template<class TPointType, class TRectVariant, class TSpatialField>
void CreateSpatialRectField(const CommonLib::str_t& sFieldName, embDB::CDatabase& db, int nType, 
	embDB::CTable *pTable, TPointType xMin, TPointType yMin, TPointType xMax,  TPointType yMax, TPointType nStep)
{
	embDB::ITransactions *pFieldTran = db.startTransaction(embDB::eTT_DDL);
	pFieldTran->begin();


	embDB::sFieldInfo fi;
	fi.m_sFieldName = sFieldName;
	fi.m_nFieldType =0;// embDB::eShapeType;
	fi.m_nFieldDataType = nType;
	fi.m_Extent.m_minX = xMin;
	fi.m_Extent.m_minY = yMin;
	fi.m_Extent.m_maxX = xMax;
	fi.m_Extent.m_maxY = yMax;
	pTable->addField(fi, (embDB::IDBTransactions*)pFieldTran);
 

	pFieldTran->commit();
	db.closeTransaction(pFieldTran);


	typedef embDB::TRect2D<TPointType> TRect;

	embDB::ITransactions *pInsertTran = db.startTransaction(embDB::eTT_INSERT);
	pInsertTran->begin();

 
	
	TSpatialField *pField = (TSpatialField*)pTable->getField(sFieldName);
	TSpatialField::TOIDSpatialField* pOIDField = pField->getSpatialOIDField((embDB::IDBTransactions*)pInsertTran, (embDB::IDBStorage*)db.getMainStorage());

	uint64 nEndX = xMax - xMin;
	uint64 nEndY = yMax - yMin;

	TRect feature;
	uint64 nOID = 1;
	for (uint64 x = xMin; x < nEndX - nStep; x +=nStep)
	{
		for (uint64 y = yMin; y < nEndY - nStep; y +=nStep)
		{
			TRectVariant variant((TPointType)x, (TPointType)y, (TPointType)x + (TPointType)nStep, (TPointType)y + (TPointType)nStep);
			pOIDField->insert(&variant, nOID);
			++nOID;
		}

	}

	pInsertTran->commit();
	db.closeTransaction(pInsertTran);

}

void TestSpatialField(embDB::CDatabase& db)
{

	/*embDB::CSchema* pSchema = db.getSchema();
	embDB::CTable *pTable = pSchema->getTable("table_2");
	embDB::IDBFieldHandler *pRectField = pTable->getField(_T("rect16spatialField"));
	if(!pRectField)
	{
		CreateSpatialRectField<uint16, embDB::TVariantShapeField16, embDB::TRect16Field>("rect16spatialField", db, embDB::ftShape16, pTable, 0, 0, 0xFFFF, 0xFFFF, 1000 );
	}
	else
	{

	}*/
}