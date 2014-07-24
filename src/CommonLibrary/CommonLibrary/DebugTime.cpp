#include "stdafx.h"
#include "DebugTime.h"

namespace CommonLib
{
	namespace TimeUtils
	{
		CDebugTime::CDebugTime() : m_nStartCount(0)
		{}
		CDebugTime::~CDebugTime(){}

		void CDebugTime::start()
		{
#ifdef WIN32	
			::QueryPerformanceCounter((LARGE_INTEGER*)&m_nStartCount);
			
#endif
		}
		double CDebugTime::stop()
		{
#ifdef WIN32	
			int64 counter;
			int64 freq;
			::QueryPerformanceCounter((LARGE_INTEGER*)&counter);
			::QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
			return   double(counter - m_nStartCount)/freq;
#endif
		}
	}
}
