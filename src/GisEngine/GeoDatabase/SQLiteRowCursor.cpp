#include "stdafx.h"
#include "FieldSet.h"
#include "SQLiteRowCursor.h"
#include "sqlite3/sqlite3.h"
#include "Feature.h"
#include "CommonLibrary/MemoryStream.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "GisGeometry/Envelope.h"
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
				
					case dtBlob:
						{
							int nSize = sqlite3_column_bytes(m_pStmt, fieldIndex);
							if(!m_pCacheBlob.get())
							{
								m_pCacheBlob = new CommonLib::CBlob();
							}
							if(nSize == 0)
								m_pCacheBlob->resize(nSize);
							else 
							{
								m_pCacheBlob->copy((byte*)sqlite3_column_blob(m_pStmt, fieldIndex), nSize);
							}

							*pValue = CommonLib::IRefObjectPtr((IRefCnt*)m_pCacheBlob.get());
						}
						break;
					case dtGeometry:
					case dtAnnotation:
						{
							int nSize = sqlite3_column_bytes(m_pStmt, fieldIndex);
							if(!nSize)
							{
								m_pCacheShape->clear();
							}
							else
							{
								CommonLib::FxMemoryReadStream stream;
								stream.attach((byte*)sqlite3_column_blob(m_pStmt, fieldIndex), nSize);
								m_pCacheShape->read(&stream);
							}
						}
					case dtUInteger8:
						*pValue = (byte)sqlite3_column_int(m_pStmt, fieldIndex);//TO DO fix
						break;
					case dtInteger8:
						*pValue = (int8)sqlite3_column_int(m_pStmt, fieldIndex);
						break;
					case dtUInteger16:
						*pValue = (uint16)sqlite3_column_int(m_pStmt, fieldIndex);//TO DO fix
						break;
					case dtInteger32:
						*pValue = (int32)sqlite3_column_int(m_pStmt, fieldIndex);
						break;
					case dtUInteger32:
						*pValue = (uint32)sqlite3_column_int(m_pStmt, fieldIndex);//TO DO fix
						break;
					case dtInteger16:
						*pValue = (int16)sqlite3_column_int(m_pStmt, fieldIndex);
						break;
					case dtUInteger64:
						*pValue = (uint64)sqlite3_column_int64(m_pStmt, fieldIndex); //TO DO fix
						break;
					case dtInteger64:
						*pValue = sqlite3_column_int64(m_pStmt, fieldIndex); 
						break;
					case dtString:
						{
							CommonLib::CString str = (const char*)sqlite3_column_text(m_pStmt, fieldIndex);
							*pValue = str;
						}
						break;
				 }

			}
			*pRow = m_pCurrentRow;
			return true;
		}
	}
}