#ifndef ANDROID
#pragma once
#include "shapefil.h"
#if defined(SHAPELIB_TMP_LIBPREFIX) || defined(SHAPELIB_TMP_LIBSUFFIX_DBG) \
	|| defined(SHAPELIB_TMP_LIBSUFFIX_UNC) || defined(SHAPELIB_TMP_LIBSUFFIX_CRT) \
	||/* defined(SHAPELIB_TMP_CCVER) || */defined(SHAPELIB_TMP_PLATFORM) \
	|| defined(SHAPELIB_TMP_LIBNAME)
# error Required macroses already defined
#endif

#if defined(_WIN32_WCE)
# define SHAPELIB_TMP_PLATFORM "wce_"
#elif defined(WIN64)
# define  SHAPELIB_TMP_PLATFORM "64"
#elif defined(_WIN32)
# define  SHAPELIB_TMP_PLATFORM "32"

#else
# error Unknown platform
#endif
/*
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
# define SHAPELIB_TMP_CCVER "80"
#elif defined(_MSC_VER) && (_MSC_VER >= 1310)
# define SHAPELIB_TMP_CCVER "71"
#elif defined(_MSC_VER) && (_MSC_VER >= 1300)
# define SHAPELIB_TMP_CCVER "70"
#elif defined(_MSC_VER) && (_MSC_VER >= 1200)
# define SHAPELIB_TMP_CCVER "60"
#else
# error Unsupported compiler
#endif*/

#if defined(_DEBUG)
# define SHAPELIB_TMP_LIBSUFFIX_DBG "d"
#else
# define SHAPELIB_TMP_LIBSUFFIX_DBG "r"
#endif

//#if defined(_UNICODE)
//# define SHAPELIB_TMP_LIBSUFFIX_UNC "u"
//#else
# define SHAPELIB_TMP_LIBSUFFIX_UNC ""
//#endif

#if defined(_DLL)
# define SHAPELIB_TMP_LIBSUFFIX_CRT "x"
#else
# define SHAPELIB_TMP_LIBSUFFIX_CRT "s"
#endif

//#if defined (SHAPELIB_STATIC_LIB)
# define SHAPELIB_TMP_LIBPREFIX "ShapeLib"
//#else
//# define SHAPELIB_TMP_LIBPREFIX ""
//#endif

#define SHAPELIB_TMP_LIBNAME \
	SHAPELIB_TMP_LIBPREFIX SHAPELIB_TMP_PLATFORM \
	SHAPELIB_TMP_LIBSUFFIX_DBG SHAPELIB_TMP_LIBSUFFIX_UNC SHAPELIB_TMP_LIBSUFFIX_CRT ".lib"

#pragma message ("Automatical linking with " SHAPELIB_TMP_LIBNAME)

#pragma comment(lib, SHAPELIB_TMP_LIBNAME)

#undef SHAPELIB_TMP_LIBPREFIX
#undef SHAPELIB_TMP_LIBSUFFIX_DBG
#undef SHAPELIB_TMP_LIBSUFFIX_UNC
#undef SHAPELIB_TMP_LIBSUFFIX_CRT
//#undef SHAPELIB_TMP_CCVER
#undef SHAPELIB_TMP_PLATFORM
#undef SHAPELIB_TMP_LIBNAME
#else
#include "shapefil.h"
#endif

