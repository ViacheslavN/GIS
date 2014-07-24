#ifndef _LIB_COMMON_LIBRARY_H_
#define _LIB_COMMON_LIBRARY_H_


#ifdef _WIN32
#include <windows.h>
#include <process.h> 
#include <tchar.h>
#include <wtypes.h>
#endif //_WIN32
#include "alloc_t.h"

#if _MSC_VER>=1300
#include <xhash>
#else
#define _HASH_SEED	(size_t)0xdeadbeef
#endif

#ifdef ANDROID
#include <wchar.h>
#endif


#include <stdio.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <list>
#include <assert.h>


#include "GeneralTypes.h"
#define  _CHECK_MEM


#endif