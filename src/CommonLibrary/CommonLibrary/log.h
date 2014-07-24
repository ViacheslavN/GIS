#ifndef _LIB_COMMON_LIBRARY_LOG_H_
#define _LIB_COMMON_LIBRARY_LOG_H_

namespace CommonLib
{
	 void setLogLevel(int level);
	 void setLogFileName(const char* pszFileName);
	 void setLogFileNameW(const wchar_t* pszFileName);
	 void devent  (int level, char * format, ...);	
	 void dwevent  (int level, wchar_t * format, ...);	
}

#endif