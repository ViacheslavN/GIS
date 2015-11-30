#include "stdafx.h"
#include "EmbDBWorkspace.h"
#include "CommonLibrary/File.h"
#include "EmbDBFeatureClass.h"
#include "EmbDBTransaction.h"
namespace GisEngine
{
	namespace GeoDatabase
	{

		CEmbDBWorkspace::CEmbDBWorkspace(int32 nID): TBase (wtEmbDB, nID)
		{
		}
		CEmbDBWorkspace::CEmbDBWorkspace(const wchar_t *pszName, const wchar_t *pszPath, int32 nID) : TBase (wtEmbDB, nID),	m_sPath(pszPath)
		{
			m_sName = pszName;
			m_sHash = m_sPath + m_sName;
		}
		CEmbDBWorkspace::~CEmbDBWorkspace()
		{
		}

		void CEmbDBWorkspace::close()
		{
			if(m_pDB.get())
				m_pDB->close();
		}
		bool CEmbDBWorkspace::IsError() const
		{
			return false;
		}
		uint32 CEmbDBWorkspace::GetErrorCode() const
		{
			return 0;
		}
		void CEmbDBWorkspace::GetErrorText( CommonLib::CString& sStr, uint32 nCode)
		{

		}


		IWorkspacePtr CEmbDBWorkspace::Create(const wchar_t *pszName, const wchar_t *pszPath)
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
			IWorkspacePtr pWks = CWorkspaceHolder::GetWorkspace(wtEmbDB, sFullName);
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

			CEmbDBWorkspace *pEmbDBWks = new  CEmbDBWorkspace(pszName, pszPath, CWorkspaceHolder::GetIDWorkspace());
			if(!pEmbDBWks->create(sFullName))
			{
				//TO DO Error
				delete pEmbDBWks;
				return IWorkspacePtr();
			}
			CWorkspaceHolder::AddWorkspace((IWorkspace*)pEmbDBWks);
			return IWorkspacePtr((IWorkspace*)pEmbDBWks);
		}

		IWorkspacePtr CEmbDBWorkspace::Open(const wchar_t *pszName, const wchar_t *pszPath, bool bWrite, bool bOpenAll)
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
			IWorkspacePtr pWks = CWorkspaceHolder::GetWorkspace(wtSqlLite, sFullName);
			if(pWks.get())
			{
				//TO DO Error
				return pWks;
			}

			CEmbDBWorkspace *pEmbDBWks = new  CEmbDBWorkspace(pszName, pszPath, CWorkspaceHolder::GetIDWorkspace());
			if(!pEmbDBWks->load(sFullName, bWrite, bOpenAll))
			{
				//TO DO Error
				delete pEmbDBWks;
				return IWorkspacePtr();
			}

