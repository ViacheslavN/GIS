#include "stdafx.h"
#include "general.h"
#include "Interlocked.h"
#ifdef __IPHONE_3_1
#include <libkern/OSAtomic.h>
#endif

namespace CommonLib
{


Interlocked::inc_type Interlocked::Increment(inc_type* value)
{
#if defined(WIN32) || defined(_WIN32_WCE)
  return ::InterlockedIncrement(value);
#elif defined(__IPHONE_3_1)
	OSAtomicIncrement32(value); // can return incorrect value
	return *value;
#elif defined (ANDROID)
   return	__sync_fetch_and_add(value, 1);
#else
#error implement your platform
#endif
}

Interlocked::inc_type Interlocked::Decrement(inc_type* value)
{
#if defined(WIN32) || defined(_WIN32_WCE)
  return ::InterlockedDecrement(value);
#elif defined(__IPHONE_3_1)
	OSAtomicDecrement32(value); // can return incorrect value
	return *value;
#elif defined (ANDROID)
  return __sync_fetch_and_add(value, -1);
#else
#error implement your platform
#endif
}

void Interlocked::Add(inc_type* addend, inc_type value)
{
#if defined(WIN32) || defined(_WIN32_WCE)
  ::InterlockedExchangeAdd(addend, value);
#elif ANDROID
  __sync_fetch_and_add(addend, value);
#elif defined(__IPHONE_3_1)
	*addend = OSAtomicAdd32(value, addend);
#else
#error implement your platform
#endif

}

}
