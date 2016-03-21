#ifndef ANDROID
#pragma once
#include "src/projects.h"
#include "src/proj_api.h"

#if defined(PROJ4_TMP_LIBPREFIX) || defined(PROJ4_TMP_LIBSUFFIX_DBG) \
	|| defined(PROJ4_TMP_LIBSUFFIX_UNC) || defined(PROJ4_TMP_LIBSUFFIX_CRT) \
	||/* defined(PROJ4_TMP_CCVER) || */defined(PROJ4_TMP_PLATFORM) \
	|| defined(PROJ4_TMP_LIBNAME)
# error Required macroses already defined
#endif

#if defined(_WIN32_WCE)
# define PROJ4_TMP_PLATFORM "wce_"
#elif defined(WIN64)
# define  PROJ4_TMP_PLATFORM "64"
#elif defined(_WIN32)
# define  PROJ4_TMP_PLATFORM "32"
#else
# error Unknown platform
#endif
/*
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
# define PROJ4_TMP_CCVER "80"
#elif defined(_MSC_VER) && (_MSC_VER >= 1310)
# define PROJ4_TMP_CCVER "71"
#elif defined(_MSC_VER) && (_MSC_VER >= 1300)
# define PROJ4_TMP_CCVER "70"
#elif defined(_MSC_VER) && (_MSC_VER >= 1200)
# define PROJ4_TMP_CCVER "60"
#else
# error Unsupported compiler
#endif*/

#if defined(_DEBUG)
# define PROJ4_TMP_LIBSUFFIX_DBG "d"
#else
# define PROJ4_TMP_LIBSUFFIX_DBG "r"
#endif

//#if defined(_UNICODE)
//# define PROJ4_TMP_LIBSUFFIX_UNC "u"
//#else
# define PROJ4_TMP_LIBSUFFIX_UNC ""
//#endif

#if defined(_DLL)
# define PROJ4_TMP_LIBSUFFIX_CRT "x"
#else
# define PROJ4_TMP_LIBSUFFIX_CRT "s"
#endif

//#if defined (PROJ4_STATIC_LIB)
# define PROJ4_TMP_LIBPREFIX "Lib"
//#else
//# define PROJ4_TMP_LIBPREFIX ""
//#endif

#define PROJ4_TMP_LIBNAME \
	PROJ4_TMP_LIBPREFIX "Proj4"/* PROJ4_TMP_CCVER */PROJ4_TMP_PLATFORM \
	PROJ4_TMP_LIBSUFFIX_DBG PROJ4_TMP_LIBSUFFIX_UNC PROJ4_TMP_LIBSUFFIX_CRT "-4.9.1"  ".lib"

#pragma message ("Automatical linking with " PROJ4_TMP_LIBNAME)

#pragma comment(lib, PROJ4_TMP_LIBNAME)

#undef PROJ4_TMP_LIBPREFIX
#undef PROJ4_TMP_LIBSUFFIX_DBG
#undef PROJ4_TMP_LIBSUFFIX_UNC
#undef PROJ4_TMP_LIBSUFFIX_CRT
//#undef PROJ4_TMP_CCVER
#undef PROJ4_TMP_PLATFORM
#undef PROJ4_TMP_LIBNAME
#else
#include "src/projects.h"
#include "src/proj_api.h"
#endif