#include "stdafx.h"
#include "SQLiteFeatureClass.h"
#include "SQLiteDB.h"
#include "SQLiteWorkspace.h"
#include "GisGeometry/SpatialReferenceProj4.h"
#include "SQLiteRowCursor.h"
#include "GisGeometry/Envelope.h"
#include "SQLiteUtils.h"
#include "GeometryDef.h"
#include "OIDSet.h"
namespace GisEngine
{
	namespace GeoDatabase
	{


		CSQLiteFeatureClass::CSQLiteFeatureClass(CSQLiteWorkspace *pWorkspace, const CommonLib::CString& sName,  const CommonLib::CString& sViewName) :
			TBase((IWorkspace*)pWorkspace), m_pSQLiteWorkspace(pWorkspace), m_nOIDType(dtUnknown)
		{
				m_sDatasetName = sName;
				m_sDatasetViewName = sViewName;
				m_sSpatialIndexName = sViewName + "_SpatialIndex";
				m_sPropTableName = sViewName + L"_PROPERTIES";
		}
 
		CSQLiteFeatureClass::~CSQLiteFeatureClass()
		{

		}

		bool CSQLiteFeatureClass::CreateFeatureClass(IFields* pFields)
		{

			SQLiteUtils::CSQLiteDB* pDB = m_pSQLiteWorkspace->GetDB();
			if(!pDB)
				return false;


			CommonLib::CString sSQL;
			CommonLib::CString sOidField;
			CommonLib::CString sShapeField;
			CommonLib::CString sAnno;
			m_ShapeType = CommonLib::shape_type_null;
			GisGeometry::ISpatialReferencePtr pSPRef;
		 

			SQLiteUtils::CreateSQLCreateTable(pFields, m_sDatasetName, sSQL,
				 &sOidField, &sShapeField, &sAnno, 	&m_ShapeType, &pSPRef);



			m_pFields = pFields->clone();
			m_sShapeFieldName = sShapeField;
			m_sAnnotationName = sAnno;
			m_sOIDFieldName = sOidField;


			if(m_sShapeFieldName.isEmpty())
			{
				pDB->SetErrorText(L"empty shape field name");
				return false;
			}


			if(m_ShapeType == CommonLib::shape_type_null)
			{
				pDB->SetErrorText(L"null shape type");
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
		 


			IShapeField* pShapeField = (IShapeField*)m_pFields->GetField(m_sShapeFieldName).get();
			assert(pShapeField);
			m_pExtent = new GisGeometry::CEnvelope(pShapeField->GetGeometryDef()->GetBaseExtent(), m_pSpatialReference.get());


			if(!pDB->execute(sSQL))
				return  false;

			CommonLib::CString sRTreeSQL;
			sRTreeSQL.format(L"CREATE VIRTUAL TABLE %s USING rtree(feature_id, minX, maxX, minY, maxY)", m_sSpatialIndexName.cwstr());

			if(!pDB->execute(sRTreeSQL))
				return  false;


			return saveFeatureInfo();

		
			
			
			return true;
		}
		bool CSQLiteFeatureClass::saveFeatureInfo()
		{

			if(m_sShapeFieldName.isEmpty())
				return false;
			if(m_ShapeType == CommonLib::shape_type_null)
				return false;

			SQLiteUtils::CSQLiteDB* pDB = m_pSQLiteWorkspace->GetDB();
			if(!pDB)
				return false;

			CommonLib::CString sSQL;

			
			sSQL.format(L"SELECT name FROM sqlite_master WHERE type='table' AND name='%s'", m_sPropTableName.cwstr());
			SQLiteUtils::TSQLiteResultSetPtr pRS = pDB->prepare_query(sSQL);
			if(!pRS->IsError() && pRS->StepNext())
			{
				sSQL.format(L"delete from %s", m_sPropTableName.cwstr());

				if(!pDB->execute(sSQL))
					return  false;
			}
			else
			{
			
				sSQL.format(L"CREATE TABLE %s (PROJ TEXT, SHAPEFIELD TEXT NOT NULL, GEOMTYPE INTEGER NOT NULL, ANNO TEXT, OIDFIELD TEXT, OIDTYPE INTEGER)", m_sPropTableName.cwstr());
				if(!pDB->execute(sSQL))
					return  false;
			}

		

			sSQL.format(L"INSERT INTO %s (PROJ, SHAPEFIELD, GEOMTYPE, ANNO, OIDFIELD, OIDTYPE) VALUES('%s', '%s', %d, '%s', '%s', %d)", m_sPropTableName.cwstr(),
				m_pSpatialReference.get() ? m_pSpatialReference->GetProjectionString().cwstr() : L"",	m_sShapeFieldName.cwstr(), (int)m_ShapeType, m_sAnnotationName.cwstr(), m_sOIDFieldName.cwstr(), m_nOIDType);
			if(!pDB->execute(sSQL))
				return  false;
			
			return true;
		}
		bool CSQLiteFeatureClass::open()
		{
			SQLiteUtils::CSQLiteDB *pDB = m_pSQLiteWorkspace->GetDB();
			if(!pDB)
				return false;

	

			m_pFields = pDB->ReadFields(m_sDatasetName);
			if(!m_pFields.get())
				return false;

			if(!m_pFields->GetFieldCount())
				return false;
		
			CommonLib::CString sBBSql;
			sBBSql.format(L"SELECT MIN(minX), MIN(minY), MAX(maxX), MAX(maxY) FROM %s", m_sSpatialIndexName.cwstr());

			SQLiteUtils::TSQLiteResultSetPtr pRS_BB =  pDB->prepare_query(sBBSql);
			GisBoundingBox bounds;
			if(pRS_BB.get())
			{
				if(pRS_BB->StepNext())
				{
					bounds.type = CommonLib::bbox_type_normal;
					bounds.xMin = pRS_BB->ColumnDouble(0);
					bounds.yMin = pRS_BB->ColumnDouble(1);
					bounds.xMax = pRS_BB->ColumnDouble(2);
					bounds.yMax = pRS_BB->ColumnDouble(3);
				}
			}
			 

			CommonLib::CString sSPropSQL;
			sSPropSQL.format(L"SELECT PROJ, SHAPEFIELD, GEOMTYPE, ANNO, OIDFIELD, OIDTYPE from %s  limit 1", m_sPropTableName.cwstr());
			SQLiteUtils::TSQLiteResultSetPtr pRS_Prop =  pDB->prepare_query(sSPropSQL);

			
			if(pRS_Prop.get())
			{	
			 
				if (pRS_Prop->StepNext()) 
				{			 
					CommonLib::CString sProjStr = (char*)pRS_Prop->ColumnText(0);
					m_sShapeFieldName = (char*)pRS_Prop->ColumnText(1);
					m_ShapeType = (CommonLib::eShapeType)pRS_Prop->ColumnInt(2);
					m_sAnnotationName = (char*)pRS_Prop->ColumnText(3);
					m_sOIDFieldName  = (char*)pRS_Prop->ColumnText(4); 
					m_nOIDType   = (eDataTypes)pRS_Prop->ColumnInt(5);
					m_pSpatialReference = new GisGeometry::CSpatialReferenceProj4(sProjStr);
					if(!m_pSpatialReference->IsValid())
						m_pSpatialReference = NULL;

				}
				
			}
			else
			{
				return true;
			}

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
		bool CSQLiteFeatureClass::IsFeatureClass(CommonLib::CString& sName, SQLiteUtils::CSQLiteDB *pDB)
		{
			if(!pDB)
				return false;
		
			return pDB->IsTableExist(sName + "_SpatialIndex");
		}
		IRowPtr	CSQLiteFeatureClass::GetRow(int64 id)
		{

			CSQLiteRowCursor cursor(id,  NULL, this, m_pSQLiteWorkspace->GetDB());
			IRowPtr pRow;
			cursor.NextRow(&pRow);
			return pRow;
		}
		ICursorPtr	CSQLiteFeatureClass::Search(IQueryFilter* filter, bool recycling)
		{
			return  ICursorPtr(new CSQLiteRowCursor(filter, recycling, this, m_pSQLiteWorkspace->GetDB()));
		}


		bool CSQLiteFeatureClass::save(CommonLib::IWriteStream *pWriteStream) const
		{
			return false;
		}
		bool CSQLiteFeatureClass::load(CommonLib::IReadStream* pReadStream)
		{
			return false;
		}

		bool CSQLiteFeatureClass::saveXML(GisCommon::IXMLNode* pXmlNode) const
		{
			return false;
		}
		bool CSQLiteFeatureClass::load(const GisCommon::IXMLNode* pXmlNode)
		{
			return false;
		}
	}
}
