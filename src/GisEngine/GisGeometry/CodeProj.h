#ifndef GIS_ENGINE_GEOMETRY_CODE_PROJ_
#define GIS_ENGINE_GEOMETRY_CODE_PROJ_

#include "Common/Common.h"
namespace GisEngine
{
	namespace GisGeometry
	{
		CommonLib::CString CodeToProj4Str(int code);
		int Proj4StrToCode(const CommonLib::CString& sProj4Str);
	}
}
#endif