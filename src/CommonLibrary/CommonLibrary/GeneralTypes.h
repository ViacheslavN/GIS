#ifndef _LIB_COMMON_LIBRARY_GENERAL_TYPES_H_
#define _LIB_COMMON_LIBRARY_GENERAL_TYPES_H_

#pragma once

#ifdef __IPHONE_3_1
#import <Foundation/Foundation.h>
#endif

#ifdef ANDROID
#include <stdint.h>
#endif

//// CommonLib
//{
  typedef unsigned char byte;
  //typedef char int8;
  //typedef unsigned char uint8;
  typedef short int16;
  typedef unsigned short uint16;
  typedef int int32;
  typedef unsigned int uint32;
  typedef wchar_t   twchar;
	
#ifdef ANDROID
  typedef int64_t int64;
	typedef unsigned long long int uint64;  
#elif defined(__IPHONE_3_1)
	typedef int64_t int64;
  typedef uint64_t uint64;
	typedef unsigned int UINT;
#elif defined(WIN32) || defined(_WIN32_WCE)
  typedef __int64 int64;
  typedef unsigned __int64 uint64;
#endif
//}


#endif //_GIS_COMMON_LIBRARY_PORTABLE_GENERAL_TYPES_H_