#include "stdafx.h"
#include "LoaderWorkspase.h"
#include "ShapefileWorkspace.h"
#include "SQLiteWorkspace.h"
#include "EmbDBWorkspace.h"

namespace GisEngine
{
	namespace GeoDatabase
	{

		template<class TSerealizer>
		IWorkspacePtr LoadWorkspaceT(TSerealizer *pSerealizer, uint32 nSymbolID);

		IWorkspacePtr LoaderWorkspace::LoadWorkspace(CommonLib::IReadStream *pStream)
		{
 
			uint32 nWksType = pStream->readIntu32();
			if(nWksType == wtUndefined)
				return IWorkspacePtr();

			return LoadWorkspaceT<CommonLib::IReadStream>(pStream, nWksType);
		}

		IWorkspacePtr LoaderWorkspace::LoadWorkspace(GisCommon::IXMLNode *pNode)
		{

			uint32 nWksType = pNode->GetPropertyInt32U(L"WksType", wtUndefined);
			if(nWksType == wtUndefined)
				return IWorkspacePtr();

			return LoadWorkspaceT<GisCommon::IXMLNode>(pNode, nWksType);
		}

		template<class TSerealizer>
		IWorkspacePtr LoadWorkspaceT(TSerealizer *pSerealizer, uint32 nWksType)
		{
			IWorkspacePtr pWorkspace;
			switch(nWksType)
			{
			case wtShapeFile:
				{
				 	pWorkspace = CShapefileWorkspace::Open(pSerealizer);
				}
				break;
			case wtSqlLite:
				{
					pWorkspace = CSQLiteWorkspace::Open(pSerealizer);
				}
				break;
			case wtEmbDB:
				{
					pWorkspace = CEmbDBWorkspace::Open(pSerealizer);
				}
				break;
			}

			return pWorkspace;
		}
	}
}
