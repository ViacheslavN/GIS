#ifndef GIS_ENGINE_LOADER_WORKSPASE_H_
#define GIS_ENGINE_LOADER_WORKSPASE_H_

#include "GeoDatabase.h"

namespace GisEngine
{
	namespace GeoDatabase
	{
		class LoaderWorkspace
		{
		public:
			LoaderWorkspace(){}
			~LoaderWorkspace(){}
			static IWorkspacePtr LoadWorkspace(CommonLib::IReadStream *pStream);
			static IWorkspacePtr LoadWorkspace(GisCommon::IXMLNode *pNode);
		};

	}
}

#endif