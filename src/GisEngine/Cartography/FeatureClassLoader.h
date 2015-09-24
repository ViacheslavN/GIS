#ifndef GIS_ENGINE_CARTOGRAPHY_LOADER_FEATURE_CLASS_H_
#define GIS_ENGINE_CARTOGRAPHY_LOADER_FEATURE_CLASS_H_

#include "Cartography.h"

namespace GisEngine
{
	namespace Cartography
	{
		class  LoaderFeatureClass
		{
		public:
			static GeoDatabase::IFeatureClassPtr LoadFeatureClass(CommonLib::IReadStream *pSteam);
			static GeoDatabase::IFeatureClassPtr LoadFeatureClass(GisCommon::IXMLNode *pNode);
		};

	}
}

#endif