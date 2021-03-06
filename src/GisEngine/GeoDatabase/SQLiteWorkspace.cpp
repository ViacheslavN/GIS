#include "stdafx.h"
#include "SQLiteWorkspace.h"
extern "C" {
#include "sqlite3/sqlite3.h"
}
#include "CommonLibrary/File.h"
#include "SQLiteUtils.h"
#include "Fields.h"
#include "Field.h"
#include "GeoDatabaseUtils.h"
#include "SQLiteTable.h"
#include "SQLiteFeatureClass.h"
#include "SQLiteDB.h"
#include "SQLiteTransaction.h"
#include "SQLiteTable.h"

namespace GisEngine
{
	namespace GeoDatabase
	{
 
		
		CSQLiteWorkspace::CSQLiteWorkspace(int32 nID) : TBase (wtSqlLite, nID)
		{
			m_WorkspaceType = wtSqlLite;
		//	sqlite3_initialize();
		}
		CSQLiteWorkspace::~CSQLiteWorkspace()
		{
			close();
		//	sqlite3_shutdown();
		}
		CSQLiteWorkspace::CSQLiteWorkspace(const wchar_t *pszName, const wchar_t *pszPath, int32 nID) : TBase (wtSqlLite, nID),
			m_sPath(pszPath)
		{
			m_sName = pszName;
			m_sHash = m_sPath + m_sName;
		}
		bool CSQLiteWorkspace::IsConnect() const
		{
			return m_pDB.get() ? m_pDB->IsConnect() : false;
		}
		bool CSQLiteWorkspace::IsError() const
		{
			return m_pDB.get() ? m_pDB->IsError() : false;
		}
		uint32 CSQLiteWorkspace::GetErrorCode() const {return 0;}
		void CSQLiteWorkspace::GetErrorText( CommonLib::CString& sStr, uint32 nCode) 
		{
			if(m_pDB.get())
				sStr = m_pDB->GetError();
		}
		IWorkspacePtr CSQLiteWorkspace::Create(const wchar_t *pszName, const wchar_t *pszPath)
		{
			CommonLib::CString sFullName = pszPath;

			if(!sFullName.isEmpty())
			{
				if(sFullName[sFullName.length() - 1] != L'/' || sFullName[sFullName.length() - 1] != L'\\')
				{
					sFullName += L'\\';
				}
			}

			sFullName += pszName;
			if(sFullName.isEmpty())
			{
				//TO DO Error
				return IWorkspacePtr();
			}
			IWorkspacePtr pWks = CWorkspaceHolder::GetWorkspace(wtSqlLite, sFullName);
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

			CSQLiteWorkspace *pSQLiteWks = new  CSQLiteWorkspace(pszName, pszPath, CWorkspaceHolder::GetIDWorkspace());
			if(!pSQLiteWks->create(sFullName))
			{
				//TO DO Error
				delete pSQLiteWks;
				return IWorkspacePtr();
			}
			CWorkspaceHolder::AddWorkspace((IWorkspace*)pSQLiteWks);
			return IWorkspacePtr((IWorkspace*)pSQLiteWks);
		}

