#ifndef _LIB_GENERAL_COMMON_LIBRARY_H_
#define _LIB_GENERAL_COMMON_LIBRARY_H_


#ifdef _WIN32
	#include <windows.h>
	#include <process.h> 
	#include <tchar.h>
	#include <wtypes.h>
#endif //_WIN32
#include "alloc_t.h"

#if _MSC_VER>=1300
//#include <xhash>
#else
#define _HASH_SEED	(size_t)0xdeadbeef
#endif

#ifdef ANDROID
	#include <wchar.h>
	#include <sys/types.h>
	#include <sys/stat.h>
    #include <fcntl.h>
#ifndef min
	#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
	#define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif


#endif


#include <stdio.h>
#include <string>
#include <vector>
#include <list>
#include <assert.h>
#include <map>
#include <memory>
#include <algorithm>
#include <limits>
#include <functional>
#include <iterator>
#include <iostream>
#include <stack>
#include "GeneralTypes.h"
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include<shared_mutex>

#define  _CHECK_MEM


#ifdef ANDROID
#ifndef min
	#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
	#define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif


#endif
#endif
