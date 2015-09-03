#include "stdafx.h"
#include "LoadSymbolAssigners.h"
#include "SimpleSymbolAssigner.h"
namespace GisEngine
{
	namespace Cartography
	{

		template<class TSerealizer>
		ISymbolAssignerPtr LoadSymbolAssigners(TSerealizer *pSerealizer, uint32 nLayerID);

		ISymbolAssignerPtr LoadSymbolAssigners(CommonLib::IReadStream *pStream)
		{
			uint32 nSymbolID = UndefineSymbolAssignerID;
			SAFE_READ(pStream, nSymbolID);
			if(nSymbolID == UndefineSymbolAssignerID)
				return ISymbolAssignerPtr();

			return LoadSymbolAssigners<CommonLib::IReadStream>(pStream, nSymbolID);
		}

		ISymbolAssignerPtr LoadSymbolAssigners(GisCommon::IXMLNode *pNode)
		{

			uint32 nSymbolID = pNode->GetPropertyInt32U(L"SymbolID", UndefineSymbolAssignerID);
			if(nSymbolID == UndefineSymbolAssignerID)
				return ISymbolAssignerPtr();

			return LoadSymbolAssigners<GisCommon::IXMLNode>(pNode, nSymbolID);
		}

		template<class TSerealizer>
		ISymbolAssignerPtr LoadSymbolAssigners(TSerealizer *pSerealizer, uint32 nSymbolID)
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