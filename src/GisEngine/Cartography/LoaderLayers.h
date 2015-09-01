#ifndef GIS_ENGINE_CARTOGRAPHY_LOADER_LAYERS_H_
#define GIS_ENGINE_CARTOGRAPHY_LOADER_LAYERS_H_

#include "Cartography.h"

namespace GisEngine
{
	namespace Cartography
	{

		ILayerPtr LoadLayer(CommonLib::IReadStream *pSteam);
		ILayerPtr LoadLayer(GisCommon::IXMLNode *pNode);
	}
}

#endif