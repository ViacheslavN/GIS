#ifndef _COMMON_LIB_LIB_H__
#define _COMMON_LIB_LIB_H__


#if defined(COMMON_TMP_LIBPREFIX) || defined(COMMON_TMP_LIBSUFFIX_DBG) \
	|| defined(COMMON_TMP_LIBSUFFIX_UNC) || defined(COMMON_TMP_LIBSUFFIX_CRT) \
	||defined(COMMON_TMP_PLATFORM) \
	|| defined(COMMON_TMP_LIBNAME)
# error Required macroses already defined
#endif
#define COMMON_LIB_VER_STRING ""


#if defined(_WIN32_WCE)
# define COMMON_TMP_PLATFORM "wce_"
#elif defined(_WIN32)
# define  COMMON_TMP_PLATFORM "32"
#elif defined(WIN64)
# define  COMMON_TMP_PLATFORM "64"
#else
# error Unknown platform
#endif


#if defined(_DEBUG)
# define COMMON_TMP_LIBSUFFIX_DBG "d"
#else
# define COMMON_TMP_LIBSUFFIX_DBG "r"
#endif

//#if defined(_UNICODE)
//# define COMMON_TMP_LIBSUFFIX_UNC "u"
//#else
# define COMMON_TMP_LIBSUFFIX_UNC ""
//#endif

#if defined(_DLL)
# define COMMON_TMP_LIBSUFFIX_CRT "x"
#else
# define COMMON_TMP_LIBSUFFIX_CRT "s"
#endif

//#if defined (COMMON_STATIC_LIB)
# define COMMON_TMP_LIBPREFIX "Lib"
//#else
//# define COMMON_TMP_LIBPREFIX ""
//#endif

#define COMMON_TMP_LIBNAME \
	COMMON_TMP_LIBPREFIX "Common"/* COMMON_TMP_CCVER */COMMON_TMP_PLATFORM"_" \
	COMMON_TMP_LIBSUFFIX_DBG COMMON_TMP_LIBSUFFIX_UNC COMMON_TMP_LIBSUFFIX_CRT".lib"

#pragma message ("Automatical linking with " COMMON_TMP_LIBNAME)

#pragma comment(lib, COMMON_TMP_LIBNAME)

#undef COMMON_TMP_LIBPREFIX
#undef COMMON_TMP_LIBSUFFIX_DBG
#undef COMMON_TMP_LIBSUFFIX_UNC
#undef COMMON_TMP_LIBSUFFIX_CRT
//#undef COMMON_TMP_CCVER
#undef COMMON_TMP_PLATFORM
#undef COMMON_TMP_LIBNAME
#else

#endif

