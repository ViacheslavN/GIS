#ifndef _DISPLAY_LIB_LIB_H__
#define _DISPLAY_LIB_LIB_H__


#if defined(DISPLAY_TMP_LIBPREFIX) || defined(DISPLAY_TMP_LIBSUFFIX_DBG) \
	|| defined(DISPLAY_TMP_LIBSUFFIX_UNC) || defined(DISPLAY_TMP_LIBSUFFIX_CRT) \
	||defined(DISPLAY_TMP_PLATFORM) \
	|| defined(DISPLAY_TMP_LIBNAME)
# error Required macroses already defined
#endif
#define DISPLAY_LIB_VER_STRING ""


#if defined(_WIN32_WCE)
# define DISPLAY_TMP_PLATFORM "wce_"
#elif defined(_WIN32)
# define  DISPLAY_TMP_PLATFORM "32"
#elif defined(WIN64)
# define  DISPLAY_TMP_PLATFORM "64"
#else
# error Unknown platform
#endif


#if defined(_DEBUG)
# define DISPLAY_TMP_LIBSUFFIX_DBG "d"
#else
# define DISPLAY_TMP_LIBSUFFIX_DBG "r"
#endif

//#if defined(_UNICODE)
//# define DISPLAY_TMP_LIBSUFFIX_UNC "u"
//#else
# define DISPLAY_TMP_LIBSUFFIX_UNC ""
//#endif

#if defined(_DLL)
# define DISPLAY_TMP_LIBSUFFIX_CRT "x"
#else
# define DISPLAY_TMP_LIBSUFFIX_CRT "s"
#endif

//#if defined (DISPLAY_STATIC_LIB)
# define DISPLAY_TMP_LIBPREFIX "Lib"
//#else
//# define DISPLAY_TMP_LIBPREFIX ""
//#endif

#define DISPLAY_TMP_LIBNAME \
	DISPLAY_TMP_LIBPREFIX "Display"/* DISPLAY_TMP_CCVER */DISPLAY_TMP_PLATFORM"_" \
	DISPLAY_TMP_LIBSUFFIX_DBG DISPLAY_TMP_LIBSUFFIX_UNC DISPLAY_TMP_LIBSUFFIX_CRT".lib"

#pragma message ("Automatical linking with " DISPLAY_TMP_LIBNAME)

#pragma comment(lib, DISPLAY_TMP_LIBNAME)

#undef DISPLAY_TMP_LIBPREFIX
#undef DISPLAY_TMP_LIBSUFFIX_DBG
#undef DISPLAY_TMP_LIBSUFFIX_UNC
#undef DISPLAY_TMP_LIBSUFFIX_CRT

#undef DISPLAY_TMP_PLATFORM
#undef DISPLAY_TMP_LIBNAME
#else

#endif

