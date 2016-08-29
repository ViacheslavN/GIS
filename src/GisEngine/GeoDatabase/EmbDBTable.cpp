#include "stdafx.h"
#include "EmbDBTable.h"
#include "EmbDBWorkspace.h"
#include "embDBUtils.h"
#include "../../EmbDB/EmbDB/embDBInternal.h"

namespace GisEngine
{
	namespace GeoDatabase
	{
		CEmbDBTable::CEmbDBTable(CEmbDBWorkspace *pWorkspace, const CommonLib::CString& sName,  const CommonLib::CString& sViewName):
			TBase(pWorkspace), m_pEmbDBWorkspace(pWorkspace)
		{
			m_sDatasetName = sName;
			m_sDatasetViewName = sViewName;
		}
		CEmbDBTable::~CEmbDBTable()
		{

		}

		IRowPtr		CEmbDBTable::GetRow(int64 id)
		{
			return IRowPtr();
		}
		ICursorPtr	CEmbDBTable::Search(IQueryFilter* filter, bool recycling)
		{
			return ICursorPtr();
		}
		bool CEmbDBTable::load()
		{
			return true;
		}

		bool CEmbDBTable::save(CommonLib::IWriteStream *pWriteStream) const
		{
			return false;
		}
		bool CEmbDBTable::load(CommonLib::IReadStream* pReadStream)
		{
			return false;
		}

		bool CEmbDBTable::saveXML(GisCommon::IXMLNode* pXmlNode) const
		{
			return false;
		}
		bool CEmbDBTable::load(const GisCommon::IXMLNode* pXmlNode)
		{
			return true;
		}

		bool CEmbDBTable::open()
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
			return true;
		}
		bool CEmbDBTable::CreateTable(IFields* pFields)
		{
			embDB::IConnectionPtr pConn = m_pEmbDBWorkspace->GetConnection();
			if(!pConn)
				return false;


			CommonLib::CString sSQL;
			CommonLib::CString sOidField;
			GisGeometry::ISpatialReferencePtr pSPRef;


			if(!embDBUtils::CreateTable(m_sDatasetName.cwstr(), pFields, pConn.get(), &sOidField))
				return false;

			m_pFields = pFields->clone();
			m_sOIDFieldName = sOidField;

			return true;
		}
	}
}
