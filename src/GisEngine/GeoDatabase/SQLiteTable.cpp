#include "stdafx.h"
#include "SQLiteTable.h"
#include "SQLiteWorkspace.h"
#include "SQLiteUtils.h"
#include "SQLIteDB.h"

namespace GisEngine
{
	namespace GeoDatabase
	{
 
		CSQLiteTable::CSQLiteTable(CSQLiteWorkspace *pWorkspace, const CommonLib::CString& sName,  const CommonLib::CString& sViewName) :
			TBase(pWorkspace), m_pSQLiteWorkspace(pWorkspace)
		{
			m_sDatasetName = sName;
			m_sDatasetViewName = sViewName;
		}
		CSQLiteTable::~CSQLiteTable()
		{

		}

		IRowPtr		CSQLiteTable::GetRow(int64 id)
		{
			return IRowPtr();
		}
		ICursorPtr	CSQLiteTable::Search(IQueryFilter* filter, bool recycling)
		{
			return ICursorPtr();
		}

		bool CSQLiteTable::load()
		{
			
			return false;
		}
		bool CSQLiteTable::save(CommonLib::IWriteStream *pWriteStream) const
		{
			return false;
		}
		bool CSQLiteTable::load(CommonLib::IReadStream* pReadStream)
		{
			return false;
		}

		bool CSQLiteTable::saveXML(GisCommon::IXMLNode* pXmlNode) const
		{
			return false;
		}
		bool CSQLiteTable::load(const GisCommon::IXMLNode* pXmlNode)
		{
			return false;
		}

		bool CSQLiteTable::CreateTable(IFields* pFields)
		{
			SQLiteUtils::CSQLiteDB* pDB = m_pSQLiteWorkspace->GetDB();
			if(!pDB)
				return false;


			CommonLib::CString sSQL;
			CommonLib::CString sOidField;
			SQLiteUtils::CreateSQLCreateTable(pFields, m_sDatasetName, sSQL,
				&sOidField);

			m_pFields = pFields->clone();
			m_sOIDFieldName = sOidField;


			return pDB->execute(sSQL);
		}

		bool CSQLiteTable::open()
		{

			SQLiteUtils::CSQLiteDB *pDB = m_pSQLiteWorkspace->GetDB();
			if(!pDB)
				return false;

			m_pFields = pDB->ReadFields(m_sDatasetName);
			if(!m_pFields.get())
				return false;

			if(!m_pFields->GetFieldCount())
				return false;

			return true;
		}

	}
}
