#ifndef _GIS_GEODATABASE_LIB_H__
#define _GIS_GEODATABASE_LIB_H__


#if defined(GEODATABASE_TMP_LIBPREFIX) || defined(GEODATABASE_TMP_LIBSUFFIX_DBG) \
	|| defined(GEODATABASE_TMP_LIBSUFFIX_UNC) || defined(GEODATABASE_TMP_LIBSUFFIX_CRT) \
	||defined(GEODATABASE_TMP_PLATFORM) \
	|| defined(GEODATABASE_TMP_LIBNAME)
# error Required macroses already defined
#endif
#define GEODATABASE_LIB_VER_STRING ""


#if defined(_WIN32_WCE)
# define GEODATABASE_TMP_PLATFORM "wce_"
#elif defined(WIN64)
# define  GEODATABASE_TMP_PLATFORM "64"
#elif defined(_WIN32)
# define  GEODATABASE_TMP_PLATFORM "32"
#else
# error Unknown platform
#endif


#if defined(_DEBUG)
# define GEODATABASE_TMP_LIBSUFFIX_DBG "d"
#else
# define GEODATABASE_TMP_LIBSUFFIX_DBG "r"
#endif

//#if defined(_UNICODE)
//# define GEODATABASE_TMP_LIBSUFFIX_UNC "u"
//#else
# define GEODATABASE_TMP_LIBSUFFIX_UNC ""
//#endif

#if defined(_DLL)
# define GEODATABASE_TMP_LIBSUFFIX_CRT "x"
#else
# define GEODATABASE_TMP_LIBSUFFIX_CRT "s"
#endif

//#if defined (GEODATABASE_STATIC_LIB)
# define GEODATABASE_TMP_LIBPREFIX "Lib"
//#else
//# define GEODATABASE_TMP_LIBPREFIX ""
//#endif

#define GEODATABASE_TMP_LIBNAME \
	GEODATABASE_TMP_LIBPREFIX "GeoDatabase"/* GEODATABASE_TMP_CCVER */GEODATABASE_TMP_PLATFORM"_" \
	GEODATABASE_TMP_LIBSUFFIX_DBG GEODATABASE_TMP_LIBSUFFIX_UNC GEODATABASE_TMP_LIBSUFFIX_CRT".lib"

#pragma message ("Automatical linking with " GEODATABASE_TMP_LIBNAME)

#pragma comment(lib, GEODATABASE_TMP_LIBNAME)

#undef GEODATABASE_TMP_LIBPREFIX
#undef GEODATABASE_TMP_LIBSUFFIX_DBG
#undef GEODATABASE_TMP_LIBSUFFIX_UNC
#undef GEODATABASE_TMP_LIBSUFFIX_CRT

#undef GEODATABASE_TMP_PLATFORM
#undef GEODATABASE_TMP_LIBNAME
#else

#endif

