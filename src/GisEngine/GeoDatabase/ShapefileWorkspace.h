#ifndef GIS_ENGINE_GEO_DATABASE_SHAPE_WORK_SPACE_H_
#define GIS_ENGINE_GEO_DATABASE_SHAPE_WORK_SPACE_H_

#include "GeoDatabase.h"

namespace GisEngine
{
	namespace GeoDatabase
	{
		class CShapefileWorkspace : public IWorkspace
		{
			public:
				CShapefileWorkspace();
				CShapefileWorkspace(const wchar_t *pszPath);

			private:
		};
	}
}
#endif