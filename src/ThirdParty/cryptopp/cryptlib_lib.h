#ifndef ANDROID
#pragma once
 

#if defined(CRYPTOPP_TMP_LIBPREFIX) || defined(CRYPTOPP_TMP_LIBSUFFIX_DBG) \
	|| defined(CRYPTOPP_TMP_LIBSUFFIX_UNC) || defined(CRYPTOPP_TMP_LIBSUFFIX_CRT) \
	||/* defined(CRYPTOPP_TMP_CCVER) || */defined(CRYPTOPP_TMP_PLATFORM) \
	|| defined(CRYPTOPP_TMP_LIBNAME)
# error Required macroses already defined
#endif

#if defined(_WIN32_WCE)
# define CRYPTOPP_TMP_PLATFORM "wce_"
#elif defined(WIN64)
# define  CRYPTOPP_TMP_PLATFORM "64"
#elif defined(_WIN32)
# define  CRYPTOPP_TMP_PLATFORM "32"
#else
# error Unknown platform
#endif
/*
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
# define CRYPTOPP_TMP_CCVER "80"
#elif defined(_MSC_VER) && (_MSC_VER >= 1310)
# define CRYPTOPP_TMP_CCVER "71"
#elif defined(_MSC_VER) && (_MSC_VER >= 1300)
# define CRYPTOPP_TMP_CCVER "70"
#elif defined(_MSC_VER) && (_MSC_VER >= 1200)
# define CRYPTOPP_TMP_CCVER "60"
#else
# error Unsupported compiler
#endif*/

#if defined(_DEBUG)
# define CRYPTOPP_TMP_LIBSUFFIX_DBG "d"
#else
# define CRYPTOPP_TMP_LIBSUFFIX_DBG "r"
#endif

//#if defined(_UNICODE)
//# define CRYPTOPP_TMP_LIBSUFFIX_UNC "u"
//#else
# define CRYPTOPP_TMP_LIBSUFFIX_UNC ""
//#endif

#if defined(_DLL)
# define CRYPTOPP_TMP_LIBSUFFIX_CRT "x"
#else
# define CRYPTOPP_TMP_LIBSUFFIX_CRT "s"
#endif

//#if defined (CRYPTOPP_STATIC_LIB)
# define CRYPTOPP_TMP_LIBPREFIX "Lib"
//#else
//# define CRYPTOPP_TMP_LIBPREFIX ""
//#endif

#define CRYPTOPP_TMP_LIBNAME \
	CRYPTOPP_TMP_LIBPREFIX "Cryptopp"/* CRYPTOPP_TMP_CCVER */CRYPTOPP_TMP_PLATFORM \
	CRYPTOPP_TMP_LIBSUFFIX_DBG CRYPTOPP_TMP_LIBSUFFIX_UNC CRYPTOPP_TMP_LIBSUFFIX_CRT "-5.6.3"  ".lib"

#pragma message ("Automatical linking with " CRYPTOPP_TMP_LIBNAME)

#pragma comment(lib, CRYPTOPP_TMP_LIBNAME)

#undef CRYPTOPP_TMP_LIBPREFIX
#undef CRYPTOPP_TMP_LIBSUFFIX_DBG
#undef CRYPTOPP_TMP_LIBSUFFIX_UNC
#undef CRYPTOPP_TMP_LIBSUFFIX_CRT
//#undef CRYPTOPP_TMP_CCVER
#undef CRYPTOPP_TMP_PLATFORM
#undef CRYPTOPP_TMP_LIBNAME
#else
 
#endif