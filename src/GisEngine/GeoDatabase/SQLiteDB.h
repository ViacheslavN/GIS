#ifndef GIS_ENGINE_GEO_DATABASE_SQLITE_DB_H_
#define GIS_ENGINE_GEO_DATABASE_SQLITE_DB_H_

#include "SQLiteResultSet.h"
extern "C" 
{
#include "sqlite3/sqlite3.h"
}
#include "Fields.h"
#include "Field.h"
#include "SQLiteUtils.h"
#include "GeoDatabaseUtils.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
		namespace SQLiteUtils
		{

			class CSQLiteDB
			{
			public:
				CSQLiteDB(CommonLib::CString sFile, bool bCreate) : m_pDB(NULL)
				{


					int mode =  SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE|SQLITE_OPEN_READWRITE;
					if(bCreate)
					{
						mode  |= SQLITE_OPEN_CREATE;
					}
					
					int retVal = sqlite3_open_v2 (sFile.cstr(), &m_pDB, mode, 0);
 					if (retVal != SQLITE_OK)
					{
						m_sError =  sqlite3_errmsg (m_pDB);
					}
					else
						sqlite3_busy_timeout(m_pDB, 1000);

				}


			CSQLiteDB (CommonLib::CString sFile,  int flags)
				: m_pDB(NULL)
				
			{
				int retVal = sqlite3_open_v2 (sFile.cstr(), &m_pDB, flags, 0);
				if (retVal != SQLITE_OK)
				{
					m_sError =  sqlite3_errmsg (m_pDB);
				}
			}

			virtual ~CSQLiteDB ()
			{
				if (m_pDB)
				{
					sqlite3_close (m_pDB);
				}
			}

			

			TSQLiteResultSetPtr prepare_query(const CommonLib::CString& sSql)
			{
				sqlite3_stmt* pStmt = 0;
				const int rc = sqlite3_prepare_v2 (m_pDB, sSql.cstr(), -1, &pStmt, 0);
				if (rc != SQLITE_OK)
				{
					m_sError  =  sqlite3_errmsg (m_pDB); 
					return TSQLiteResultSetPtr();
				}

				return std::make_shared<CSQLiteResultSet>(pStmt);
			}

			bool execute(const CommonLib::CString& sSql)
			{
				const int rc = sqlite3_exec(m_pDB, sSql.cstr(), 0, 0, 0);
				if (rc != SQLITE_OK)
				{
					m_sError  =  sqlite3_errmsg (m_pDB); 
					return false;
				}
				return true;
			}

			int execute_ret_code(const CommonLib::CString& sSql)
			{

				const int rc = sqlite3_exec(m_pDB, sSql.cstr(), 0, 0, 0);
				return rc;
			}

			sqlite3* operator*()
			{
				return m_pDB;
			}

			bool load_extension(const CommonLib::CString& sExtPath)
			{
				sqlite3_enable_load_extension(m_pDB, 1);
				int result = sqlite3_load_extension(m_pDB, sExtPath.cstr(), 0 , 0);
				return (result == SQLITE_OK)? true : false;
			}
			bool IsError() const
			{
				return !m_sError.isEmpty();
			}
			const CommonLib::CString& GetError() const
			{
				return m_sError;
			}
			bool IsConnect()
			{
				return m_pDB != NULL;
			}
			void SetErrorText(const CommonLib::CString& sText)
			{
				m_sError = sText;
			}
			void ClearError()
			{
				m_sError.clear();
			}
			int64 GetLastInsertRowID()
			{
				return sqlite3_last_insert_rowid(m_pDB);
			}

			IFieldsPtr ReadFields(const CommonLib::CString& sTableName)
			{
				IFieldsPtr pFields;
				CommonLib::CString sQuery;
				sQuery.format(L"pragma table_info ('%s')", sTableName.cwstr());

				SQLiteUtils::TSQLiteResultSetPtr pRS = prepare_query(sQuery);
				if (pRS.get())
				{
					pFields = new CFields();
					while(pRS->StepNext())
					{
						CommonLib::CString sName = pRS->ColumnText(1);
						CommonLib::CString sType = pRS->ColumnText(2);
						CommonLib::CString sNotnull = pRS->ColumnText(3);
						CommonLib::CString sDefValue = pRS->ColumnText(4);
						CommonLib::CString sPK = pRS->ColumnText(5);

						eDataTypes type = SQLiteUtils::SQLiteType2FieldType(sType);
						IFieldPtr pField(new CField());
						pField->SetType(type);
						pField->SetName(sName);
						pField->SetIsNullable(sType != L"1");
						if(!sDefValue.isEmpty())
						{
							CommonLib::CVariant var = GeoDatabaseUtils::GetVariantFromString(type, sDefValue);
							pField->SetIsDefault(var);
						}
						pField->SetIsPrimaryKey(sPK == L"1");
						pFields->AddField(pField.get());
					}
				}

				return pFields;
			}

			bool IsTableExist(const CommonLib::CString& sName)
			{
				CommonLib::CString sSQL;
				sSQL.format(L"SELECT name FROM sqlite_master WHERE type='table' AND name='%s'", sName.cwstr());
				SQLiteUtils::TSQLiteResultSetPtr pRS = prepare_query(sSQL);
				if(!pRS->IsError() && pRS->StepNext())
				{
					return true;
				}
				return false;
			}
		private:

			sqlite3* m_pDB;
			CommonLib::CString m_sDBFile;
			CommonLib::CString m_sError;
		};

		}
	}
}

#endif