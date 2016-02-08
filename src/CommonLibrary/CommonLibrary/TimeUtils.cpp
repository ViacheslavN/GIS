#include "stdafx.h"
#include "TimeUtils.h"
#include "general.h"

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

			long nRet = st.wHour;
			nRet *= 100; nRet += st.wMinute;
			nRet *= 100; nRet += st.wSecond;
			nRet *= 1000; nRet += st.wMilliseconds;
#endif
			return nRet;
		}
		long GetCurrentDate(long* pnTimeMs)
		{
			long nRet = 0;
#ifdef WIN32
			SYSTEMTIME st = {0};
			::GetLocalTime(&st);

			long nRet = st.wYear;
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
#endif
			return nRet;
		}

	}
}