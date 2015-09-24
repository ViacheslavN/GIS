#ifndef GIS_ENGINE_CARTOGRAPHY_LOADER_RENDERERS_H_
#define GIS_ENGINE_CARTOGRAPHY_LOADER_RENDERERS_H_

#include "Cartography.h"

namespace GisEngine
{
	namespace Cartography
	{
		class  LoaderRenderers
		{
		public:
			static IFeatureRendererPtr LoadRenderer(CommonLib::IReadStream *pSteam);
			static IFeatureRendererPtr LoadRenderer(GisCommon::IXMLNode *pNode);
		};
	
	}
}

#endif