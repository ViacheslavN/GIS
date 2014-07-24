#ifndef _LIB_COMMON_TIME_UTILS_H_
#define _LIB_COMMON_TIME_UTILS_H_
namespace CommonLib
{
	namespace TimeUtils
	{
		long GetCurrentTimeMs();
		long GetCurrentDate(long* pnTimeMs = 0);
	}
}

#endif