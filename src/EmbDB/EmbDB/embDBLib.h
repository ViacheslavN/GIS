#ifndef _EMB_DB_LIB_H__
#define _EMB_DB_LIB_H__


#if defined(EMB_DB_TMP_LIBPREFIX) || defined(EMB_DB_TMP_LIBSUFFIX_DBG) \
	|| defined(EMB_DB_TMP_LIBSUFFIX_UNC) || defined(EMB_DB_TMP_LIBSUFFIX_CRT) \
	||defined(EMB_DB_TMP_PLATFORM) \
	|| defined(EMB_DB_TMP_LIBNAME)
# error Required macroses already defined
#endif
#define EMB_DB_LIB_VER_STRING ""


#if defined(_WIN32_WCE)
# define EMB_DB_TMP_PLATFORM "wce_"
#elif defined(_WIN32)
# define  EMB_DB_TMP_PLATFORM "32"
#elif defined(WIN64)
# define  EMB_DB_TMP_PLATFORM "64"
#else
# error Unknown platform
#endif


#if defined(_DEBUG)
# define EMB_DB_TMP_LIBSUFFIX_DBG "d"
#else
# define EMB_DB_TMP_LIBSUFFIX_DBG "r"
#endif

//#if defined(_UNICODE)
//# define EMB_DB_TMP_LIBSUFFIX_UNC "u"
//#else
# define EMB_DB_TMP_LIBSUFFIX_UNC ""
//#endif

#if defined(_DLL)
# define EMB_DB_TMP_LIBSUFFIX_CRT "x"
#else
# define EMB_DB_TMP_LIBSUFFIX_CRT "s"
#endif

//#if defined (EMB_DB_STATIC_LIB)
# define EMB_DB_TMP_LIBPREFIX "Lib"
//#else
//# define EMB_DB_TMP_LIBPREFIX ""
//#endif

#define EMB_DB_TMP_LIBNAME \
	EMB_DB_TMP_LIBPREFIX "EmbDB"/* EMB_DB_TMP_CCVER */EMB_DB_TMP_PLATFORM"_" \
	EMB_DB_TMP_LIBSUFFIX_DBG EMB_DB_TMP_LIBSUFFIX_UNC EMB_DB_TMP_LIBSUFFIX_CRT".lib"

#pragma message ("Automatical linking with " EMB_DB_TMP_LIBNAME)

#pragma comment(lib, EMB_DB_TMP_LIBNAME)

#undef EMB_DB_TMP_LIBPREFIX
#undef EMB_DB_TMP_LIBSUFFIX_DBG
#undef EMB_DB_TMP_LIBSUFFIX_UNC
#undef EMB_DB_TMP_LIBSUFFIX_CRT

#undef EMB_DB_TMP_PLATFORM
#undef EMB_DB_TMP_LIBNAME
#else

#endif

