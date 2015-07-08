#ifndef GIS_ENGINE_GEOMETRY_CODE_PROJ_
#define GIS_ENGINE_GEOMETRY_CODE_PROJ_

#include "Common/Common.h"
namespace GisEngine
{
	namespace Geometry
	{
		CommonLib::str_t CodeToProj4Str(int code);
		int Proj4StrToCode(const CommonLib::str_t& sProj4Str);
	}
}
#endif