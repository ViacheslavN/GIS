#ifndef GIS_ENGINE_LOADER_SYMBOLS_H_
#define GIS_ENGINE_LOADER_SYMBOLS_H_

#include "Display.h"




namespace GisEngine
{
	namespace Display
	{
		class LoaderSymbol
		{
		public:
			LoaderSymbol(){}
			~LoaderSymbol(){}
			static ISymbolPtr LoadSymbol(CommonLib::IReadStream *pSteam);
			static ISymbolPtr LoadSymbol(GisCommon::IXMLNode *pNode);
		};

	}
}

#endif