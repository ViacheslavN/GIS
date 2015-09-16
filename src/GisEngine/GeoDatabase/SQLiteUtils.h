#ifndef GIS_ENGINE_GEO_DATABASE_SQLITE_UTILS_H_
#define GIS_ENGINE_GEO_DATABASE_SQLITE_UTILS_H_


#include "GeoDatabase.h"

namespace GisEngine
{
	namespace GeoDatabase
	{
		namespace SQLitefUtils
		{
			void FieldType2SQLiteType(eDataTypes type, CommonLib::CString& sSQLiteType);
		}
	}
}
#endif