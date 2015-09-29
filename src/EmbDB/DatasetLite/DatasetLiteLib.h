#ifndef _DATASET_LITE_LIB_H__
#define _DATASET_LITE_LIB_H__


#if defined(DATASET_LITE_TMP_LIBPREFIX) || defined(DATASET_LITE_TMP_LIBSUFFIX_DBG) \
	|| defined(DATASET_LITE_TMP_LIBSUFFIX_UNC) || defined(DATASET_LITE_TMP_LIBSUFFIX_CRT) \
	||defined(DATASET_LITE_TMP_PLATFORM) \
	|| defined(DATASET_LITE_TMP_LIBNAME)
# error Required macroses already defined
#endif
#define DATASET_LITE_LIB_VER_STRING ""


#if defined(_WIN32_WCE)
# define DATASET_LITE_TMP_PLATFORM "wce_"
#elif defined(_WIN32)
# define  DATASET_LITE_TMP_PLATFORM "32"
#elif defined(WIN64)
# define  DATASET_LITE_TMP_PLATFORM "64"
#else
# error Unknown platform
#endif


#if defined(_DEBUG)
# define DATASET_LITE_TMP_LIBSUFFIX_DBG "d"
#else
# define DATASET_LITE_TMP_LIBSUFFIX_DBG "r"
#endif

//#if defined(_UNICODE)
//# define DATASET_LITE_TMP_LIBSUFFIX_UNC "u"
//#else
# define DATASET_LITE_TMP_LIBSUFFIX_UNC ""
//#endif

#if defined(_DLL)
# define DATASET_LITE_TMP_LIBSUFFIX_CRT "x"
#else
# define DATASET_LITE_TMP_LIBSUFFIX_CRT "s"
#endif

//#if defined (DATASET_LITE_STATIC_LIB)
# define DATASET_LITE_TMP_LIBPREFIX "Lib"
//#else
//# define DATASET_LITE_TMP_LIBPREFIX ""
//#endif

#define DATASET_LITE_TMP_LIBNAME \
	DATASET_LITE_TMP_LIBPREFIX "DatasetLite"/* DATASET_LITE_TMP_CCVER */DATASET_LITE_TMP_PLATFORM"_" \
	DATASET_LITE_TMP_LIBSUFFIX_DBG DATASET_LITE_TMP_LIBSUFFIX_UNC DATASET_LITE_TMP_LIBSUFFIX_CRT".lib"

#pragma message ("Automatical linking with " DATASET_LITE_TMP_LIBNAME)

#pragma comment(lib, DATASET_LITE_TMP_LIBNAME)

#undef DATASET_LITE_TMP_LIBPREFIX
#undef DATASET_LITE_TMP_LIBSUFFIX_DBG
#undef DATASET_LITE_TMP_LIBSUFFIX_UNC
#undef DATASET_LITE_TMP_LIBSUFFIX_CRT

#undef DATASET_LITE_TMP_PLATFORM
#undef DATASET_LITE_TMP_LIBNAME
#else

#endif

