#ifndef _FREETYPE_LIB_LIB_H__
#define _FREETYPE_LIB_LIB_H__
#include <ft2build.h>

#ifndef ANDROID

#if defined(FT_TMP_LIBPREFIX) || defined(FT_TMP_LIBSUFFIX_DBG) \
	|| defined(FT_TMP_LIBSUFFIX_UNC) || defined(FT_TMP_LIBSUFFIX_CRT) \
	||defined(FT_TMP_PLATFORM) \
	|| defined(FT_TMP_LIBNAME)
# error Required macroses already defined
#endif
#define FT_LIB_VER_STRING "2.4.5"


#if defined(_WIN32_WCE)
# define FT_TMP_PLATFORM "wce_"
#elif defined(WIN64)
# define  FT_TMP_PLATFORM "64"
#elif defined(_WIN32)
# define  FT_TMP_PLATFORM "32"
#else
# error Unknown platform
#endif


#if defined(_DEBUG)
# define FT_TMP_LIBSUFFIX_DBG "d"
#else
# define FT_TMP_LIBSUFFIX_DBG "r"
#endif

//#if defined(_UNICODE)
//# define FT_TMP_LIBSUFFIX_UNC "u"
//#else
# define FT_TMP_LIBSUFFIX_UNC ""
//#endif

#if defined(_DLL)
# define FT_TMP_LIBSUFFIX_CRT "x"
#else
# define FT_TMP_LIBSUFFIX_CRT "s"
#endif

//#if defined (FT_STATIC_LIB)
# define FT_TMP_LIBPREFIX "Lib"
//#else
//# define FT_TMP_LIBPREFIX ""
//#endif

#define FT_TMP_LIBNAME \
	FT_TMP_LIBPREFIX "FreeType"/* FT_TMP_CCVER */FT_TMP_PLATFORM \
	FT_TMP_LIBSUFFIX_DBG FT_TMP_LIBSUFFIX_UNC FT_TMP_LIBSUFFIX_CRT "-" FT_LIB_VER_STRING ".lib"

#pragma message ("Automatical linking with " FT_TMP_LIBNAME)

#pragma comment(lib, FT_TMP_LIBNAME)

#undef FT_TMP_LIBPREFIX
#undef FT_TMP_LIBSUFFIX_DBG
#undef FT_TMP_LIBSUFFIX_UNC
#undef FT_TMP_LIBSUFFIX_CRT
//#undef FT_TMP_CCVER
#undef FT_TMP_PLATFORM
#undef FT_TMP_LIBNAME
#else
#include <ft2build.h>
#endif

#endif