#include "stdafx.h"
#include "EmbDBFeatureClass.h"
#include "EmbDBWorkspace.h"
#include "GisGeometry/SpatialReferenceProj4.h"
#include "embDBRowCursor.h"
#include "GisGeometry/Envelope.h"
#include "GeometryDef.h"
#include "OIDSet.h"
#include "embDBUtils.h"
#include "../../EmbDB/EmbDB/embDBInternal.h"
#include "EmbDBRowCursor.h"
namespace GisEngine
{
	namespace GeoDatabase
	{


		CEmbDBFeatureClass::CEmbDBFeatureClass(CEmbDBWorkspace *pWorkspace, const CommonLib::CString& sName,  const CommonLib::CString& sViewName) :
	TBase((IWorkspace*)pWorkspace), m_pEmbDBWorkspace(pWorkspace), m_nOIDType(dtUnknown), m_bUseRowID(false)
	{
		m_sDatasetName = sName;
		m_sDatasetViewName = sViewName;
		m_sPropTableName = sViewName + L"_PROPERTIES";
	}

	CEmbDBFeatureClass::~CEmbDBFeatureClass()
	{

	}

	bool CEmbDBFeatureClass::CreateFeatureClass(IFields* pFields, bool bSaveFCProp)
	{

		embDB::IConnectionPtr pConnect = m_pEmbDBWorkspace->GetConnection();
		if(!pConnect)
			return false;


		CommonLib::CString sSQL;
		CommonLib::CString sOidField;
		CommonLib::CString sShapeField;
		CommonLib::CString sAnno;
		m_ShapeType = CommonLib::shape_type_null;
		GisGeometry::ISpatialReferencePtr pSPRef;


		embDBUtils::CreateTable(m_sDatasetName.cwstr(), pFields, pConnect.get(), &sOidField, &sShapeField, &sAnno, 	&m_ShapeType, &pSPRef);



		m_pFields = pFields->clone();
		m_sShapeFieldName = sShapeField;
		m_sAnnotationName = sAnno;
		m_sOIDFieldName = sOidField;
		 


		if(m_sShapeFieldName.isEmpty())
		{
			//TO DO error log
			return false;
		}


		if(m_ShapeType == CommonLib::shape_type_null)
		{
			//TO DO error log
			return false;
		}

		if(pSPRef.get())
			m_pSpatialReference = pSPRef->clone();



		if(!m_sOIDFieldName.isEmpty())
		{
			m_bHashOID = true;
			IFieldPtr pOIDField =  m_pFields->GetField(m_sOIDFieldName);
			assert(pOIDField.get());
			m_nOIDType = pOIDField->GetType();
		}
		else
		{
			m_bUseRowID = true;
			m_sOIDFieldName = L"RowID";
			m_nOIDType = dtUInteger64;
		}



		IShapeField* pShapeField = (IShapeField*)m_pFields->GetField(m_sShapeFieldName).get();
		assert(pShapeField);
		m_pExtent = new GisGeometry::CEnvelope(pShapeField->GetGeometryDef()->GetBaseExtent(), m_pSpatialReference.get());
		if(!bSaveFCProp)
			return true;

		return saveFeatureInfo();
	}
	bool CEmbDBFeatureClass::saveFeatureInfo()
	{

		if(m_sShapeFieldName.isEmpty())
			return false;
		if(m_ShapeType == CommonLib::shape_type_null)
			return false;

		embDB::IConnectionPtr  pConnection = m_pEmbDBWorkspace->GetConnection();
		if(!pConnection.get())
			return false;

		//TO DO Create from sql

		embDB::ISchemaPtr pSchema = pConnection->getSchema();

		embDB::ITablePtr pTable = pSchema->getTableByName(m_sPropTableName.cwstr());

		if(pTable.get())
		{
			embDB::ITransactionPtr pTran = pConnection->startTransaction(embDB::eTT_MODIFY);
			pTran->begin();
			embDB::IDeleteCursorPtr pDeleteCursor = pTran->createDeleteCursor(pTable->getName().cwstr());
			embDB::IRowPtr pRow;
			{
				embDB::ICursorPtr pSelectCursor = pTran->executeSelectQuery(pTable->getName().cwstr());
				while(pSelectCursor->NextRow(&pRow))
				{
					pDeleteCursor->remove(pRow.get());
				}
			}
			pTran->commit();
			
		}
		else
		{
			embDB::ITransactionPtr pTran = pConnection->startTransaction(embDB::eTT_DDL);
			pTran->begin();

			pSchema->addTable(m_sPropTableName.cwstr(), pTran.get());
			pTable = pSchema->getTableByName(m_sPropTableName.cwstr());
			embDB::IDBTable* pDBTable = (embDB::IDBTable*)pTable.get();

			{
				embDB::SFieldProp fp;
				fp.m_dataType = embDB::dtString;
				fp.m_sFieldName = L"PROJ";
				pDBTable->createField(fp, pTran.get());
			}
			{
				embDB::SFieldProp fp;
				fp.m_dataType = embDB::dtString;
				fp.m_sFieldName = L"SHAPEFIELD";
				fp.m_nLenField = 128;
				fp.m_bNotNull = true;
				pDBTable->createField(fp, pTran.get());
			}
			{
				embDB::SFieldProp fp;
				fp.m_dataType = embDB::dtInteger32;
				fp.m_sFieldName = L"GEOMTYPE";
				fp.m_bNotNull = true;
				pDBTable->createField(fp, pTran.get());
			}
			{
				embDB::SFieldProp fp;
				fp.m_dataType = embDB::dtString;
				fp.m_sFieldName = L"ANNO";
				fp.m_nLenField = 128;
				pDBTable->createField(fp, pTran.get());
			}
			{
				embDB::SFieldProp fp;
				fp.m_dataType = embDB::dtString;
				fp.m_sFieldName = L"OIDFIELD";
				pDBTable->createField(fp, pTran.get());
			}
			{
				embDB::SFieldProp fp;
				fp.m_dataType = embDB::dtInteger32;
				fp.m_sFieldName = L"OIDTYPE";
				pDBTable->createField(fp, pTran.get());
			}
			
			pTran->commit();
		}

		embDB::ITransactionPtr pTran = pConnection->startTransaction(embDB::eTT_MODIFY);
		pTran->begin();
		embDB::IInsertCursorPtr pCursor = pTran->createInsertCursor(pTable->getName().cwstr());

		embDB::IRowPtr pRow = pCursor->createRow();

		CommonLib::CVariant spRefVar(m_pSpatialReference.get() ? m_pSpatialReference->GetProjectionString() : CommonLib::CString(L""));
		CommonLib::CVariant varShapeField(m_sShapeFieldName);
		CommonLib::CVariant varShapeType((int)m_ShapeType);
		CommonLib::CVariant varAnnotation(m_sAnnotationName);
		CommonLib::CVariant varOIDField(m_bUseRowID ? L"" :  m_sOIDFieldName);
		CommonLib::CVariant varOIDType((int32)m_nOIDType);
		pRow->set(spRefVar, 0);
		pRow->set(varShapeField, 1);
		pRow->set(varShapeType, 2);
		pRow->set(varAnnotation, 3);
		pRow->set(varOIDField, 4);
		pRow->set(varOIDType, 5);
		pCursor->insert(pRow.get());
		return pTran->commit();
	}
	bool CEmbDBFeatureClass::open()
	{
		embDB::IConnectionPtr pConnection = m_pEmbDBWorkspace->GetConnection();
		if(!pConnection.get())
			return false;

		embDB::ISchemaPtr pSchema = pConnection->getSchema();
		if(!pSchema.get())
			return false;

		embDB::ITablePtr pTable = pSchema->getTableByName(m_sDatasetName.cwstr());
		if(!pTable.get())
			return false;

		m_pFields = embDBUtils::EmbDBFields2Fields(pTable->getFields().get());
		if(!m_pFields.get())
			return false;

		if(!m_pFields->GetFieldCount())
			return false;


		embDB::ITransactionPtr pTran =  pConnection->startTransaction(embDB::eTT_SELECT);
		pTran->begin();
		embDB::ICursorPtr pCursor = pTran->executeSelectQuery(m_sPropTableName.cwstr());
		embDB::IRowPtr pRow;
		pCursor->NextRow(&pRow);

		if(pRow.get())
		{
			CommonLib::CString sProjStr = pRow->value(0)->Get<CommonLib::CString>();
			m_sShapeFieldName =  pRow->value(1)->Get<CommonLib::CString>();
			m_ShapeType =  (CommonLib::eShapeType )pRow->value(2)->Get<int32>();
			m_sAnnotationName = pRow->value(3)->Get<CommonLib::CString>();
			m_sOIDFieldName = pRow->value(4)->Get<CommonLib::CString>();
			m_nOIDType   = (eDataTypes)pRow->value(5)->Get<int32>();
			m_pSpatialReference = new GisGeometry::CSpatialReferenceProj4(sProjStr);
			if(!m_pSpatialReference->IsValid())
				m_pSpatialReference = NULL;
		}
		else
		{
			return true;
		}


		 
		GisBoundingBox bounds;

		//TO DO temporaty for test

		
		embDB::IFieldPtr pEmbDBShapeField = pTable->getField(m_sShapeFieldName.cwstr());
		if(!pEmbDBShapeField.get())
			return false;

		embDB::IDBShapeFieldHandler *pShapeValueField = dynamic_cast<embDB::IDBShapeFieldHandler *>(pEmbDBShapeField.get());
		if(!pShapeValueField)
			return false;

	 
		
		bounds = pShapeValueField->GetBoundingBox();
		bounds.type = CommonLib::bbox_type_normal; //TO DO fix in embDB to save type	

		if(!m_pSpatialReference.get())
		{
			m_pSpatialReference = new GisGeometry::CSpatialReferenceProj4(m_sDatasetName + L".prj", GisGeometry::eSPRefTypePRJFilePath);
			if(!m_pSpatialReference->IsValid())
				m_pSpatialReference = NULL;
		}
		if(!m_pSpatialReference.get())
		{
			m_pSpatialReference = new GisGeometry::CSpatialReferenceProj4(bounds);
			if(!m_pSpatialReference->IsValid())
				m_pSpatialReference = NULL;
		}

		m_pExtent = new GisGeometry::CEnvelope(bounds, m_pSpatialReference.get());
		IShapeField* pShapeField = (IShapeField*)m_pFields->GetField(m_sShapeFieldName).get();
		assert(pShapeField);

		bool bHasZ = false, bHasM = false;
		CommonLib::CGeoShape::getTypeParams(m_ShapeType, NULL, &bHasZ, &bHasM, NULL, NULL);

		IGeometryDefPtr pGeomDef(new CGeometryDef());
		pGeomDef->SetSpatialReference(m_pSpatialReference.get());
		pGeomDef->SetBaseExtent(bounds);
		pGeomDef->SetGeometryType(m_ShapeType);
		pGeomDef->SetHasM(bHasM);
		pGeomDef->SetHasZ(bHasZ);
		pShapeField->SetGeometryDef(pGeomDef.get());
		pShapeField->SetType(dtGeometry);

		if(!m_sAnnotationName.isEmpty())
		{
			IFieldPtr pField = m_pFields->GetField(m_sShapeFieldName);
			assert(pField.get());
			pField->SetType(dtAnnotation);
		}
		if(!m_sOIDFieldName.isEmpty())
		{
			IFieldPtr pField = m_pFields->GetField(m_sOIDFieldName);
			assert(pField.get());
			assert(m_nOIDType == dtOid32 || m_nOIDType == dtOid64);
			pField->SetType(m_nOIDType);
			m_bHashOID = true;
		}

		return true;
	}
	bool CEmbDBFeatureClass::IsFeatureClass(embDB::ITable* pTable)
	{
		if(!pTable)
			return false;
		
		for (uint32 i = 0, sz = pTable->getFieldCnt(); i < sz; ++i)
		{
			embDB::IFieldPtr pField = pTable->getField(i);
			if(pField->getType() == embDB::dtGeometry)
				return true;

		}
		return false;
	}
	IRowPtr	CEmbDBFeatureClass::GetRow(int64 id)
	{

		CEmbDBRowCursor cursor(id,  NULL, this, m_pEmbDBWorkspace->GetConnection().get());
		IRowPtr pRow;
		cursor.NextRow(&pRow);
		return pRow;
	}
	ICursorPtr	CEmbDBFeatureClass::Search(IQueryFilter* filter, bool recycling)
	{
		return  ICursorPtr( (ICursor*)(new CEmbDBRowCursor(filter, recycling, this, m_pEmbDBWorkspace->GetConnection().get())) );
	}


	bool CEmbDBFeatureClass::save(CommonLib::IWriteStream *pWriteStream) const
	{
		return false;
	}
	bool CEmbDBFeatureClass::load(CommonLib::IReadStream* pReadStream)
	{
		return false;
	}

	bool CEmbDBFeatureClass::saveXML(GisCommon::IXMLNode* pXmlNode) const
	{
		return false;
	}
	bool CEmbDBFeatureClass::load(const GisCommon::IXMLNode* pXmlNode)
	{
		return false;
	}
	}
}
