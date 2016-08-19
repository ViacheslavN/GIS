#include "stdafx.h"
#include "Connection.h"
#include "Database.h"

namespace embDB
{
	CConnection::CConnection(CDatabase *pDB, const wchar_t *pszUserName, uint64 nUID) :
		m_pDB(pDB), m_nUID(nUID)
	{
		m_sUserName = pszUserName;
	}
	CConnection::~CConnection()
	{

	}

	ITransactionPtr CConnection::startTransaction(eTransactionType trType)
	{

		//TO Do check right
		return m_pDB->startTransaction(trType, m_nUID);
	}
	bool CConnection::closeTransaction(ITransaction* pTran)
	{
		return m_pDB->closeTransaction(pTran, m_nUID);
	}
	uint64 CConnection::getUserUID() const
	{
		return m_nUID;
	}
	const wchar_t *CConnection::getUserName() const
	{
		return m_sUserName.cwstr();
	}
}
