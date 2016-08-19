#include "stdafx.h"
#include "Database.h"


#include "UserManager.h"

namespace embDB
{
	CUserManager::CUserManager(CDatabase *pDB) : m_pDB(pDB)
	{

	}
	CUserManager::~CUserManager()
	{

	}

	bool CUserManager::init(int64 nAddr, eUserMode mode)
	{
		return true;
	}
	bool CUserManager::load(int64 nAddr)
	{
		return true;
	}

	eUserMode CUserManager::getMode() const
	{
		return m_UserMode;
	}


	bool CUserManager::CheckUser(const wchar_t *pszUser, const wchar_t *pszPassword)
	{
		return true;
	}
	bool CUserManager::GetKey(const wchar_t *pszUser, const wchar_t *pszPassword, CommonLib::CBlob& blob)
	{
		return true;
	}
}
