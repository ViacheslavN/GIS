#ifndef ANDROID
#pragma once

#include "png.h"

#if defined(PNG_TMP_LIBPREFIX) || defined(PNG_TMP_LIBSUFFIX_DBG) \
  || defined(PNG_TMP_LIBSUFFIX_UNC) || defined(PNG_TMP_LIBSUFFIX_CRT) \
  ||/* defined(PNG_TMP_CCVER) || */defined(PNG_TMP_PLATFORM) \
  || defined(PNG_TMP_LIBNAME)
# error Required macroses already defined
#endif

#if defined(_WIN32_WCE)
# define PNG_TMP_PLATFORM "wce_"
#elif defined(_WIN32)
# define  PNG_TMP_PLATFORM "32"
#elif defined(WIN64)
# define  PNG_TMP_PLATFORM "64"
#else
# error Unknown platform
#endif
/*
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
# define PNG_TMP_CCVER "80"
#elif defined(_MSC_VER) && (_MSC_VER >= 1310)
# define PNG_TMP_CCVER "71"
#elif defined(_MSC_VER) && (_MSC_VER >= 1300)
# define PNG_TMP_CCVER "70"
#elif defined(_MSC_VER) && (_MSC_VER >= 1200)
# define PNG_TMP_CCVER "60"
#else
# error Unsupported compiler
#endif*/

#if defined(_DEBUG)
# define PNG_TMP_LIBSUFFIX_DBG "d"
#else
# define PNG_TMP_LIBSUFFIX_DBG "r"
#endif

//#if defined(_UNICODE)
//# define PNG_TMP_LIBSUFFIX_UNC "u"
//#else
# define PNG_TMP_LIBSUFFIX_UNC ""
//#endif

#if defined(_DLL)
# define PNG_TMP_LIBSUFFIX_CRT "x"
#else
# define PNG_TMP_LIBSUFFIX_CRT "s"
#endif

//#if defined (PNG_STATIC_LIB)
# define PNG_TMP_LIBPREFIX "Lib"
//#else
//# define PNG_TMP_LIBPREFIX ""
//#endif

#define PNG_TMP_LIBNAME \
  PNG_TMP_LIBPREFIX "Png"/* PNG_TMP_CCVER */PNG_TMP_PLATFORM \
  PNG_TMP_LIBSUFFIX_DBG PNG_TMP_LIBSUFFIX_UNC PNG_TMP_LIBSUFFIX_CRT "-" PNG_LIBPNG_VER_STRING ".lib"

#pragma message ("Automatical linking with " PNG_TMP_LIBNAME)

#pragma comment(lib, PNG_TMP_LIBNAME)

#undef PNG_TMP_LIBPREFIX
#undef PNG_TMP_LIBSUFFIX_DBG
#undef PNG_TMP_LIBSUFFIX_UNC
#undef PNG_TMP_LIBSUFFIX_CRT
//#undef PNG_TMP_CCVER
#undef PNG_TMP_PLATFORM
#undef PNG_TMP_LIBNAME
#else
#include "png.h"
#endif