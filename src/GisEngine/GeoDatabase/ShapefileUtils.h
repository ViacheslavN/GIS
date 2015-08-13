#ifndef GIS_ENGINE_GEO_DATABASE_SHAPE_FILE_UTILS_H_
#define GIS_ENGINE_GEO_DATABASE_SHAPE_FILE_UTILS_H_


#include "GeoDatabase.h"
#include "ShapeLib/shapefil.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
		namespace ShapefileUtils
		{
			CommonLib::str_t  NormalizePath(const CommonLib::str_t& path);
			CommonLib::eShapeType SHPTypeToGeometryType(int shpType, bool* hasZ = NULL, bool* hasM = NULL);
			eDataTypes SHPFieldInfoToFieldInfo(ShapeLib::DBFFieldType ftype, int width, int dec, int* length, int* precision, int* scale);
			void SHPObjectToGeometry(ShapeLib::SHPObject* obj, CommonLib::CGeoShape& result);


			struct SHPGuard
			{
				ShapeLib::SHPHandle file;
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
						ShapeLib::SHPClose(file);
					file = NULL;
				}
			};

			struct DBFGuard
			{
				ShapeLib::DBFHandle file;
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
						ShapeLib::DBFClose(file);
					file = NULL;
				}
			};

		}
	}
}

#endif