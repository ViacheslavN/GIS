#ifndef _AGG_LIB_LIB_H__
#define _AGG_LIB_LIB_H__
 

#if defined(AGG_TMP_LIBPREFIX) || defined(AGG_TMP_LIBSUFFIX_DBG) \
	|| defined(AGG_TMP_LIBSUFFIX_UNC) || defined(AGG_TMP_LIBSUFFIX_CRT) \
	||defined(AGG_TMP_PLATFORM) \
	|| defined(AGG_TMP_LIBNAME)
# error Required macroses already defined
#endif
#define AGG_LIB_VER_STRING ""


#if defined(_WIN32_WCE)
# define AGG_TMP_PLATFORM "wce_"
#elif defined(WIN64)
# define  AGG_TMP_PLATFORM "64"
#elif defined(_WIN32)
# define  AGG_TMP_PLATFORM "32"
#else
# error Unknown platform
#endif


#if defined(_DEBUG)
# define AGG_TMP_LIBSUFFIX_DBG "d"
#else
# define AGG_TMP_LIBSUFFIX_DBG "r"
#endif

//#if defined(_UNICODE)
//# define AGG_TMP_LIBSUFFIX_UNC "u"
//#else
# define AGG_TMP_LIBSUFFIX_UNC ""
//#endif

#if defined(_DLL)
# define AGG_TMP_LIBSUFFIX_CRT "x"
#else
# define AGG_TMP_LIBSUFFIX_CRT "s"
#endif

//#if defined (AGG_STATIC_LIB)
# define AGG_TMP_LIBPREFIX "Lib"
//#else
//# define AGG_TMP_LIBPREFIX ""
//#endif

#define AGG_TMP_LIBNAME \
	AGG_TMP_LIBPREFIX "Agg"/* AGG_TMP_CCVER */AGG_TMP_PLATFORM"_" \
	AGG_TMP_LIBSUFFIX_DBG AGG_TMP_LIBSUFFIX_UNC AGG_TMP_LIBSUFFIX_CRT".lib"

#pragma message ("Automatical linking with " AGG_TMP_LIBNAME)

#pragma comment(lib, AGG_TMP_LIBNAME)

#undef AGG_TMP_LIBPREFIX
#undef AGG_TMP_LIBSUFFIX_DBG
#undef AGG_TMP_LIBSUFFIX_UNC
#undef AGG_TMP_LIBSUFFIX_CRT
//#undef AGG_TMP_CCVER
#undef AGG_TMP_PLATFORM
#undef AGG_TMP_LIBNAME
#else
 
#endif

