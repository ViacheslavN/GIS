#ifndef _GIS_FRAMEWORK_LIB_H__
#define _GIS_FRAMEWORK_LIB_H__


#if defined(FRAMEWORK_TMP_LIBPREFIX) || defined(FRAMEWORK_TMP_LIBSUFFIX_DBG) \
	|| defined(FRAMEWORK_TMP_LIBSUFFIX_UNC) || defined(FRAMEWORK_TMP_LIBSUFFIX_CRT) \
	||defined(FRAMEWORK_TMP_PLATFORM) \
	|| defined(FRAMEWORK_TMP_LIBNAME)
# error Required macroses already defined
#endif
#define FRAMEWORK_LIB_VER_STRING ""


#if defined(_WIN32_WCE)
# define FRAMEWORK_TMP_PLATFORM "wce_"
#elif defined(WIN64)
# define  FRAMEWORK_TMP_PLATFORM "64"
#elif defined(_WIN32)
# define  FRAMEWORK_TMP_PLATFORM "32"
#else
# error Unknown platform
#endif


#if defined(_DEBUG)
# define FRAMEWORK_TMP_LIBSUFFIX_DBG "d"
#else
# define FRAMEWORK_TMP_LIBSUFFIX_DBG "r"
#endif

//#if defined(_UNICODE)
//# define FRAMEWORK_TMP_LIBSUFFIX_UNC "u"
//#else
# define FRAMEWORK_TMP_LIBSUFFIX_UNC ""
//#endif

#if defined(_DLL)
# define FRAMEWORK_TMP_LIBSUFFIX_CRT "x"
#else
# define FRAMEWORK_TMP_LIBSUFFIX_CRT "s"
#endif

//#if defined (FRAMEWORK_STATIC_LIB)
# define FRAMEWORK_TMP_LIBPREFIX "Lib"
//#else
//# define FRAMEWORK_TMP_LIBPREFIX ""
//#endif

#define FRAMEWORK_TMP_LIBNAME \
	FRAMEWORK_TMP_LIBPREFIX "Framework"/* FRAMEWORK_TMP_CCVER */FRAMEWORK_TMP_PLATFORM"_" \
	FRAMEWORK_TMP_LIBSUFFIX_DBG FRAMEWORK_TMP_LIBSUFFIX_UNC FRAMEWORK_TMP_LIBSUFFIX_CRT".lib"

#pragma message ("Automatical linking with " FRAMEWORK_TMP_LIBNAME)

#pragma comment(lib, FRAMEWORK_TMP_LIBNAME)

#undef FRAMEWORK_TMP_LIBPREFIX
#undef FRAMEWORK_TMP_LIBSUFFIX_DBG
#undef FRAMEWORK_TMP_LIBSUFFIX_UNC
#undef FRAMEWORK_TMP_LIBSUFFIX_CRT

#undef FRAMEWORK_TMP_PLATFORM
#undef FRAMEWORK_TMP_LIBNAME
#else

#endif

