#ifndef _GIS_GEOMETRY_LIB_H__
#define _GIS_GEOMETRY_LIB_H__


#if defined(GEOMETRY_TMP_LIBPREFIX) || defined(GEOMETRY_TMP_LIBSUFFIX_DBG) \
	|| defined(GEOMETRY_TMP_LIBSUFFIX_UNC) || defined(GEOMETRY_TMP_LIBSUFFIX_CRT) \
	||defined(GEOMETRY_TMP_PLATFORM) \
	|| defined(GEOMETRY_TMP_LIBNAME)
# error Required macroses already defined
#endif
#define GEOMETRY_LIB_VER_STRING ""


#if defined(_WIN32_WCE)
# define GEOMETRY_TMP_PLATFORM "wce_"
#elif defined(WIN64)
# define  GEOMETRY_TMP_PLATFORM "64"
#elif defined(_WIN32)
# define  GEOMETRY_TMP_PLATFORM "32"
#else
# error Unknown platform
#endif


#if defined(_DEBUG)
# define GEOMETRY_TMP_LIBSUFFIX_DBG "d"
#else
# define GEOMETRY_TMP_LIBSUFFIX_DBG "r"
#endif

//#if defined(_UNICODE)
//# define GEOMETRY_TMP_LIBSUFFIX_UNC "u"
//#else
# define GEOMETRY_TMP_LIBSUFFIX_UNC ""
//#endif

#if defined(_DLL)
# define GEOMETRY_TMP_LIBSUFFIX_CRT "x"
#else
# define GEOMETRY_TMP_LIBSUFFIX_CRT "s"
#endif

//#if defined (GEOMETRY_STATIC_LIB)
# define GEOMETRY_TMP_LIBPREFIX "Lib"
//#else
//# define GEOMETRY_TMP_LIBPREFIX ""
//#endif

#define GEOMETRY_TMP_LIBNAME \
	GEOMETRY_TMP_LIBPREFIX "Geometry"/* GEOMETRY_TMP_CCVER */GEOMETRY_TMP_PLATFORM"_" \
	GEOMETRY_TMP_LIBSUFFIX_DBG GEOMETRY_TMP_LIBSUFFIX_UNC GEOMETRY_TMP_LIBSUFFIX_CRT".lib"

#pragma message ("Automatical linking with " GEOMETRY_TMP_LIBNAME)

#pragma comment(lib, GEOMETRY_TMP_LIBNAME)

#undef GEOMETRY_TMP_LIBPREFIX
#undef GEOMETRY_TMP_LIBSUFFIX_DBG
#undef GEOMETRY_TMP_LIBSUFFIX_UNC
#undef GEOMETRY_TMP_LIBSUFFIX_CRT

#undef GEOMETRY_TMP_PLATFORM
#undef GEOMETRY_TMP_LIBNAME
#else

#endif

