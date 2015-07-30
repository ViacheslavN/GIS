#ifndef GIS_ENGINE_GEO_DATABASE_SHAPE_FILE_UTILS_H_
#define GIS_ENGINE_GEO_DATABASE_SHAPE_FILE_UTILS_H_


#include "GeoDatabase.h"
#include "shapelib/shapefil.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
		namespace ShapefileUtils
		{
			CommonLib::str_t  NormalizePath(const CommonLib::str_t& path);




			struct SHPGuard
			{
				shapelib::SHPHandle file;
				SHPGuard()
					: file(NULL)
				{}
				~SHPGuard()
				{
					clear();
				}
				void clear()
				{
					if(file)
						shapelib::SHPClose(file);
					file = NULL;
				}
			};

			struct DBFGuard
			{
				shapelib::DBFHandle file;
				DBFGuard()
					: file(NULL)
				{}
				~DBFGuard()
				{
					clear();
				}
				void clear()
				{
					if(file)
						shapelib::DBFClose(file);
					file = NULL;
				}
			};

		}
	}
}

#endif