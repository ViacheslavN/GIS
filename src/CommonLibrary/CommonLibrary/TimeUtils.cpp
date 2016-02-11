#include "stdafx.h"
#include "TimeUtils.h"
#include "general.h"
#ifdef ANDROID
	#include <time.h>  
#endif


namespace CommonLib
{
	namespace TimeUtils
	{

		long GetCurrentTimeMs()
		{
			long nRet = 0;
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
			 nRet = tm.tm_hour;
			 nRet *= 100; nRet += tm.tm_min;
			 nRet *= 100; nRet += tm.tm_sec;
			 nRet *= 1000; /*nRet += st.wMilliseconds;*/

#endif
			return nRet;
		}
		long GetCurrentDate(long* pnTimeMs)
		{
			long nRet = 0;
#ifdef WIN32
			SYSTEMTIME st = {0};
			::GetLocalTime(&st);

			nRet = st.wYear;
			nRet *= 100; nRet += st.wMonth;
			nRet *= 100; nRet += st.wDay;

			if (pnTimeMs != NULL)
			{
				long& nTimeMs = *pnTimeMs;
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
			nRet = tm.tm_year;
			nRet *= 100; nRet +=tm.tm_mon;
			nRet *= 100; nRet += tm.tm_mday;
			if (pnTimeMs != NULL)
			{
				long& nTimeMs = *pnTimeMs;
				nTimeMs = tm.tm_hour;
				nTimeMs *= 100; nTimeMs += tm.tm_min;
				nTimeMs *= 100; nTimeMs += tm.tm_sec;
				nTimeMs *= 1000;  /*nRet += st.wMilliseconds;*/
			}
#endif
			return nRet;
		}

	}
}