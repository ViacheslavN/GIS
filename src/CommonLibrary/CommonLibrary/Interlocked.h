#ifndef UGLITE_SYSTEM_INTERLOCKED_H
#define UGLITE_SYSTEM_INTERLOCKED_H
namespace CommonLib
{


struct Interlocked
{
#if defined(WIN32) || defined(_WIN32_WCE)
	typedef LONG inc_type;
#elif defined(__IPHONE_3_1)
	typedef volatile int32_t inc_type;
#elif defined (ANDROID)
	typedef volatile int32_t inc_type;
#else
#error implement your platform
#endif
	static inc_type Increment(inc_type* value);
	static inc_type Decrement(inc_type* value);
	static void     Add(inc_type* addend, inc_type value); 
};

}

#endif