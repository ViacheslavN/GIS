#ifndef _GIS_CARTOGRAPHY_LIB_H__
#define _GIS_CARTOGRAPHY_LIB_H__


#if defined(CARTOGRAPHY_TMP_LIBPREFIX) || defined(CARTOGRAPHY_TMP_LIBSUFFIX_DBG) \
	|| defined(CARTOGRAPHY_TMP_LIBSUFFIX_UNC) || defined(CARTOGRAPHY_TMP_LIBSUFFIX_CRT) \
	||defined(CARTOGRAPHY_TMP_PLATFORM) \
	|| defined(CARTOGRAPHY_TMP_LIBNAME)
# error Required macroses already defined
#endif
#define CARTOGRAPHY_LIB_VER_STRING ""


#if defined(_WIN32_WCE)
# define CARTOGRAPHY_TMP_PLATFORM "wce_"
#elif defined(_WIN32)
# define  CARTOGRAPHY_TMP_PLATFORM "32"
#elif defined(WIN64)
# define  CARTOGRAPHY_TMP_PLATFORM "64"
#else
# error Unknown platform
#endif


#if defined(_DEBUG)
# define CARTOGRAPHY_TMP_LIBSUFFIX_DBG "d"
#else
# define CARTOGRAPHY_TMP_LIBSUFFIX_DBG "r"
#endif

//#if defined(_UNICODE)
//# define CARTOGRAPHY_TMP_LIBSUFFIX_UNC "u"
//#else
# define CARTOGRAPHY_TMP_LIBSUFFIX_UNC ""
//#endif

#if defined(_DLL)
# define CARTOGRAPHY_TMP_LIBSUFFIX_CRT "x"
#else
# define CARTOGRAPHY_TMP_LIBSUFFIX_CRT "s"
#endif

//#if defined (CARTOGRAPHY_STATIC_LIB)
# define CARTOGRAPHY_TMP_LIBPREFIX "Lib"
//#else
//# define CARTOGRAPHY_TMP_LIBPREFIX ""
//#endif

#define CARTOGRAPHY_TMP_LIBNAME \
	CARTOGRAPHY_TMP_LIBPREFIX "Cartography"/* CARTOGRAPHY_TMP_CCVER */CARTOGRAPHY_TMP_PLATFORM"_" \
	CARTOGRAPHY_TMP_LIBSUFFIX_DBG CARTOGRAPHY_TMP_LIBSUFFIX_UNC CARTOGRAPHY_TMP_LIBSUFFIX_CRT".lib"

#pragma message ("Automatical linking with " CARTOGRAPHY_TMP_LIBNAME)

#pragma comment(lib, CARTOGRAPHY_TMP_LIBNAME)

#undef CARTOGRAPHY_TMP_LIBPREFIX
#undef CARTOGRAPHY_TMP_LIBSUFFIX_DBG
#undef CARTOGRAPHY_TMP_LIBSUFFIX_UNC
#undef CARTOGRAPHY_TMP_LIBSUFFIX_CRT

#undef CARTOGRAPHY_TMP_PLATFORM
#undef CARTOGRAPHY_TMP_LIBNAME
#else

#endif

