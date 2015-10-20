#include "stdafx.h"
#include "EmbDBWorkspace.h"
#include "../../EmbDB/EmbDB/Database.h"
#include "CommonLibrary/File.h"

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

		bool CEmbDBWorkspace::create(const CommonLib::CString& sFullName)
		{
			if(m_pDB.get())
				m_pDB->close();
			m_pDB.reset(new embDB::CDatabase());
			if(!m_pDB->create(sFullName.cwstr(),  8192))
			{
				//TO DO Error
				return false;
			}

			return true;
		}
	}
}