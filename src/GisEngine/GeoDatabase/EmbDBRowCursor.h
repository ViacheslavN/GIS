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
			CEmbDBRowCursor(IQueryFilter* filter, bool recycling, ITable *pTable, embDB::IConnection *pConnection);
			CEmbDBRowCursor(int64 nOId, IFieldSet *pFieldSet, ITable* pTable, embDB::IConnection *pConnection);

			virtual ~CEmbDBRowCursor();

		public:
			// IRowCursor
			virtual bool NextRow(IRowPtr* row);
		protected:
			CommonLib::IGeoShapePtr   m_pCacheShape;
			std::map<int, CommonLib::IBlobPtr> m_mapCacheBlob;
			embDB::IConnectionPtr				m_pConnection;
			embDB::ICursorPtr				m_pCursor;
			embDB::ITransactionPtr			m_pTran;
			//embDB::IRowPtr					m_pEmbDbRow;
			int m_nRecordCount;
			bool m_bInvalidCursor;

		};
	}
}

#endif