#include "stdafx.h"
#include "SQLiteFeatureClass.h"
#include "SQLiteDB.h"
#include "SQLiteWorkspace.h"
#include "GisGeometry/SpatialReferenceProj4.h"
#include "SQLiteRowCursor.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
		CSQLiteFeatureClass::CSQLiteFeatureClass(CSQLiteWorkspace *pWorkspace, const CommonLib::CString& sName,  const CommonLib::CString& sViewName,
			const CommonLib::CString& SpatialIndexName) :
			TBase((IWorkspace*)pWorkspace), m_pSQLiteWorkspace(pWorkspace)
		{
				m_sDatasetName = sName;
				m_sDatasetViewName = sViewName;
				m_sSpatialIndexName = SpatialIndexName;
		}
 
		CSQLiteFeatureClass::~CSQLiteFeatureClass()
		{

		}
		bool CSQLiteFeatureClass::open()
		{
			SQLiteUtils::CSQLiteDB *pDB = m_pSQLiteWorkspace->GetDB();
			if(!pDB)
				return false;

	
		
			CommonLib::CString sBBSql;
			sBBSql.format(L"SELECT MIN(minX), MIN(minY), MAX(maxX), MAX(maxY) FROM %s", m_sSpatialIndexName.cwstr());

			SQLiteUtils::TSQLiteResultSetPtr pRS_BB =  pDB->prepare_query(sBBSql);
			GisBoundingBox bounds;
			if(pRS_BB.get())
			{
				if(pRS_BB->StepNext())
				{
					bounds.xMin = pRS_BB->ColumnDouble(1);
					bounds.yMin = pRS_BB->ColumnDouble(2);
					bounds.xMax = pRS_BB->ColumnDouble(3);
					bounds.yMax = pRS_BB->ColumnDouble(4);
				}
			}
			 

			CommonLib::CString sSPSQL;
			sSPSQL.format(L"SELECT PROJ from %s_PROJ limit 1", m_sDatasetName.cwstr());
			SQLiteUtils::TSQLiteResultSetPtr pRS_SPRef =  pDB->prepare_query(sSPSQL);
			if(pRS_SPRef.get())
			{	
			 
				if (pRS_SPRef->StepNext()) 
				{
			
			 
					CommonLib::CString sProjStr = (char*)pRS_SPRef->ColumnText(0);
					m_pSpatialReference = new GisGeometry::CSpatialReferenceProj4(sProjStr);

				}
				
			}
				

			return true;
		}
		IRowPtr	CSQLiteFeatureClass::GetRow(int64 id)
		{
			return IRowPtr();
		}
		ICursorPtr	CSQLiteFeatureClass::Search(IQueryFilter* filter, bool recycling)
		{
			return  ICursorPtr(new CSQLiteRowCursor(filter, recycling, this));
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
		bool CSQLiteFeatureClass::load(GisCommon::IXMLNode* pXmlNode)
		{
			return false;
		}
	}
}
