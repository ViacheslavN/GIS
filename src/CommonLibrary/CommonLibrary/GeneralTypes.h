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

  typedef wchar_t   twchar;
	
#ifdef ANDROID
  typedef int8_t int8;
  typedef uint8_t byte;

  typedef int16_t int16;
  typedef uint16_t uint16;

  typedef int32_t int32;
  typedef uint32_t uint32;

  typedef int64_t int64;
  typedef uint64_t uint64;  
#elif defined(__IPHONE_3_1)
	typedef int64_t int64;
  typedef uint64_t uint64;
	typedef unsigned int UINT;
#elif defined(_WIN32) || defined(_WIN32_WCE)

  typedef __int8  int8;
  typedef unsigned __int8 byte;

  typedef __int16 int16;
  typedef unsigned __int16 uint16;

  typedef __int32 int32;
  typedef unsigned __int32 uint32;

  typedef __int64 int64;
  typedef unsigned __int64 uint64;
#endif
//}


#endif //_GIS_COMMON_LIBRARY_PORTABLE_GENERAL_TYPES_H_