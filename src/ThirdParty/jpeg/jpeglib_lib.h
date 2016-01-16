#ifndef ANDROID
#pragma once

#include "jpeglib.h"

#if defined(JPG_TMP_LIBPREFIX) || defined(JPG_TMP_LIBSUFFIX_DBG) \
  || defined(JPG_TMP_LIBSUFFIX_UNC) || defined(JPG_TMP_LIBSUFFIX_CRT) \
  || defined(JPG_TMP_PLATFORM) \
  || defined(JPG_TMP_LIBNAME)
# error Required macroses already defined
#endif

#if defined(_WIN32_WCE)
# define JPG_TMP_PLATFORM "wce_"
#elif defined(WIN64)
# define  JPG_TMP_PLATFORM "64"
#elif defined(WIN32)
# define  JPG_TMP_PLATFORM "32"
#else
# error Unknown platform
#endif
/*
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
# define JPG_TMP_CCVER "80"
#elif defined(_MSC_VER) && (_MSC_VER >= 1310)
# define JPG_TMP_CCVER "71"
#elif defined(_MSC_VER) && (_MSC_VER >= 1300)
# define JPG_TMP_CCVER "70"
#elif defined(_MSC_VER) && (_MSC_VER >= 1200)
# define JPG_TMP_CCVER "60"
#else
# error Unsupported compiler
#endif*/

#if defined(_DEBUG)
# define JPG_TMP_LIBSUFFIX_DBG "d"
#else
# define JPG_TMP_LIBSUFFIX_DBG "r"
#endif

//#if defined(_UNICODE)
//# define JPG_TMP_LIBSUFFIX_UNC "u"
//#else
# define JPG_TMP_LIBSUFFIX_UNC ""
//#endif

#if defined(_DLL)
# define JPG_TMP_LIBSUFFIX_CRT "x"
#else
# define JPG_TMP_LIBSUFFIX_CRT "s"
#endif

//#if defined (JPG_STATIC_LIB)
# define JPG_TMP_LIBPREFIX "Lib"
//#else
//# define JPG_TMP_LIBPREFIX ""
//#endif

#define JPG_TMP_LIBNAME \
  JPG_TMP_LIBPREFIX "JPG" JPG_TMP_PLATFORM \
  JPG_TMP_LIBSUFFIX_DBG JPG_TMP_LIBSUFFIX_UNC JPG_TMP_LIBSUFFIX_CRT "-9a"  ".lib"

	#pragma message ("Automatical linking with " JPG_TMP_LIBNAME)

	#pragma comment(lib, JPG_TMP_LIBNAME)

	#undef JPG_TMP_LIBPREFIX
	#undef JPG_TMP_LIBSUFFIX_DBG
	#undef JPG_TMP_LIBSUFFIX_UNC
	#undef JPG_TMP_LIBSUFFIX_CRT
	//#undef JPG_TMP_CCVER
	#undef JPG_TMP_PLATFORM
	#undef JPG_TMP_LIBNAME
#else
#include "jpeglib.h"
#endif