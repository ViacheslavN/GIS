#include "stdafx.h"
#include "SQLiteTable.h"
#include "SQLiteWorkspace.h"


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
			sqlite3 *pConn = m_pSQLiteWorkspace->GetConnections();
			if(!pConn)
				return false;

			return false;
		}
	}
}