		IWorkspacePtr CSQLiteWorkspace::Open(const wchar_t *pszName, const wchar_t *pszPath, bool bWrite, bool bOpenAll)
		{

			CommonLib::CString sFullName = pszPath;

			if(!sFullName.isEmpty())
			{
				if(sFullName[sFullName.length() - 1] != L'/' || sFullName[sFullName.length() - 1] != L'\\')
				{
					sFullName += L'\\';
				}
			}

			sFullName += pszName;
			if(sFullName.isEmpty())
			{
				//TO DO Error
				return IWorkspacePtr();
			}
			IWorkspacePtr pWks = CWorkspaceHolder::GetWorkspace(wtSqlLite, sFullName);
			if(pWks.get())
			{
				//TO DO Error
				return pWks;
			}

			CSQLiteWorkspace *pSQLiteWks = new  CSQLiteWorkspace(pszName, pszPath, CWorkspaceHolder::GetIDWorkspace());
			if(!pSQLiteWks->load(sFullName, bWrite, bOpenAll))
			{
				//TO DO Error
				delete pSQLiteWks;
				return IWorkspacePtr();
			}

			CWorkspaceHolder::AddWorkspace((IWorkspace*)pSQLiteWks);
			return IWorkspacePtr((IWorkspace*)pSQLiteWks);
		}
		IWorkspacePtr CSQLiteWorkspace::Open(CommonLib::IReadStream* pSteram, bool bOpenAll)
		{
			
			CSQLiteWorkspace *pSQLiteWks = new  CSQLiteWorkspace(-1);
			if(!pSQLiteWks->load(pSteram)) // TO FIX use name, path
			{
				//TO DO Error
				delete pSQLiteWks;
				return IWorkspacePtr();
			}

			return IWorkspacePtr((IWorkspace*)pSQLiteWks);
		}
		IWorkspacePtr CSQLiteWorkspace::Open(GisCommon::IXMLNode *pNode, bool bOpenAll)
		{
			int nWksID = pNode->GetPropertyInt32(L"ID", -1);
			if(nWksID == -1)
				return  IWorkspacePtr();
			CommonLib::CString sName = pNode->GetPropertyString(L"Name", "");
			CommonLib::CString sPath = pNode->GetPropertyString(L"Path", "");
			CSQLiteWorkspace *pSQLiteWks = new  CSQLiteWorkspace(sName.cwstr(), sPath.cwstr(), nWksID);
			if(!pSQLiteWks->load(sPath + sName, true, bOpenAll)) // TO FIX use name, path
			{
				//TO DO Error
				delete pSQLiteWks;
				return IWorkspacePtr();
			}
 
			return IWorkspacePtr((IWorkspace*)pSQLiteWks);
		}

		
		ITablePtr  CSQLiteWorkspace::CreateTable(const CommonLib::CString& sName, IFields* pFields )
		{

			if(!IsConnect())
				return ITablePtr();

			m_pDB->ClearError();
			if(sName.isEmpty())
			{
				m_pDB->SetErrorText(L"empty table name");
				return ITablePtr();
			}

			ITablePtr pTable = GetTable(sName);
			if(pTable.get())
			{
				m_pDB->SetErrorText(L"table " + sName + L" is exist");
				return  ITablePtr();
			}

			CommonLib::CString sSQL;
			SQLiteUtils::CreateSQLCreateTable(pFields, sName, sSQL);
			

			 if(!m_pDB->execute(sSQL))
				 return ITablePtr();
			

			pTable = OpenTable(sName);
			return pTable;
		}
		IFeatureClassPtr CSQLiteWorkspace::CreateFeatureClass(const CommonLib::CString& sName, IFields* pFields, bool bSaveFCProp)
		{
	
			if(!IsConnect())
				return IFeatureClassPtr();
			
			m_pDB->ClearError();

			if(sName.isEmpty())
			{
				m_pDB->SetErrorText(L"empty FeatureClass name");
				return IFeatureClassPtr();
			}

			IFeatureClassPtr pFC = GetFeatureClass(sName);
			if(pFC.get())
			{
				m_pDB->SetErrorText("FeatureClass " + sName + L" is exist");
				return  IFeatureClassPtr();
			}
			CSQLiteFeatureClass* pSQLiteFC = new CSQLiteFeatureClass(this, sName, sName);
			if(!pSQLiteFC->CreateFeatureClass(pFields, bSaveFCProp))
			{
				delete pSQLiteFC;
				return  IFeatureClassPtr();
			}
			AddDataset(pSQLiteFC);
			return  IFeatureClassPtr(pSQLiteFC);

			//pFC =  OpenFeatureClass(sName);
		 
			//return pFC;
		}

