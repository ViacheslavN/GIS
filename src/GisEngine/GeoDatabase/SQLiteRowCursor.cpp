#include "stdafx.h"
#include "SQLiteRowCursor.h"
#include "sqlite3/sqlite3.h"
#include "Feature.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
		CSQLiteRowCursor::CSQLiteRowCursor(IQueryFilter* pFilter, bool bRecycling, ITable *pTable, sqlite3 *pConn, sqlite3_stmt *pStmt) :
			TBase(pFilter, bRecycling, pTable), m_pConn(pConn), m_pStmt(pStmt)
		{

		}
		CSQLiteRowCursor::~CSQLiteRowCursor()
		{

		}

		// IRowCursor
		bool CSQLiteRowCursor::NextRow(IRowPtr* pRow)
		{
			if(sqlite3_step(m_pStmt) != SQLITE_ROW)
				return false;


			if(!m_pCurrentRow.get())
			{
				m_pCurrentRow = new  CFeature(m_pFilter->GetFieldSet().get(), m_pSourceFields.get());
				if(m_nShapeFieldIndex >= 0 && IsFieldSelected(m_nShapeFieldIndex))
				{
					IFeature* feature = (IFeature*)(m_pCurrentRow.get());
					if(feature)
					{
						m_pCacheShape = new CommonLib::CGeoShape();
						feature->SetShape(m_pCacheShape.get());
					}
				}
			}


			for(int i = 0; i < (int)m_vecActualFieldsIndexes.size(); ++i)
			{
				int fieldIndex = m_vecActualFieldsIndexes[i];
				CommonLib::CVariant* pValue = m_pCurrentRow->GetValue(fieldIndex);
				 switch(m_vecActualFieldsTypes[i])
				 {
					case dtGeometry:
					case dtBlob:
						{
							int nByte = sqlite3_column_bytes(m_pStmt, fieldIndex);
						}
						break;
				 }

			}
			*pRow = m_pCurrentRow;
			return true;
		}
	}
}