#ifndef _GIS_GISCOMMON_LIB_H__
#define _GIS_GISCOMMON_LIB_H__


#if defined(GISCOMMON_TMP_LIBPREFIX) || defined(GISCOMMON_TMP_LIBSUFFIX_DBG) \
	|| defined(GISCOMMON_TMP_LIBSUFFIX_UNC) || defined(GISCOMMON_TMP_LIBSUFFIX_CRT) \
	||defined(GISCOMMON_TMP_PLATFORM) \
	|| defined(GISCOMMON_TMP_LIBNAME)
# error Required macroses already defined
#endif
#define GISCOMMON_LIB_VER_STRING ""


#if defined(_WIN32_WCE)
# define GISCOMMON_TMP_PLATFORM "wce_"
#elif defined(_WIN32)
# define  GISCOMMON_TMP_PLATFORM "32"
#elif defined(WIN64)
# define  GISCOMMON_TMP_PLATFORM "64"
#else
# error Unknown platform
#endif


#if defined(_DEBUG)
# define GISCOMMON_TMP_LIBSUFFIX_DBG "d"
#else
# define GISCOMMON_TMP_LIBSUFFIX_DBG "r"
#endif

//#if defined(_UNICODE)
//# define GISCOMMON_TMP_LIBSUFFIX_UNC "u"
//#else
# define GISCOMMON_TMP_LIBSUFFIX_UNC ""
//#endif

#if defined(_DLL)
# define GISCOMMON_TMP_LIBSUFFIX_CRT "x"
#else
# define GISCOMMON_TMP_LIBSUFFIX_CRT "s"
#endif

//#if defined (GISCOMMON_STATIC_LIB)
# define GISCOMMON_TMP_LIBPREFIX "Lib"
//#else
//# define GISCOMMON_TMP_LIBPREFIX ""
//#endif

#define GISCOMMON_TMP_LIBNAME \
	GISCOMMON_TMP_LIBPREFIX "GisCommon"/* GISCOMMON_TMP_CCVER */GISCOMMON_TMP_PLATFORM"_" \
	GISCOMMON_TMP_LIBSUFFIX_DBG GISCOMMON_TMP_LIBSUFFIX_UNC GISCOMMON_TMP_LIBSUFFIX_CRT".lib"

#pragma message ("Automatical linking with " GISCOMMON_TMP_LIBNAME)

#pragma comment(lib, GISCOMMON_TMP_LIBNAME)

#undef GISCOMMON_TMP_LIBPREFIX
#undef GISCOMMON_TMP_LIBSUFFIX_DBG
#undef GISCOMMON_TMP_LIBSUFFIX_UNC
#undef GISCOMMON_TMP_LIBSUFFIX_CRT

#undef GISCOMMON_TMP_PLATFORM
#undef GISCOMMON_TMP_LIBNAME
#else

#endif