		ITablePtr CSQLiteWorkspace::OpenTable(const CommonLib::CString& sName)
		{
			
			if(!IsConnect())
			{
				return ITablePtr();
			}
			m_pDB->ClearError();

			if(sName.isEmpty())
			{
				m_pDB->SetErrorText(L"Empty table Name");
				return ITablePtr();
			}

			ITablePtr pTable = GetTable(sName);
			if(pTable.get())
				return pTable;
		
		 
			IFieldsPtr pFields = m_pDB->ReadFields(sName);
			if(!pFields.get())
				return pTable;


			pTable = (ITable*)new  CSQLiteTable(this, sName, sName);
			pTable->SetFields(pFields.get());

		

			AddDataset(pTable.get());
	 
			return pTable;
		}
		IFeatureClassPtr CSQLiteWorkspace::OpenFeatureClass(const CommonLib::CString& sName)
		{
	
			if(!IsConnect())
			{
				return IFeatureClassPtr();
			}
			m_pDB->ClearError();
			if(sName.isEmpty())
			{
				m_pDB->SetErrorText(L"Empty table Name");
				return IFeatureClassPtr();
			}

			IFeatureClassPtr pFC = GetFeatureClass(sName);
			if(pFC.get())
				return pFC;


			CSQLiteFeatureClass *pSQLiteFC = new CSQLiteFeatureClass(this, sName, sName);
			if(!pSQLiteFC->open())
			{
				delete pSQLiteFC;
				return IFeatureClassPtr();
			}			
	 
			AddDataset(pSQLiteFC);
			return IFeatureClassPtr(pSQLiteFC);
		}

		
		bool CSQLiteWorkspace::save(CommonLib::IWriteStream *pWriteStream) const
		{
			TBase::save(pWriteStream);
			CommonLib::CWriteMemoryStream steram;
			steram.write(m_sPath);

			pWriteStream->write(&steram);
			return true;
		}
		bool CSQLiteWorkspace::load(CommonLib::IReadStream* pReadStream)
		{
			TBase::load(pReadStream);
			CommonLib::FxMemoryReadStream stream;
			SAFE_READ(pReadStream->save_read(&stream, true))
			SAFE_READ(stream.save_read(m_sPath))

			const CommonLib::CString sFullName = m_sPath + m_sName;
			return load(sFullName, true, false);
		}
		bool CSQLiteWorkspace::saveXML(GisCommon::IXMLNode* pXmlNode) const
		{
			TBase::saveXML(pXmlNode);
			pXmlNode->AddPropertyString(L"Path", m_sPath);
			return true;
		}
		bool CSQLiteWorkspace::load(const GisCommon::IXMLNode* pXmlNode)
		{
			if(!TBase::load(pXmlNode))
				return false;
			m_sPath = pXmlNode->GetPropertyString(L"Path", m_sPath);
			return true;
		}
		bool  CSQLiteWorkspace::create(const CommonLib::CString& sFullName)
		{
			m_pDB.reset(new SQLiteUtils::CSQLiteDB(sFullName, true));
			return !m_pDB->IsError();
		}
		bool CSQLiteWorkspace::load(const CommonLib::CString& sFullName, bool bWrite, bool bOpenAll)
		{

			 m_pDB.reset(new SQLiteUtils::CSQLiteDB(sFullName, false));
			 if(m_pDB->IsError())
			 {
				 return false;
			 }

			 if(!bOpenAll)
				 return true;


			 CommonLib::CString sSQL = L"SELECT * FROM sqlite_master WHERE type='table' AND name NOT LIKE '%_SpatialIndex%' AND name NOT LIKE '%_PROPERTIES' ORDER BY name ";

			 SQLiteUtils::CSQLiteResultSetPtr pRS = m_pDB->prepare_query(sSQL);
			 if(!pRS.get())
			 {
				return false;
			 }

			 
			 std::vector<CommonLib::CString> tableNames;
			 while(pRS->StepNext())
			 {
				CommonLib::CString tableName = pRS->ColumnText(1);
				CommonLib::CString sCreateSQL = pRS->ColumnText(4);

				//if(sCreateSQL.find(m_sRTreePrefix) == -1)
					 tableNames.push_back(tableName);
			 }
		 


			 for (size_t i = 0, sz = tableNames.size(); i < sz; ++i)
			 {
				 if(CSQLiteFeatureClass::IsFeatureClass(tableNames[i], m_pDB.get()))
				 {
					 if(!OpenFeatureClass(tableNames[i]))
					 {
						 return false;
					 }
				 }
				 else
				 {

					 if(!OpenTable(tableNames[i]))
					 {
						 return false;
					 }
				 }

			 }

			 return true;
		}
		void CSQLiteWorkspace::close()
		{
			if(m_pDB.get())
			{
				m_pDB.reset();
			}
		}
		ITransactionPtr CSQLiteWorkspace::startTransaction(eTransactionType type)
		{
			if(!m_pDB.get())
			{
				return ITransactionPtr();
			}
			CSQLiteTransaction *pTran = new  CSQLiteTransaction(m_pDB.get());
			if(!pTran->begin())
			{
				delete pTran;
				return ITransactionPtr();
			}
			return ITransactionPtr(pTran);
		}
	}
}