#include "stdafx.h"
#include "general.h"
#include "TimeUtils.h"

#ifdef ANDROID
	#include <time.h>  
#endif


namespace CommonLib
{
	namespace TimeUtils
	{

		uint32 GetCurrentTimeMs()
		{
			uint32 nRet = 0;
#ifdef WIN32
			SYSTEMTIME st = {0};
			::GetLocalTime(&st);

			nRet = st.wHour;
			nRet *= 100; nRet += st.wMinute;
			nRet *= 100; nRet += st.wSecond;
			nRet *= 1000; nRet += st.wMilliseconds;


#elif ANDROID
			time_t rawtime;
			struct tm  timeinfo;
			time (&rawtime);
			timeinfo = *localtime (&rawtime);
			 nRet = timeinfo.tm_hour;
			 nRet *= 100; nRet += timeinfo.tm_min;
			 nRet *= 100; nRet += timeinfo.tm_sec;
			 nRet *= 1000; /*nRet += st.wMilliseconds;*/

#endif
			return nRet;
		}
		uint32 GetCurrentDate(uint32* pnTimeMs)
		{
			uint32 nRet = 0;
#ifdef WIN32
			SYSTEMTIME st = {0};
			::GetLocalTime(&st);

			nRet = st.wYear;
			nRet *= 100; nRet += st.wMonth;
			nRet *= 100; nRet += st.wDay;

			if (pnTimeMs != NULL)
			{
				uint32& nTimeMs = *pnTimeMs;
				nTimeMs = st.wHour;
				nTimeMs *= 100; nTimeMs += st.wMinute;
				nTimeMs *= 100; nTimeMs += st.wSecond;
				nTimeMs *= 1000; nTimeMs += st.wMilliseconds;
			}
#elif ANDROID
			time_t rawtime;
			struct tm  timeinfo;
			time (&rawtime);
			timeinfo = *localtime (&rawtime);
			nRet = timeinfo.tm_year;
			nRet *= 100; nRet +=timeinfo.tm_mon;
			nRet *= 100; nRet += timeinfo.tm_mday;
			if (pnTimeMs != NULL)
			{
				uint32& nTimeMs = *pnTimeMs;
				nTimeMs = timeinfo.tm_hour;
				nTimeMs *= 100; nTimeMs += timeinfo.tm_min;
				nTimeMs *= 100; nTimeMs += timeinfo.tm_sec;
				nTimeMs *= 1000;  /*nRet += st.wMilliseconds;*/
			}
#endif
			return nRet;
		}

	}
}