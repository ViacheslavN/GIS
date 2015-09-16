#ifndef GIS_ENGINE_GEO_DATABASE_UTILS_H
#define GIS_ENGINE_GEO_DATABASE_UTILS_H

#include "GeoDatabase.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
		namespace GeoDatabaseUtils
		{
			CommonLib::CVariant GetVariantFromString(eDataTypes type, const CommonLib::CString& sData);
		}
	}
}

#endif