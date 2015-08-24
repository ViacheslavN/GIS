#include "stdafx.h"
#include "LoaderSymbols.h"
#include "SimpleLineSymbol.h"
#include "SimpleFillSymbol.h"

namespace GisEngine
{
	namespace Display
	{

		template<class TSerealizer>
		ISymbolPtr LoadSymbol(TSerealizer *pSerealizer, uint32 nSymbolID);
		
		ISymbolPtr LoadSymbol(CommonLib::IReadStream *pSteam)
		{
			uint32 nSymbolID = UndefineSymbolID;
			SAFE_READ(pSteam, nSymbolID);
			if(nSymbolID == UndefineSymbolID)
				return ISymbolPtr();

			return LoadSymbol<CommonLib::IReadStream>(pSteam, nSymbolID);
		}

		ISymbolPtr LoadSymbol(GisCommon::IXMLNode *pNode)
		{

			uint32 nSymbolID = pNode->GetPropertyInt32U(L"SymbolID", UndefineSymbolID);
			if(nSymbolID == UndefineSymbolID)
				return ISymbolPtr();

			return LoadSymbol<GisCommon::IXMLNode>(pNode, nSymbolID);
		}

		template<class TSerealizer>
		ISymbolPtr readSymbol(TSerealizer *pSerealizer, uint32 nSymbolID)
		{
			ISymbolPtr pSymbol;
			switch(nSymbolID)
			{
			case SimpleLineSymbolID:
				{
					CSimpleLineSymbol *pSimpleLineSymbol = new CSimpleLineSymbol();
					pSimpleLineSymbol->load(pSerealizer);
					pSymbol = pSimpleLineSymbol;
				}
				break;
			case SimpleFillSymbolID:
				{
					CSimpleFillSymbol *pSimpleFillSymbol = new CSimpleFillSymbol();
					pSimpleFillSymbol->load(pSerealizer);
					pSymbol = pSimpleFillSymbol;
				}
				break;
			}

			return pSymbol;
		}
	}
}
