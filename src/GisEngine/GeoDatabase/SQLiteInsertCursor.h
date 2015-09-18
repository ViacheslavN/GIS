#ifndef GIS_ENGINE_GEO_DATABASE_SQLITE_INSERT_CURSOR_BASE_H
#define GIS_ENGINE_GEO_DATABASE_SQLITE_INSERT_CURSOR_BASE_H
#include "InsertCursorBase.h"

struct sqlite3;
struct sqlite3_stmt;
namespace GisEngine
{
	namespace GeoDatabase
	{
			class SQLiteInsertCursor : public IInsertCursorBase<IInsertCursor>
			{
				public:
					typedef IInsertCursorBase<IInsertCursor> TBase;

					SQLiteInsertCursor(ITable* pTable, IFieldSet *pFileds, sqlite3* pConn);
					virtual ~SQLiteInsertCursor();

					virtual int64 InsertRow(IRow* pRow);
				private:
					void init();
					void close();
				private:

					struct SFieldInfo
					{
						SFieldInfo(int nCol, eDataTypes type) : m_nCol(nCol), m_type(type)
						{}
						int m_nCol;
						eDataTypes m_type;
					};
					typedef std::map<CommonLib::CString, SFieldInfo> TFieldsInfo;
					TFieldsInfo m_mapFieldInfo;
					sqlite3* m_pConn;
					sqlite3_stmt*	m_pStmt;
					bool m_bValidCursor;
					bool m_bInit;
					CommonLib::CString m_sErrorMessage;
			};
	}
}

#endif