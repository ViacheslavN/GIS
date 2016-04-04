#include "stdafx.h"
#include "LoaderSymbols.h"
#include "SimpleLineSymbol.h"
#include "SimpleFillSymbol.h"
#include "SimpleMarketSymbol.h"
#include "TextSymbol.h"
#include "CommonLibrary/stream.h"

namespace GisEngine
{
	namespace Display
	{

		template<class TSerealizer>
		ISymbolPtr LoadSymbolT(TSerealizer *pSerealizer, uint32 nSymbolID);
		
		ISymbolPtr LoaderSymbol::LoadSymbol(CommonLib::IReadStream *pSteam)
		{
		//	CommonLib::FxMemoryReadStream stream;
		//	pSteam->AttachStream(&stream, pSteam->readIntu32());
			uint32 nSymbolID = pSteam->readIntu32();
			if(nSymbolID == UndefineSymbolID)
				return ISymbolPtr();

			return LoadSymbolT<CommonLib::IReadStream>(pSteam, nSymbolID);
		}

		ISymbolPtr LoaderSymbol::LoadSymbol(const GisCommon::IXMLNode *pNode)
		{

			uint32 nSymbolID = pNode->GetPropertyInt32U(L"SymbolID", UndefineSymbolID);
			if(nSymbolID == UndefineSymbolID)
				return ISymbolPtr();

			return LoadSymbolT<const GisCommon::IXMLNode>(pNode, nSymbolID);
		}

		template<class TSerealizer>
		ISymbolPtr LoadSymbolT(TSerealizer *pSerealizer, uint32 nSymbolID)
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
			case TextSymbolID:
				{
					CTextSymbol *pTextSymbol = new CTextSymbol();
					pTextSymbol->load(pSerealizer);
					pSymbol = pTextSymbol;
				}
				break;
			case SimpleMarketSymbolID:
				{
					CSimpleMarketSymbol *pMarketSymbol = new CSimpleMarketSymbol();
					pMarketSymbol->load(pSerealizer);
					pSymbol = pMarketSymbol;
				}
				break;
			}

			return pSymbol;
		}
	}
}
