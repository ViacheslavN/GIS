#ifndef GIS_ENGINE_GEO_DATABASE_SQLITE_ROW_CURSOR_H
#define GIS_ENGINE_GEO_DATABASE_SQLITE_ROW_CURSOR_H

#include "GeoDatabase.h"
#include "CursorBase.h"
#include "SQLiteDB.h"
 
namespace GisEngine
{
	namespace GeoDatabase
	{
		class CSQLiteRowCursor : ICursorBase<ICursor>
		{
		public:

			typedef ICursorBase<ICursor> TBase;
			CSQLiteRowCursor(IQueryFilter* filter, bool recycling, ITable *pTable, SQLiteUtils::CSQLiteDB* pDB);
			CSQLiteRowCursor(int64 nOId, IFieldSet *pFieldSet, ITable* pTable, SQLiteUtils::CSQLiteDB* pDB);
		 
			virtual ~CSQLiteRowCursor();

		public:
			// IRowCursor
			virtual bool NextRow(IRowPtr* row);

		protected:
			CommonLib::IGeoShapePtr   m_pCacheShape;
			std::map<int, CommonLib::IBlobPtr> m_mapCacheBlob;
			//CommonLib::IBlobPtr		  m_pCacheBlob;
			SQLiteUtils::CSQLiteDB*				  m_pDB;
			SQLiteUtils::CSQLiteResultSetPtr	  m_pStmt;
			int m_nRecordCount;
			bool m_bInvalidCursor;
		 
		};
	}
}

#endif