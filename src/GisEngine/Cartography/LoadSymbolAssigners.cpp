#include "stdafx.h"
#include "LoadSymbolAssigners.h"
#include "SimpleSymbolAssigner.h"
namespace GisEngine
{
	namespace Cartography
	{

		template<class TSerealizer>
		ISymbolAssignerPtr LoadSymbolAssignersT(TSerealizer *pSerealizer, uint32 nLayerID);

		ISymbolAssignerPtr LoaderSymbolAssigners::LoadSymbolAssigners(CommonLib::IReadStream *pStream)
		{
			uint32 nSymbolID = pStream->readIntu32();
			if(nSymbolID == UndefineSymbolAssignerID)
				return ISymbolAssignerPtr();

			return LoadSymbolAssignersT<CommonLib::IReadStream>(pStream, nSymbolID);
		}

		ISymbolAssignerPtr LoaderSymbolAssigners::LoadSymbolAssigners(GisCommon::IXMLNode *pNode)
		{

			uint32 nSymbolID = pNode->GetPropertyInt32U(L"SymbolID", UndefineSymbolAssignerID);
			if(nSymbolID == UndefineSymbolAssignerID)
				return ISymbolAssignerPtr();

			return LoadSymbolAssignersT<GisCommon::IXMLNode>(pNode, nSymbolID);
		}

		template<class TSerealizer>
		ISymbolAssignerPtr LoadSymbolAssignersT(TSerealizer *pSerealizer, uint32 nSymbolID)
		{
			ISymbolAssignerPtr pSymbol;

			switch(nSymbolID)
			{
				case SimpleSymbolAssignerID:
					{
						CSimpleSymbolAssigner* pSimpleSymbol = new CSimpleSymbolAssigner();
						pSimpleSymbol->load(pSerealizer);
						pSymbol = pSimpleSymbol;
					}
					break;
			}

			return pSymbol;
		}

	}
}