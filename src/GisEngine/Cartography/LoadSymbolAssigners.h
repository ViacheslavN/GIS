#ifndef GIS_ENGINE_CARTOGRAPHY_LOADER_SYMBOL_ASSIGNERS_H_
#define GIS_ENGINE_CARTOGRAPHY_LOADER_SYMBOL_ASSIGNERS_H_

#include "Cartography.h"

namespace GisEngine
{
	namespace Cartography
	{
		class LoaderSymbolAssigners
		{
		public:
			static ISymbolAssignerPtr LoadSymbolAssigners(CommonLib::IReadStream *pSteam);
			static ISymbolAssignerPtr LoadSymbolAssigners(GisCommon::IXMLNode *pNode);
		};

	}
}

#endif