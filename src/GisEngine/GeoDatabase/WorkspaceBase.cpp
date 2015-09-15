#include "stdafx.h"
#include "WorkspaceBase.h"

namespace GisEngine
{
	namespace GeoDatabase
	{

		
		CWorkspaceHolder::TWksMap CWorkspaceHolder::m_wksMap;
		CommonLib::CSSection CWorkspaceHolder::m_SharedMutex;

		IWorkspacePtr CWorkspaceHolder::GetWorkspace(eWorkspaceType WorkspaceType, const CommonLib::CString& sID)
		{
			CommonLib::CSSection::scoped_lock lock (m_SharedMutex);
			SWksInfo wksInfo(WorkspaceType, sID);
			 TWksMap::iterator it = m_wksMap.find(wksInfo);
			 if(it != m_wksMap.end())
				 return it->second;

			 return IWorkspacePtr();
		}
 
		void CWorkspaceHolder::AddWorkspace(IWorkspace* pWks,  const CommonLib::CString& sID)
		{
			CommonLib::CSSection::scoped_lock lock (m_SharedMutex);
			SWksInfo wksInfo(pWks->GetWorkspaceType(), sID);
			TWksMap::iterator it = m_wksMap.find(wksInfo);
			if(it == m_wksMap.end())
			{
				m_wksMap.insert(std::make_pair(wksInfo, IWorkspacePtr(pWks)));
			}
		}
		void CWorkspaceHolder::RemoveWorkspace(eWorkspaceType WorkspaceType, const CommonLib::CString& sID)
		 {
			 CommonLib::CSSection::scoped_lock lock (m_SharedMutex);
			 SWksInfo wksInfo(WorkspaceType, sID);
			 m_wksMap.erase(wksInfo);

		 }
	}
}
