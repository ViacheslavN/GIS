#include "stdafx.h"
#include "ShapefileUtils.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
		namespace ShapefileUtils
		{
			CommonLib::str_t  NormalizePath(const CommonLib::str_t& path)
			{
				if(path.isEmpty())
					return path;

				CommonLib::str_t normalizedPath = path;
#ifdef WIN32
				normalizedPath.lower();
#endif
				if(normalizedPath[-1 + (int)normalizedPath.length()] != L'\\' &&
					normalizedPath[-1 + (int)normalizedPath.length()] != L'/')
				{
#ifdef WIN32
					normalizedPath += L"\\";
#else
					normalizedPath += L"/";
#endif
				}

				return normalizedPath;
			}
			eDataTypes SHPFieldInfoToFieldInfo(ShapeLib::DBFFieldType ftype, int width, int dec, int* length, int* precision, int* scale)
			{
				*length = 0;
				*precision = 0;
				*scale = 0;

				switch(ftype)
				{
				case ShapeLib::FTString:
					*length = width;
					return dtString;
				case ShapeLib::FTInteger:
					*precision = width - 1;
					return dtInteger32;
				case ShapeLib::FTDouble:
					*precision = width - 1;
					*scale = dec;
					return dtDouble;
				case ShapeLib::FTDate:
					return dtDate;
				}
				return dtUnknown;
			}

			CommonLib::eShapeType SHPTypeToGeometryType(int shpType, bool* hasZ, bool* hasM)
			{
				CommonLib::eShapeType geomType = CommonLib::shape_type_null;

				switch((size_t)shpType)
				{
				case SHPT_NULL:
					geomType = CommonLib::shape_type_null;
					break;
				case SHPT_POINT:
				case SHPT_POINTZ:
				case SHPT_POINTM:
					geomType = CommonLib::shape_type_point;
					break;
				case SHPT_ARC:
				case SHPT_ARCZ:
				case SHPT_ARCM:
					geomType = CommonLib::shape_type_polyline;
					break;
				case SHPT_POLYGON:
				case SHPT_POLYGONZ:
				case SHPT_POLYGONM:
					geomType = CommonLib::shape_type_polygon;
					break;
				case SHPT_MULTIPOINT:
				case SHPT_MULTIPOINTZ:
				case SHPT_MULTIPOINTM:
					geomType = CommonLib::shape_type_multipoint;
					break;
				case SHPT_MULTIPATCH:
					geomType = CommonLib::shape_type_multipatch;
					break;
				}

				if(hasZ)
				{ 
					if(shpType == SHPT_POINTZ || shpType == SHPT_ARCZ || 
						shpType == SHPT_POLYGONZ || shpType == SHPT_MULTIPOINTZ || 
						shpType == SHPT_MULTIPOINTZ)
						*hasZ = true;
					else
						*hasZ = false;
				}

				if(hasM)
				{ 
					if(shpType == SHPT_POINTM || shpType == SHPT_ARCM || 
						shpType == SHPT_POLYGONM || shpType == SHPT_MULTIPOINTM)
						*hasM = true;
					else
						*hasM = false;
				}

				return geomType;
			}
		}
	}
}