#ifndef _LIB_GIS_ENGINE_COMMON_
#define _LIB_GIS_ENGINE_COMMON_

#if defined(WIN32) || defined(WIN64)
#if !defined(_WIN32_WINNT) && !defined(_WIN32_WCE)
#define _WIN32_WINNT 0x0501
#endif
#include <windows.h>
#endif

#include "CommonLibrary/BoundaryBox.h"
#include "CommonLibrary/GeoShape.h"
#include "CommonLibrary/String.h"
#include "CommonLibrary/stream.h"
#include <vector>
#include <algorithm>
#include <map>

struct GisXYZPoint
{
	double dX;
	double dY;
	double dZ;
};

typedef CommonLib::GisXYPoint GisXYPoint;
 

#ifndef M_PI
#define M_PI            3.14159265358979323846
#endif
#define RAD2DEG(r) ((double)(r) * (180.0 / M_PI))
#define DEG2RAD(d) ((double)(d) * (M_PI / 180.0))

namespace GisEngine
{


#ifdef __IPHONE_3_1
	using decore::byte;
#include <cstddef>
	typedef unsigned char BYTE;
	typedef std::size_t SIZE;
	typedef wchar_t TCHAR;
#define _isnan(x)    \
	(   sizeof (x) == sizeof(float)    ?    __inline_isnanf((float)(x))    \
	:    sizeof (x) == sizeof(double)    ?    __inline_isnand((double)(x))    \
	:    __inline_isnan ((long double)(x)))
#ifndef _T
#define _T(a) L##a
#endif//_T
#include "decore/blob_t.h" // for agg
#include "decore/shp_t.h" // for agg
#endif//__IPHONE_3_1

#ifdef ANDROID
#include <android/log.h>
#include <cstddef>
	using decore::byte;

	typedef unsigned char BYTE;
	//typedef std::size_t SIZE;
	typedef wchar_t TCHAR;

	typedef struct tagSIZE
	{
		long        cx;
		long        cy;
	} SIZE, *PSIZE, *LPSIZE;

#endif

	typedef  CommonLib::bbox   GisBoundingBox;



}
#endif