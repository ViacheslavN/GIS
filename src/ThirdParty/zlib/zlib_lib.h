#pragma once

#include "zlib.h"

#if defined(ZLIB_TMP_LIBPREFIX) || defined(ZLIB_TMP_LIBSUFFIX_DBG) \
  || defined(ZLIB_TMP_LIBSUFFIX_UNC) || defined(ZLIB_TMP_LIBSUFFIX_CRT) \
  || defined(ZLIB_TMP_PLATFORM) \
  || defined(ZLIB_TMP_LIBNAME)
# error Required macroses already defined
#endif

#if defined(_WIN32_WCE)
# define ZLIB_TMP_PLATFORM "wce_"
#elif defined(WIN64)
# define  ZLIB_TMP_PLATFORM "64"
#elif defined(WIN32)
# define  ZLIB_TMP_PLATFORM "32"
#else
# error Unknown platform
#endif
/*
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
# define ZLIB_TMP_CCVER "80"
#elif defined(_MSC_VER) && (_MSC_VER >= 1310)
# define ZLIB_TMP_CCVER "71"
#elif defined(_MSC_VER) && (_MSC_VER >= 1300)
# define ZLIB_TMP_CCVER "70"
#elif defined(_MSC_VER) && (_MSC_VER >= 1200)
# define ZLIB_TMP_CCVER "60"
#else
# error Unsupported compiler
#endif*/

#if defined(_DEBUG)
# define ZLIB_TMP_LIBSUFFIX_DBG "d"
#else
# define ZLIB_TMP_LIBSUFFIX_DBG "r"
#endif

//#if defined(_UNICODE)
//# define ZLIB_TMP_LIBSUFFIX_UNC "u"
//#else
# define ZLIB_TMP_LIBSUFFIX_UNC ""
//#endif

#if defined(_DLL)
# define ZLIB_TMP_LIBSUFFIX_CRT "x"
#else
# define ZLIB_TMP_LIBSUFFIX_CRT "s"
#endif

//#if defined (ZLIB_STATIC_LIB)
# define ZLIB_TMP_LIBPREFIX "Lib"
//#else
//# define ZLIB_TMP_LIBPREFIX ""
//#endif

#define ZLIB_TMP_LIBNAME \
  ZLIB_TMP_LIBPREFIX "ZLib"  ZLIB_TMP_PLATFORM \
  ZLIB_TMP_LIBSUFFIX_DBG ZLIB_TMP_LIBSUFFIX_UNC ZLIB_TMP_LIBSUFFIX_CRT "-" ZLIB_VERSION ".lib"

#pragma message ("Automatical linking with " ZLIB_TMP_LIBNAME)

#pragma comment(lib, ZLIB_TMP_LIBNAME)

#undef ZLIB_TMP_LIBPREFIX
#undef ZLIB_TMP_LIBSUFFIX_DBG
#undef ZLIB_TMP_LIBSUFFIX_UNC
#undef ZLIB_TMP_LIBSUFFIX_CRT
//#undef ZLIB_TMP_CCVER
#undef ZLIB_TMP_PLATFORM
#undef ZLIB_TMP_LIBNAME
