#include "stdafx.h"
#include "ConsolLog.h"
#include <iostream>

namespace embDB
{
	CConsolLogger::CConsolLogger(bool bMultiTread ) : m_nLogLevel(0)
	{

		if(bMultiTread)
			m_pLockObj = &m_CSLock;
		else
			m_pLockObj = &m_emptyLock;

	}
	CConsolLogger::~CConsolLogger()
	{

	}

	uint32 CConsolLogger::GetLogLevel() const
	{
		CommonLib::ILockObject::scoped_lock lock(m_pLockObj);

		return m_nLogLevel;
	}
	void CConsolLogger::SetLogLevel(uint32 nLogLevel)
	{
		CommonLib::ILockObject::scoped_lock lock(m_pLockObj);
		m_nLogLevel = nLogLevel;
	}

	void CConsolLogger::error(const wchar_t *pszFormat, ...)
	{
		CommonLib::ILockObject::scoped_lock lock(m_pLockObj);

		va_list args;
		va_start(args, pszFormat);
		wprintf_s(pszFormat, args);
		va_end(args);
	}
	void CConsolLogger::log(uint32 nLevel, const wchar_t *pszFormat, ...)
	{
		CommonLib::ILockObject::scoped_lock lock(m_pLockObj);
		if(nLevel > m_nLogLevel)
			return;


		va_list args;
		va_start(args, pszFormat);
		wprintf_s(pszFormat, args);
		va_end(args);
	}
	void CConsolLogger::log_msg(uint32 nLevel, const wchar_t *pszMsg)
	{
		CommonLib::ILockObject::scoped_lock lock(m_pLockObj);
		if(nLevel > m_nLogLevel)
			return;

		std::wcout << pszMsg << std::endl;
	}
}