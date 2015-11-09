#ifndef GIS_ENGINE_GEO_DATABASE_EMBDB_ROW_CURSOR_H
#define GIS_ENGINE_GEO_DATABASE_EMBDB_ROW_CURSOR_H

#include "GeoDatabase.h"
#include "CursorBase.h"
#include "../../EmbDB/EmbDB/embDB.h"

namespace GisEngine
{
	namespace GeoDatabase
	{
		class CEmbDBRowCursor : ICursorBase<ICursor>
		{
		public:

			typedef ICursorBase<ICursor> TBase;
			CEmbDBRowCursor(IQueryFilter* filter, bool recycling, ITable *pTable, embDB::IDatabase *pDB);
			CEmbDBRowCursor(int64 nOId, IFieldSet *pFieldSet, ITable* pTable, embDB::IDatabase *pDB);

			virtual ~CEmbDBRowCursor();

		public:
			// IRowCursor
			virtual bool NextRow(IRowPtr* row);
		protected:
			CommonLib::IGeoShapePtr   m_pCacheShape;
			std::map<int, CommonLib::IBlobPtr> m_mapCacheBlob;
			embDB::IDatabasePtr				m_pDB;
			embDB::ICursorPtr				m_pCursor;
			int m_nRecordCount;
			bool m_bInvalidCursor;

		};
	}
}

#endif