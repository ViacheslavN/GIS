#include "stdafx.h"
#include "ConsolLog.h"
#include <iostream>

namespace embDB
{
	CConsolLogger::CConsolLogger() : m_nLogLevel(0)
	{

	}
	CConsolLogger::~CConsolLogger()
	{

	}

	uint32 CConsolLogger::GetLogLevel() const
	{
		return m_nLogLevel;
	}
	void CConsolLogger::SetLogLevel(uint32 nLogLevel)
	{
		m_nLogLevel = nLogLevel;
	}

	void CConsolLogger::error(const wchar_t *pszFormat, ...)
	{
		va_list args;
		va_start(args, pszFormat);
		wprintf_s(pszFormat, args);
		va_end(args);
	}
	void CConsolLogger::log(uint32 nLevel, const wchar_t *pszFormat, ...)
	{
		if(nLevel > m_nLogLevel)
			return;


		va_list args;
		va_start(args, pszFormat);
		wprintf_s(pszFormat, args);
		va_end(args);
	}
	void CConsolLogger::log_msg(uint32 nLevel, const wchar_t *pszMsg)
	{
		if(nLevel > m_nLogLevel)
			return;

		std::wcout << pszMsg << std::endl;
	}
}