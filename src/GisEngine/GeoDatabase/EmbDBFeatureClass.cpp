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

	bool CEmbDBFeatureClass::CreateFeatureClass(IFields* pFields)
	{

		embDB::IDatabasePtr pDB = m_pEmbDBWorkspace->GetDB();
		if(!pDB)
			return false;


		CommonLib::CString sSQL;
		CommonLib::CString sOidField;
		CommonLib::CString sShapeField;
		CommonLib::CString sAnno;
		m_ShapeType = CommonLib::shape_type_null;
		GisGeometry::ISpatialReferencePtr pSPRef;


		embDBUtils::CreateTable(m_sDatasetName.cwstr(), pFields, pDB.get(), &sOidField, &sShapeField, &sAnno, 	&m_ShapeType, &pSPRef);



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
		return saveFeatureInfo();
	}
	bool CEmbDBFeatureClass::saveFeatureInfo()
	{

		if(m_sShapeFieldName.isEmpty())
			return false;
		if(m_ShapeType == CommonLib::shape_type_null)
			return false;

		embDB::IDatabasePtr pDB = m_pEmbDBWorkspace->GetDB();
		if(!pDB.get())
			return false;

		//TO DO Create from sql

		embDB::ISchemaPtr pSchema = pDB->getSchema();

		embDB::ITablePtr pTable = pSchema->getTableByName(m_sPropTableName.cwstr());

		if(pTable.get())
		{
			embDB::ITransactionPtr pTran = pDB->startTransaction(embDB::eTT_DELETE);
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
			pSchema->addTable(m_sPropTableName.cwstr());
			pTable = pSchema->getTableByName(m_sPropTableName.cwstr());
			embDB::IDBTable* pDBTable = (embDB::IDBTable*)pTable.get();

			{
				embDB::SFieldProp fp;
				fp.dataType = embDB::dtString;
				fp.sFieldName = L"PROJ";
				pDBTable->createField(fp);
			}
			{
				embDB::SFieldProp fp;
				fp.dataType = embDB::dtString;
				fp.sFieldName = L"SHAPEFIELD";
				fp.nLenField = 255;
				fp.dateTypeExt |= embDB::dteIsNotEmpty;
				pDBTable->createField(fp);
			}
			{
				embDB::SFieldProp fp;
				fp.dataType = embDB::dtInteger32;
				fp.sFieldName = L"GEOMTYPE";
				fp.nLenField = 255;
				fp.dateTypeExt |= embDB::dteIsNotEmpty;
				pDBTable->createField(fp);
			}
			{
				embDB::SFieldProp fp;
				fp.dataType = embDB::dtString;
				fp.sFieldName = L"ANNO";
				fp.nLenField = 255;
				pDBTable->createField(fp);
			}
			{
				embDB::SFieldProp fp;
				fp.dataType = embDB::dtString;
				fp.sFieldName = L"OIDFIELD";
				fp.nLenField = 255;
				pDBTable->createField(fp);
			}
			{
				embDB::SFieldProp fp;
				fp.dataType = embDB::dtInteger32;
				fp.sFieldName = L"OIDTYPE";
				pDBTable->createField(fp);
			}
			
		}

		embDB::ITransactionPtr pTran = pDB->startTransaction(embDB::eTT_INSERT);
		pTran->begin();
		embDB::IInsertCursorPtr pCursor = pTran->createInsertCursor(pTable->getName().cwstr());

		embDB::IRowPtr pRow = pCursor->createRow();


		pRow->set(CommonLib::CVariant(m_pSpatialReference.get() ? m_pSpatialReference->GetProjectionString().cwstr() : L""), 0);
		pRow->set(CommonLib::CVariant(m_sShapeFieldName), 1);
		pRow->set(CommonLib::CVariant((int)m_ShapeType), 2);
		pRow->set(CommonLib::CVariant(m_sAnnotationName), 3);
		pRow->set(CommonLib::CVariant(m_bUseRowID ? L"" : m_sOIDFieldName), 4);
		pRow->set(CommonLib::CVariant(m_nOIDType), 5);
		pCursor->insert(pRow.get());
		return pTran->commit();
	}
	bool CEmbDBFeatureClass::open()
	{
		embDB::IDatabasePtr pDB = m_pEmbDBWorkspace->GetDB();
		if(!pDB.get())
			return false;

		embDB::ISchemaPtr pSchema = pDB->getSchema();
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


		embDB::ITransactionPtr pTran =  pDB->startTransaction(embDB::eTT_SELECT);
		pTran->begin();
		embDB::ICursorPtr pCursor = pTran->executeSelectQuery(m_sDatasetName.cwstr());
		embDB::IRowPtr pRow;
		pCursor->NextRow(&pRow);

		if(pRow.get())
		{
			CommonLib::CString sProjStr = pRow->value(0)->Get<CommonLib::CString>();
			m_sShapeFieldName =  pRow->value(1)->Get<CommonLib::CString>();
			m_ShapeType =  (CommonLib::eShapeType )pRow->value(2)->Get<int32>();
			m_sAnnotationName = pRow->value(3)->Get<CommonLib::CString>();
			m_sOIDFieldName = pRow->value(4)->Get<CommonLib::CString>();
			m_nOIDType   = (eDataTypes)pRow->value(4)->Get<int32>();
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

		embDB::IValueField *pShapeValueField = dynamic_cast<embDB::IValueField *>(pEmbDBShapeField.get());
		if(!pShapeValueField)
			return false;

		const embDB::sSpatialFieldInfo *pSpatInfo  = dynamic_cast<const embDB::sSpatialFieldInfo *>(pShapeValueField->getFieldInfoType());
		if(!pSpatInfo)
			return false;

		bounds = pSpatInfo->m_extent;
			

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
	bool CEmbDBFeatureClass::IsFeatureClass(CommonLib::CString& sName, SQLiteUtils::CSQLiteDB *pDB)
	{
		if(!pDB)
			return false;

		return pDB->IsTableExist(sName + "_SpatialIndex");
	}
	IRowPtr	CEmbDBFeatureClass::GetRow(int64 id)
	{

		CSQLiteRowCursor cursor(id,  NULL, this, m_pSQLiteWorkspace->GetDB());
		IRowPtr pRow;
		cursor.NextRow(&pRow);
		return pRow;
	}
	ICursorPtr	CEmbDBFeatureClass::Search(IQueryFilter* filter, bool recycling)
	{
		return  ICursorPtr(new CSQLiteRowCursor(filter, recycling, this, m_pSQLiteWorkspace->GetDB()));
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
