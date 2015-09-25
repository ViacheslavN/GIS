#include "stdafx.h"
#include "WorkspaceBase.h"
#include "LoaderWorkspase.h"
namespace GisEngine
{
	namespace GeoDatabase
	{

		
		CWorkspaceHolder::TWksMap CWorkspaceHolder::m_wksMap;
		CWorkspaceHolder::TWksMapByID CWorkspaceHolder::m_wksMapByID;
		int32 CWorkspaceHolder::m_nWksID = 1;

		CommonLib::CSSection CWorkspaceHolder::m_SharedMutex;

		IWorkspacePtr CWorkspaceHolder::GetWorkspace(uint32 nID)
		{
			CommonLib::CSSection::scoped_lock lock (m_SharedMutex);
		 
			 TWksMapByID::iterator it = m_wksMapByID.find(nID);
			 if(it != m_wksMapByID.end())
				 return it->second;

			 return IWorkspacePtr();
		}

		IWorkspacePtr CWorkspaceHolder::GetWorkspace(eWorkspaceType WorkspaceType, const CommonLib::CString& sHash)
		{
			CommonLib::CSSection::scoped_lock lock (m_SharedMutex);
			SWksInfo wksInfo(WorkspaceType, sHash);
			TWksMap::iterator it = m_wksMap.find(wksInfo);
			if(it != m_wksMap.end())
				return it->second;

			return IWorkspacePtr();
		}
 
		void CWorkspaceHolder::AddWorkspace(IWorkspace* pWks)
		{
			CommonLib::CSSection::scoped_lock lock (m_SharedMutex);
			SWksInfo wksInfo(pWks->GetWorkspaceType(), pWks->GetHash());
			TWksMap::iterator it = m_wksMap.find(wksInfo);
			if(it == m_wksMap.end())
			{
				m_wksMap.insert(std::make_pair(wksInfo, IWorkspacePtr(pWks)));
				m_wksMapByID.insert(std::make_pair(pWks->GetID(),IWorkspacePtr(pWks)));
			}
		}
		void CWorkspaceHolder::RemoveWorkspace(eWorkspaceType WorkspaceType, const CommonLib::CString& sHash)
		 {
			 CommonLib::CSSection::scoped_lock lock (m_SharedMutex);
			 SWksInfo wksInfo(WorkspaceType, sHash);
			 TWksMap::iterator it = m_wksMap.find(wksInfo);
			 if(it != m_wksMap.end())
			 {
				 IWorkspacePtr pWks = it->second;
				 assert(pWks.get());
				 m_wksMapByID.erase(pWks->GetID());
				 m_wksMap.erase(it);
			 }
		 }
		void CWorkspaceHolder::RemoveWorkspace(uint32 nID)
		{
			CommonLib::CSSection::scoped_lock lock (m_SharedMutex);
			TWksMapByID::iterator it = m_wksMapByID.find(nID);
			if(it != m_wksMapByID.end())
			{
				IWorkspacePtr pWks = it->second;
				SWksInfo wksInfo(pWks->GetWorkspaceType(), pWks->GetHash());
				m_wksMapByID.erase(it);
				m_wksMap.erase(wksInfo);
			}
		}
		int32 CWorkspaceHolder::GetIDWorkspace()
		{
			CommonLib::CSSection::scoped_lock lock (m_SharedMutex); 
			return m_nWksID++;
		}
		void CWorkspaceHolder::SetLastID(uint32 nID)
		{
			CommonLib::CSSection::scoped_lock lock (m_SharedMutex); 
			m_nWksID = nID;
		}

		bool CWorkspaceHolder::SaveWks(CommonLib::IWriteStream *pStream)
		{
			CommonLib::CSSection::scoped_lock lock (m_SharedMutex); 
			CommonLib::MemoryStream stream;
			stream.write(m_nWksID);
			stream.write((uint32)m_wksMapByID.size());
			TWksMapByID::iterator it = m_wksMapByID.begin();
			TWksMapByID::iterator end = m_wksMapByID.end();
			for (; it != end; ++it)
			{
				IWorkspacePtr pWks = it->second;
				pWks->save(pStream);
			}
			pStream->write(&stream);
			return true;
		}
		bool CWorkspaceHolder::LoadWks(CommonLib::IReadStream *pStream)
		{
			CommonLib::CSSection::scoped_lock lock (m_SharedMutex); 
			CommonLib::FxMemoryReadStream stream;
			pStream->AttachStream(&stream, pStream->readIntu32());
			m_nWksID = stream.readIntu32();
			uint32 nSize = stream.readIntu32();
			for(uint32 i = 0; i < nSize; ++i)
			{
				IWorkspacePtr pWks = LoaderWorkspace::LoadWorkspace(&stream);
				if(pWks.get())
					AddWorkspace(pWks.get());
			}


			return true;
		}

		bool CWorkspaceHolder::SaveWks(GisCommon::IXMLNode *pNode)
		{
			CommonLib::CSSection::scoped_lock lock (m_SharedMutex);
			GisCommon::IXMLNodePtr pWksNode = pNode->CreateChildNode("Wks");
			pWksNode->AddPropertyInt32U(L"WksID", m_nWksID);
			TWksMapByID::iterator it = m_wksMapByID.begin();
			TWksMapByID::iterator end = m_wksMapByID.end();
			for (; it != end; ++it)
			{
				IWorkspacePtr pWks = it->second;
				GisCommon::IXMLNodePtr pNodeWks =  pWksNode->CreateChildNode(L"Workspace");
				pWks->saveXML(pNodeWks.get());
			 
			}
			return true;
		}
		bool CWorkspaceHolder::LoadWks(const GisCommon::IXMLNode *pNode)
		{
			CommonLib::CSSection::scoped_lock lock (m_SharedMutex);
			GisCommon::IXMLNodePtr pWksNode = pNode->GetChild(L"Wks");
			if(!pWksNode.get())
				return false;

			m_nWksID = pWksNode->GetPropertyInt32U(L"WksID", 1);
			int nCnt = pWksNode->GetChildCnt();
			for(int i = 0; i < nCnt; ++i)
			{
				GisCommon::IXMLNodePtr pChildNode = pWksNode->GetChild(i);
				if(pChildNode->GetName() != L"Workspace")
					continue;
				IWorkspacePtr pWks = LoaderWorkspace::LoadWorkspace(pChildNode.get());
				if(pWks.get())
					AddWorkspace(pWks.get());

			}

			return true;
		}
	}
}
