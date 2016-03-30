#ifndef GIS_ENGINE_GEO_DATABASE_SQLITE_RESULT_SET_H_
#define GIS_ENGINE_GEO_DATABASE_SQLITE_RESULT_SET_H_


#include "GeoDatabase.h"
#include "CommonLibrary/String.h"
#include "CommonLibrary/blob.h"
#include "CommonLibrary/GeoShape.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/Variant.h"
#include <iostream>
#include <memory>
extern "C" 
{
#include "sqlite3/sqlite3.h"
}

#ifdef ANDROID
	//#include <tr1/memory>
//#include "c:\NDK\x64\android-ndk-r10e\sources\cxx-stl\gnu-libstdc++\4.9\include\tr1\shared_ptr.h"
#endif


namespace GisEngine
{
	namespace GeoDatabase
	{
		namespace SQLiteUtils
		{

			class CSQLiteResultSet : public CommonLib::AutoRefCounter
			{
			public:

				CSQLiteResultSet (sqlite3_stmt* stmt)
					: m_pStmt(stmt)
				{
				}

				~CSQLiteResultSet ()
				{
					if (m_pStmt)
					{
						sqlite3_finalize (m_pStmt);
					}
				}

				bool IsValid ()
				{
					return m_pStmt != 0;
				}

				bool StepNext ()
				{
					int nRetVal = sqlite3_step (m_pStmt);

					if (nRetVal != SQLITE_ROW && nRetVal != SQLITE_DONE)
					{

						m_sError = (sqlite3_errmsg(sqlite3_db_handle(m_pStmt)));

					}
					return nRetVal == SQLITE_ROW;
				}
				
				sqlite3_stmt* GetStatement()
				{
					return m_pStmt;
				}

				const CommonLib::CString& GetError() const 
				{
					return m_sError;
				}
				bool IsError() const
				{
					return !m_sError.isEmpty();
				}

				bool reset()
				{
					int nRetVal = sqlite3_reset(m_pStmt);
					if(nRetVal != SQLITE_OK)
					{
						m_sError = (sqlite3_errmsg(sqlite3_db_handle(m_pStmt)));
						return false;
					}
					return true;
				}

				int ColumnCount ()
				{
					return sqlite3_column_count (m_pStmt);
				}

				int ColumnType (int col)
				{
					return sqlite3_column_type (m_pStmt, col);
				}

				const char* ColumnName (int col)
				{
					return sqlite3_column_name (m_pStmt, col);
				}

				bool ColumnIsnull (int col)
				{
					return sqlite3_column_type (m_pStmt, col) == SQLITE_NULL;
				}

				int ColumnInt(int col)
				{
					return sqlite3_column_int (m_pStmt, col);
				}

				sqlite_int64 ColumnInt64(int col)
				{
					return sqlite3_column_int64 (m_pStmt, col);
				}

				double ColumnDouble (int col)
				{
					return sqlite3_column_double (m_pStmt, col);
				}

				const char* ColumnText (int col, int& len)
				{
					len = sqlite3_column_bytes (m_pStmt, col);
					return (const char*) sqlite3_column_text (m_pStmt, col);
				}

				const char* ColumnText (int col)
				{
					return (const char*) sqlite3_column_text (m_pStmt, col);
				}

				void ColumnText (int col, CommonLib::CString& sText)
				{
					sText = (const char*)sqlite3_column_text (m_pStmt, col);
				}

				const char* ColumnBlob (int col, int& bytes)
				{
					bytes = sqlite3_column_bytes (m_pStmt, col);
					return (const char*) sqlite3_column_blob (m_pStmt, col);
				}

				void ColumnBlob (int col,CommonLib::CBlob *pBlob)
				{
					int bytes = sqlite3_column_bytes (m_pStmt, col);
					pBlob->copy((byte*) sqlite3_column_blob (m_pStmt, col), bytes);
				}
				void ColumnShape (int col,CommonLib::CGeoShape *pShape, CommonLib::CGeoShape::compress_params *pParams)
				{
					int bytes = sqlite3_column_bytes (m_pStmt, col);
					if(bytes == 0)
					{
						pShape->clear();
						return;
					}
		
					CommonLib::FxMemoryReadStream stream;
					stream.attachBuffer((byte*)sqlite3_column_blob(m_pStmt, col), bytes);
					pShape->decompress(&stream, pParams);
				}


