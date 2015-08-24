#ifndef GIS_ENGINE_LOADER_SYMBOLS_H_
#define GIS_ENGINE_LOADER_SYMBOLS_H_

#include "Display.h"




namespace GisEngine
{
	namespace Display
	{

		ISymbolPtr LoadSymbol(CommonLib::IReadStream *pSteam);
		ISymbolPtr LoadSymbol(GisCommon::IXMLNode *pNode);
	}
}

#endif