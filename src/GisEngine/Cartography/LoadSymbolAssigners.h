#ifndef GIS_ENGINE_CARTOGRAPHY_LOADER_LAYERS_H_
#define GIS_ENGINE_CARTOGRAPHY_LOADER_LAYERS_H_

#include "Cartography.h"

namespace GisEngine
{
	namespace Cartography
	{

		ISymbolAssignerPtr LoadSymbolAssigners(CommonLib::IReadStream *pSteam);
		ISymbolAssignerPtr LoadSymbolAssigners(GisCommon::IXMLNode *pNode);
	}
}

#endif