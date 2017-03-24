#ifndef _LIB_COMMON_TIME_UTILS_H_
#define _LIB_COMMON_TIME_UTILS_H_

namespace CommonLib
{
	namespace TimeUtils
	{
		uint32 GetCurrentTimeMs();
		uint32 GetCurrentDate(uint32* pnTimeMs = 0);
	}
}

#endif