				bool ColumnBindInt (int col, int32 nVal)
				{
					int nRetVal =sqlite3_bind_int(m_pStmt, col, nVal);
					if(nRetVal != SQLITE_OK)
					{
						m_sError = (sqlite3_errmsg(sqlite3_db_handle(m_pStmt)));
						return false;
					}
					return true;
				}

				bool ColumnBindInt64 (int col, int64 nVal)
				{
					int nRetVal =sqlite3_bind_int64(m_pStmt, col, nVal);
					if(nRetVal != SQLITE_OK)
					{
						m_sError = (sqlite3_errmsg(sqlite3_db_handle(m_pStmt)));
						return false;
					}
					return true;
				}
				bool ColumnBindDouble (int col, double dVal)
				{
					int nRetVal =sqlite3_bind_double(m_pStmt, col, dVal);
					if(nRetVal != SQLITE_OK)
					{
						m_sError = (sqlite3_errmsg(sqlite3_db_handle(m_pStmt)));
						return false;
					}
					return true;
				}

				bool ColumnBindBlob (int nCol, const byte* pData, uint32 nSize)
				{
					int nRetVal = sqlite3_bind_blob(m_pStmt, nCol, pData, nSize, SQLITE_TRANSIENT);
					if(nRetVal != SQLITE_OK)
					{
						m_sError = (sqlite3_errmsg(sqlite3_db_handle(m_pStmt)));
						return false;
					}
					return true;
				}

				bool ColumnBindBlob (int nCol, CommonLib::CBlob *pBlob)
				{
					return ColumnBindBlob(nCol, pBlob->buffer(), pBlob->size());
				}


				bool ColumnBindText (int nCol, const char* pText, uint32 nlength)
				{
					if(pText == NULL || nlength == 0)
						return true;

					int nRetVal = sqlite3_bind_text(m_pStmt, nCol, pText, nlength, SQLITE_TRANSIENT);
					if(nRetVal != SQLITE_OK)
					{
						m_sError = (sqlite3_errmsg(sqlite3_db_handle(m_pStmt)));
						return false;
					}
					return true;
				}


				bool ColumnBindText (int nCol, const CommonLib::CString& sText)
				{
					return ColumnBindText(nCol, sText.cstr(), sText.length());
				}

				bool ColumnBind(int nCol, eDataTypes type, CommonLib::CVariant *pVal)
				{
					if(!pVal)
						return true;
					int retVal = 0;
					switch(type)
					{
					case dtInteger8:
						return ColumnBindInt(nCol, (int32)pVal->Get<int8>());
						break;
					case dtInteger16:
						return ColumnBindInt(nCol, (int32)pVal->Get<int16>());
						break;
					case dtOid32:
					case dtInteger32:
						return ColumnBindInt(nCol, pVal->Get<int32>());
						break;
					case dtOid64:
					case dtInteger64:
						return ColumnBindInt64(nCol, (int64)pVal->Get<int64>());
						break;
					case dtUInteger8:
						return ColumnBindInt(nCol, (int32)pVal->Get<byte>());
						break;
					case dtUInteger16:
						return ColumnBindInt(nCol, (int32)pVal->Get<uint16>());
						break;
					case dtUInteger32:
						return ColumnBindInt(nCol, (int32)pVal->Get<uint32>());
						break;
					case dtUInteger64:
						return ColumnBindInt64(nCol, (int64)pVal->Get<uint64>());
						break;
					case dtFloat:
						return ColumnBindDouble(nCol, (double)pVal->Get<float>());
						break;
					case dtDouble:
						return ColumnBindDouble(nCol, pVal->Get<double>());
						break;
					case  dtBlob:
						{
							CommonLib::IRefObjectPtr pObj = pVal->Get<CommonLib::IRefObjectPtr>();
							if(pObj.get())
							{
								CommonLib::CBlob *pBlob = (CommonLib::CBlob*)pObj.get();
								if(pBlob)
								{
									return ColumnBindBlob(nCol, pBlob);
								}
							}

						}
						break;
					case  dtString:
					case  dtAnnotation:
						{
							CommonLib::CString sText = pVal->Get<CommonLib::CString>();
							if(!sText.isEmpty())
								return ColumnBindText(nCol, sText);
							return true;

						}
						break;
					}
					m_sError.format(L"Undefined type: %d", (int)type);
					return false;
				}

			protected:
				sqlite3_stmt* m_pStmt;
				CommonLib::CString m_sError;

			};

			COMMON_LIB_REFPTR_TYPEDEF(CSQLiteResultSet);
			//typedef std::shared_ptr<CSQLiteResultSet> CSQLiteResultSetPtr;
		}
	}
}

#endif