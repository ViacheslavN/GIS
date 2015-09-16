#include "stdafx.h"
#include "SQLiteWorkspace.h"
#include "sqlite3/sqlite3.h"
#include "CommonLibrary/File.h"
#include "SQLiteUtils.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
 

		CSQLiteWorkspace::CSQLiteWorkspace() : TBase (wiSqlLite), m_pConn(0), m_precordSet(0), m_bError(false)
		{
			m_WorkspaceType = wiSqlLite;
		//	sqlite3_initialize();
		}
		CSQLiteWorkspace::~CSQLiteWorkspace()
		{
			close();
		//	sqlite3_shutdown();
		}
		CSQLiteWorkspace::CSQLiteWorkspace(const wchar_t *pszName, const wchar_t *pszPath) : TBase (wiSqlLite),
			m_sPath(pszPath), m_pConn(0), m_precordSet(0), m_bError(false)
		{
			m_sName = pszName;
		}
		bool CSQLiteWorkspace::IsConnect() const
		{
			return m_pConn != NULL;
		}
		IWorkspacePtr CSQLiteWorkspace::Create(const wchar_t *pszName, const wchar_t *pszPath)
		{
			CommonLib::CString sFullName = pszPath;

			if(!sFullName.isEmpty())
			{
				if(sFullName[sFullName.length() - 1] != _T('/') || sFullName[sFullName.length() - 1] != _T('\\'))
				{
					sFullName += _T('\\');
				}
			}

			sFullName += pszName;
			if(sFullName.isEmpty())
			{
				//TO DO Error
				return IWorkspacePtr();
			}
			IWorkspacePtr pWks = CWorkspaceHolder::GetWorkspace(wiSqlLite, sFullName);
			if(pWks.get())
			{
				//TO DO Error
				return IWorkspacePtr();
			}

			if(CommonLib::FileSystem::isFileExisit(sFullName.cwstr()))
			{
				//TO DO Error
				return IWorkspacePtr();
			}

			CSQLiteWorkspace *pSQLiteWks = new  CSQLiteWorkspace(pszName, pszPath);
			if(!pSQLiteWks->create(sFullName))
			{
				//TO DO Error
				delete pSQLiteWks;
				return IWorkspacePtr();
			}
			CWorkspaceHolder::AddWorkspace((IWorkspace*)pSQLiteWks, pszPath);
			return IWorkspacePtr((IWorkspace*)pSQLiteWks);
		}

		IWorkspacePtr CSQLiteWorkspace::Open(const wchar_t *pszName, const wchar_t *pszPath, bool bWrite)
		{

			CommonLib::CString sFullName = pszPath;

			if(!sFullName.isEmpty())
			{
				if(sFullName[sFullName.length() - 1] != _T('/') || sFullName[sFullName.length() - 1] != _T('\\'))
				{
					sFullName += _T('\\');
				}
			}

			sFullName += pszName;
			if(sFullName.isEmpty())
			{
				//TO DO Error
				return IWorkspacePtr();
			}
			IWorkspacePtr pWks = CWorkspaceHolder::GetWorkspace(wiSqlLite, sFullName);
			if(pWks.get())
			{
				//TO DO Error
				return pWks;
			}

			CSQLiteWorkspace *pSQLiteWks = new  CSQLiteWorkspace(pszName, pszPath);
			if(!pSQLiteWks->load(sFullName, bWrite))
			{
				//TO DO Error
				delete pSQLiteWks;
				return IWorkspacePtr();
			}

			CWorkspaceHolder::AddWorkspace((IWorkspace*)pSQLiteWks, pszPath);
			return IWorkspacePtr((IWorkspace*)pSQLiteWks);
		}
		IWorkspacePtr CSQLiteWorkspace::Open(CommonLib::IReadStream* pSteram)
		{
			return IWorkspacePtr();
		}
		IWorkspacePtr CSQLiteWorkspace::Open(GisCommon::IXMLNode *pNode)
		{
			return IWorkspacePtr();
		}

		
		ITablePtr  CSQLiteWorkspace::CreateTable(const CommonLib::CString& sName, IFields* fields)
		{
			m_bError = false;
			if(!IsConnect())
				return ITablePtr();

			if(!sName.isEmpty())
				return ITablePtr();

			ITablePtr pTable = GetTable(sName);
			if(pTable.get())
				return  ITablePtr();


			std::vector<CommonLib::CString> vecPrimaryKey;
			CommonLib::CString createTable = L"CREATE TABLE "; 
			createTable += sName;

			int nCnt = fields->GetFieldCount();
			if(nCnt > 0)
				createTable += L" ( ";




			CommonLib::CString sType;

			 for (size_t i = 0; i < nCnt; ++i)
			 {
				 IFieldPtr pField = fields->GetField(i);
				 const CommonLib::CVariant& defVal = pField->GetDefaultValue();

				 SQLitefUtils::FieldType2SQLiteType(pField->GetType(), sType);
				 if(pField->GetIsPrimaryKey())
				 {
					 vecPrimaryKey.push_back(pField->GetName());
				 }

				 if(i != 0)
					 createTable += L", ";

				 createTable += pField->GetName() + L" ";
				
				 if(!defVal.isType<CommonLib::CNullVariant>())
				 {
					 CommonLib::ToStringVisitor vis;
					  sType += L" DEFAULT '" + CommonLib::apply_visitor<CommonLib::ToStringVisitor>(defVal, vis) + "' ";
				 }
				

				 if(!pField->GetIsNullable())
					 sType += L" NOT NULL ";
				  createTable += sType;
			 }
			 
			

			 if(!vecPrimaryKey.empty())
			 {
				 createTable += L", PRIMARY KEY( ";

				 for (size_t i = 0, sz = vecPrimaryKey.size(); i < sz; ++i)
				 {
					 if(i != 0)
						 createTable += L", ";
					
					 createTable += vecPrimaryKey[i];
				 }

				 createTable += L")";
			 }
			 createTable += L");";
			 


			 int retVal = sqlite3_exec(m_pConn, createTable.cstr(), 0, 0, 0);

			 if(retVal != SQLITE_OK)
			 {
				 m_bError = true;
				 m_sErrorMessage = CommonLib::CString(sqlite3_errmsg(m_pConn));
				 return ITablePtr();
			 }

			return OpenTable(sName);

		}
		IFeatureClassPtr CSQLiteWorkspace::CreateFeatureClass(const CommonLib::CString& name, IFields* fields, const CommonLib::CString& shapeFieldName)
		{
			return IFeatureClassPtr();
		}

		ITablePtr CSQLiteWorkspace::OpenTable(const CommonLib::CString& sName)
		{
			m_bError = false;
			if(!IsConnect())
				return ITablePtr();

			if(!sName.isEmpty())
				return ITablePtr();

			ITablePtr pTable = GetTable(sName);
			if(pTable.get())
				return pTable;
			/*
			//where rc is an int variable if wondering :/
			rc = sqlite3_prepare_v2(dbPointer, "pragma table_info ('your table name goes here')", -1, &stmt, NULL);

			if (rc==SQLITE_OK)
			{
			//will continue to go down the rows (columns in your table) till there are no more
			while(sqlite3_step(stmt) == SQLITE_ROW)
			{
			sprintf(colName, "%s", sqlite3_column_text(stmt, 1));
			//do something with colName because it contains the column's name
			}
			}
			sqlite3VdbeSetColName(v, 0, COLNAME_NAME, "cid", SQLITE_STATIC);
			sqlite3VdbeSetColName(v, 1, COLNAME_NAME, "name", SQLITE_STATIC);
			sqlite3VdbeSetColName(v, 2, COLNAME_NAME, "type", SQLITE_STATIC);
			sqlite3VdbeSetColName(v, 3, COLNAME_NAME, "notnull", SQLITE_STATIC);
			sqlite3VdbeSetColName(v, 4, COLNAME_NAME, "dflt_value", SQLITE_STATIC);
			sqlite3VdbeSetColName(v, 5, COLNAME_NAME, "pk", SQLITE_STATIC);

			//returns the name
			sqlite3_column_text(stmt, 1);
			//returns the type
			sqlite3_column_text(stmt, 2);
			*/
			return ITablePtr();
		}
		IFeatureClassPtr CSQLiteWorkspace::OpenFeatureClass(const CommonLib::CString& name)
		{
			return IFeatureClassPtr();
		}

		
		bool CSQLiteWorkspace::save(CommonLib::IWriteStream *pWriteStream) const
		{
			return false;
		}
		bool CSQLiteWorkspace::load(CommonLib::IReadStream* pReadStream)
		{
			return false;
		}
		bool CSQLiteWorkspace::saveXML(GisCommon::IXMLNode* pXmlNode) const
		{
			return false;
		}
		bool CSQLiteWorkspace::load(GisCommon::IXMLNode* pXmlNode)
		{
			return false;
		}
		bool  CSQLiteWorkspace::create(const CommonLib::CString& sFullName)
		{
			int retVal = sqlite3_open_v2(sFullName.cstr(), &m_pConn, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
			if(retVal != SQLITE_OK)
			{
				//TO DO Error;
				close();
				return false;
			}

			return true;
		}
		bool CSQLiteWorkspace::load(const CommonLib::CString& sFullName, bool bWrite)
		{
			 int flags = bWrite ? SQLITE_OPEN_READWRITE:  SQLITE_OPEN_READONLY;

			 int retVal = sqlite3_open_v2(sFullName.cstr(), &m_pConn, flags, 0);
			 if(retVal != SQLITE_OK)
			 {
				 //TO DO Error;
				 close();
				 return false;
			 }


			 std::string sSQL = "SELECT * FROM dbname.sqlite_master WHERE type='table' ORDER BY name";
			 int retValue = sqlite3_prepare_v2(m_pConn, sSQL.c_str(), -1, &m_precordSet, 0);
			 if(retValue != SQLITE_OK)
			 {
					//TO DO Error
				 close();
				 return false;
			 }

			 retValue = sqlite3_step(m_precordSet);
			 std::vector<std::string> tableNames;
			 while(retValue == SQLITE_ROW)
			 {

				 std::string tableName = (char*)sqlite3_column_text(m_precordSet, 0);
				 tableNames.push_back(tableName);


				 sqlite3_step(m_precordSet);
			 }
		 
			 sqlite3_finalize(m_precordSet);


			 return true;
		}
		void CSQLiteWorkspace::close()
		{
			if(m_pConn != 0)
			{
				int result = sqlite3_close(m_pConn);
				if(result != SQLITE_OK)
				{
					//TO DO Error
				}
				m_pConn = 0;
			}
		}
		ITransactionPtr CSQLiteWorkspace::startTransaction()
		{
			return ITransactionPtr();
		}
	}
}