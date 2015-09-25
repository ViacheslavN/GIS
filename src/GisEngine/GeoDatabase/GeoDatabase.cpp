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
		bool LoadWks(CommonLib::IReadStream *pStream)
		{
			return CWorkspaceHolder::LoadWks(pStream);
		}

		bool SaveWks(GisCommon::IXMLNode *pXML)
		{
			return CWorkspaceHolder::SaveWks(pXML);
		}
		bool LoadWks(GisCommon::IXMLNode *pXML)
		{
			return CWorkspaceHolder::LoadWks(pXML);
		}
	}
}