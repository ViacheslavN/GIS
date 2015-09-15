#include "stdafx.h"
#include "SQLiteWorkspace.h"
#include "sqlite3/sqlite3.h"
#include "CommonLibrary/File.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
 

		CSQLiteWorkspace::CSQLiteWorkspace() : TBase (wiSqlLite), m_pConn(0), m_precordSet(0)
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
			m_sPath(pszPath)
		{
			m_sName = pszName;
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
			ITablePtr pTable = GetTable(sName);
			if(pTable.get())
				return  ITablePtr();



			return ITablePtr();
		}
		IFeatureClassPtr CSQLiteWorkspace::CreateFeatureClass(const CommonLib::CString& name, IFields* fields, const CommonLib::CString& shapeFieldName)
		{
			return IFeatureClassPtr();
		}

		ITablePtr CSQLiteWorkspace::OpenTable(const CommonLib::CString& name)
		{
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