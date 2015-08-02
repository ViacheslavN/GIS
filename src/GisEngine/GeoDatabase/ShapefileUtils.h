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
			CommonLib::eShapeType SHPTypeToGeometryType(int shpType, bool* hasZ, bool* hasM);
			CommonLib::eDataTypes SHPFieldInfoToFieldInfo(DBFFieldType ftype, int width, int dec, int* length, int* precision, int* scale);


			struct SHPGuard
			{
				SHPHandle file;
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
						SHPClose(file);
					file = NULL;
				}
			};

			struct DBFGuard
			{
				DBFHandle file;
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
						DBFClose(file);
					file = NULL;
				}
			};

		}
	}
}

#endif