			CWorkspaceHolder::AddWorkspace((IWorkspace*)pEmbDBWks);
			return IWorkspacePtr((IWorkspace*)pEmbDBWks);
		}
		IWorkspacePtr CEmbDBWorkspace::Open(CommonLib::IReadStream* pSteram, bool bOpenAll)
		{

			CEmbDBWorkspace *pEmbDBWks = new  CEmbDBWorkspace(-1);
			if(!pEmbDBWks->load(pSteram)) // TO FIX use name, path
			{
				//TO DO Error
				delete pEmbDBWks;
				return IWorkspacePtr();
			}

			return IWorkspacePtr((IWorkspace*)pEmbDBWks);
		}
		IWorkspacePtr CEmbDBWorkspace::Open(GisCommon::IXMLNode *pNode, bool bOpenAll)
		{
			int nWksID = pNode->GetPropertyInt32(L"ID", -1);
			if(nWksID == -1)
				return  IWorkspacePtr();
			CommonLib::CString sName = pNode->GetPropertyString(L"Name", "");
			CommonLib::CString sPath = pNode->GetPropertyString(L"Path", "");
			CEmbDBWorkspace *pEmbDBWks = new  CEmbDBWorkspace(sName.cwstr(), sPath.cwstr(), nWksID);
			if(!pEmbDBWks->load(sPath + sName, true, bOpenAll)) // TO FIX use name, path
			{
				//TO DO Error
				delete pEmbDBWks;
				return IWorkspacePtr();
			}

			return IWorkspacePtr((IWorkspace*)pEmbDBWks);
		}


		bool CEmbDBWorkspace::create(const CommonLib::CString& sFullName)
		{
			if(m_pDB.get())
				m_pDB->close();
			m_pDB = embDB::IDatabase::CreateDatabase();
			if(!m_pDB->create(sFullName.cwstr()))
			{
				//TO DO Error
				return false;
			}

			return true;
		}

		bool CEmbDBWorkspace::load(const CommonLib::CString& sFullName, bool bWrite, bool bOpenAll)
		{

			m_pDB =  embDB::IDatabase::CreateDatabase();
			if(!m_pDB.get())
			{
				return false;
			}
			if(!m_pDB->open(sFullName.cwstr()))
			{
				return false;
			}
			

			if(!bOpenAll)
				return true;


			embDB::ISchemaPtr pShema = m_pDB->getSchema();
			if(!pShema.get())
				return false;

			for (size_t i = 0, sz = pShema->getTableCnt(); i < sz; ++i)
			{
				embDB::ITablePtr pTable = pShema->getTable(i);
				if(CEmbDBFeatureClass::IsFeatureClass(pTable.get()))
				{
					if(!OpenFeatureClass(pTable->getName()))
					{
						return false;
					}
				}
				else
				{
					continue;
					if(!OpenTable(pTable->getName()))
					{
						return false;
					}
				}
				
			}

			return true;
		}

		bool CEmbDBWorkspace::save(CommonLib::IWriteStream *pWriteStream) const
		{
			TBase::save(pWriteStream);
			CommonLib::MemoryStream steram;
			steram.write(m_sPath);

			pWriteStream->write(&steram);
			return true;
		}
		bool CEmbDBWorkspace::load(CommonLib::IReadStream* pReadStream)
		{
			TBase::load(pReadStream);
			CommonLib::FxMemoryReadStream stream;
			pReadStream->AttachStream(&stream, pReadStream->readIntu32());
			stream.read(m_sPath);

			const CommonLib::CString sFullName = m_sPath + m_sName;
			return load(sFullName, true, false);
		}

		bool CEmbDBWorkspace::saveXML(GisCommon::IXMLNode* pXmlNode) const
		{
				return true;
		}
		bool CEmbDBWorkspace::load(const GisCommon::IXMLNode* pXmlNode)
		{
			return true;
		}
		ITransactionPtr CEmbDBWorkspace::startTransaction(eTransactionType type)
		{
			CEmbDBTransaction* pTran = new CEmbDBTransaction(m_pDB.get(), type);
			pTran->begin();
			return ITransactionPtr(pTran);
		}


		ITablePtr  CEmbDBWorkspace::CreateTable(const CommonLib::CString& name, IFields* fields, const CommonLib::CString& sOIDName)
		{
			return ITablePtr();
		}

		IFeatureClassPtr CEmbDBWorkspace::CreateFeatureClass(const CommonLib::CString& sName,
			IFields* pFields, const CommonLib::CString& sOIDName,  
			const CommonLib::CString& shapeFieldName,
			const CommonLib::CString& sAnnotationName)
		{
			if(!m_pDB.get())
				return IFeatureClassPtr();
			
			if(sName.isEmpty())
			{
				//m_pDB->SetErrorText(L"empty FeatureClass name");
				return IFeatureClassPtr();
			}

			IFeatureClassPtr pFC = GetFeatureClass(sName);
			if(pFC.get())
			{
				//m_pDB->SetErrorText("FeatureClass " + sName + L" is exist");
				return  IFeatureClassPtr();
			}
			CEmbDBFeatureClass* pEmbDBFC = new CEmbDBFeatureClass(this, sName, sName);
			if(!pEmbDBFC->CreateFeatureClass(pFields))
			{
				delete pEmbDBFC;
				return  IFeatureClassPtr();
			}
			AddDataset(pEmbDBFC);
			return  IFeatureClassPtr(pEmbDBFC);
		}

		ITablePtr CEmbDBWorkspace::OpenTable(const CommonLib::CString& name)
		{
			return ITablePtr();
		}
		IFeatureClassPtr CEmbDBWorkspace::OpenFeatureClass(const CommonLib::CString& sName)
		{
			 
			if(!m_pDB.get())
				return IFeatureClassPtr();

			if(sName.isEmpty())
			{
				//m_pDB->SetErrorText(L"Empty table Name");
				return IFeatureClassPtr();
			}

			IFeatureClassPtr pFC = GetFeatureClass(sName);
			if(pFC.get())
				return pFC;


			CEmbDBFeatureClass *pEmbDBFC = new CEmbDBFeatureClass(this, sName, sName);
			if(!pEmbDBFC->open())
			{
				delete pEmbDBFC;
				return IFeatureClassPtr();
			}			

			AddDataset(pEmbDBFC);
			return IFeatureClassPtr(pEmbDBFC);
		}
	}
}