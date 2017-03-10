#pragma once


#if defined(DBFILE_VIEWER_TMP_LIBPREFIX) || defined(DBFILE_VIEWER_TMP_LIBSUFFIX_DBG) \
	|| defined(DBFILE_VIEWER_TMP_LIBSUFFIX_UNC) || defined(DBFILE_VIEWER_TMP_LIBSUFFIX_CRT) \
	||defined(DBFILE_VIEWER_TMP_PLATFORM) \
	|| defined(DBFILE_VIEWER_TMP_LIBNAME)
# error Required macroses already defined
#endif
#define DBFILE_VIEWER_LIB_VER_STRING ""


#if defined(_WIN32_WCE)
# define DBFILE_VIEWER_TMP_PLATFORM "wce_"
#elif defined(WIN64)
# define  DBFILE_VIEWER_TMP_PLATFORM "64"
#elif defined(_WIN32)
# define  DBFILE_VIEWER_TMP_PLATFORM "32"
#else
# error Unknown platform
#endif


#if defined(_DEBUG)
# define DBFILE_VIEWER_TMP_LIBSUFFIX_DBG "d"
#else
# define DBFILE_VIEWER_TMP_LIBSUFFIX_DBG "r"
#endif

//#if defined(_UNICODE)
//# define DBFILE_VIEWER_TMP_LIBSUFFIX_UNC "u"
//#else
# define DBFILE_VIEWER_TMP_LIBSUFFIX_UNC ""
//#endif

#if defined(_DLL)
# define DBFILE_VIEWER_TMP_LIBSUFFIX_CRT "x"
#else
# define DBFILE_VIEWER_TMP_LIBSUFFIX_CRT "s"
#endif

//#if defined (DBFILE_VIEWER_STATIC_LIB)
# define DBFILE_VIEWER_TMP_LIBPREFIX "Lib"
//#else
//# define DBFILE_VIEWER_TMP_LIBPREFIX ""
//#endif

#define DBFILE_VIEWER_TMP_LIBNAME \
	DBFILE_VIEWER_TMP_LIBPREFIX "DBFileViewerLib"/* DBFILE_VIEWER_TMP_CCVER */DBFILE_VIEWER_TMP_PLATFORM"_" \
	DBFILE_VIEWER_TMP_LIBSUFFIX_DBG DBFILE_VIEWER_TMP_LIBSUFFIX_UNC DBFILE_VIEWER_TMP_LIBSUFFIX_CRT".lib"

#pragma message ("Automatical linking with " DBFILE_VIEWER_TMP_LIBNAME)

#pragma comment(lib, DBFILE_VIEWER_TMP_LIBNAME)

#undef DBFILE_VIEWER_TMP_LIBPREFIX
#undef DBFILE_VIEWER_TMP_LIBSUFFIX_DBG
#undef DBFILE_VIEWER_TMP_LIBSUFFIX_UNC
#undef DBFILE_VIEWER_TMP_LIBSUFFIX_CRT

#undef DBFILE_VIEWER_TMP_PLATFORM
#undef DBFILE_VIEWER_TMP_LIBNAME
 

 

