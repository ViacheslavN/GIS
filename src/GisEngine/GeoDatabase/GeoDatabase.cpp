#include "stdafx.h"
#include "WorkspaceBase.h"


namespace GisEngine
{
	namespace GeoDatabase
	{


		IWorkspacePtr IWorkspace::GetWorkspaceByID(uint32 nID)
		{
			return CWorkspaceHolder::GetWorkspace(nID);
		}

		bool IWorkspace::SaveWks(CommonLib::IWriteStream *pStream)
		{
			 
			return CWorkspaceHolder::SaveWks(pStream);
		}
		bool IWorkspace::LoadWks(CommonLib::IReadStream *pStream)
		{
			return CWorkspaceHolder::LoadWks(pStream);
		}

		bool IWorkspace::SaveWks(GisCommon::IXMLNode *pXML)
		{
			return CWorkspaceHolder::SaveWks(pXML);
		}
		bool IWorkspace::LoadWks(const GisCommon::IXMLNode *pXML)
		{
			return CWorkspaceHolder::LoadWks(pXML);
		}
	}
}