#ifndef GIS_ENGINE_CARTOGRAPHY_LOADER_LAYERS_H_
#define GIS_ENGINE_CARTOGRAPHY_LOADER_LAYERS_H_

#include "Cartography.h"

namespace GisEngine
{
	namespace Cartography
	{
		class LoaderLayers
		{
		public:
			static	ILayerPtr LoadLayer(CommonLib::IReadStream *pSteam);
			static	ILayerPtr LoadLayer(const GisCommon::IXMLNode *pNode);
		};
	}
}

#endif