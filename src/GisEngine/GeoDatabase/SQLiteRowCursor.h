#ifndef GIS_ENGINE_GEO_DATABASE_SQLITE_ROW_CURSOR_H
#define GIS_ENGINE_GEO_DATABASE_SQLITE_ROW_CURSOR_H

#include "GeoDatabase.h"
#include "CursorBase.h"


struct sqlite3;
struct sqlite3_stmt;
namespace GisEngine
{
	namespace GeoDatabase
	{
		class CSQLiteRowCursor : ICursorBase<ICursor>
		{
		public:

			typedef ICursorBase<ICursor> TBase;
			CSQLiteRowCursor(IQueryFilter* filter, bool recycling, ITable *pTable, sqlite3 *pConn, sqlite3_stmt *pStmt);
			virtual ~CSQLiteRowCursor();

		public:
			// IRowCursor
			virtual bool NextRow(IRowPtr* row);

		protected:
			CommonLib::IGeoShapePtr   m_pCacheShape;
			CommonLib::IBlobPtr		  m_pCacheBlob;
			int m_nRecordCount;
			bool m_bInvalidCursor;
			sqlite3 *m_pConn;
			sqlite3_stmt *m_pStmt;
		};
	}
}

#endif