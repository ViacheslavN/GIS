#include "stdafx.h"
#include "Connection.h"
#include "Database.h"

namespace embDB
{
	CConnection::CConnection(CDatabase* pDB,  IDBShema* pSchema, const wchar_t *pszUserName, uint64 nUID, eUserGroup userGroup) :
		 m_nUID(nUID), m_UserGroup(userGroup), m_pDB(pDB)
	{
		m_sUserName = pszUserName;
		m_pSchema = pSchema;
	}
	CConnection::~CConnection()
	{

	}
	IDBStoragePtr CConnection::getDBStorage() const
	{
		return m_pDB->getDBStorage();
	}
	bool CConnection::getCheckCRC()const 
	{
		return m_pDB->getCheckCRC();
	}
	ITransactionPtr CConnection::startTransaction(eTransactionDataType trType, eDBTransationType trDbType)
	{

		//TO Do check right
		return m_pDB->startTransaction(trType, m_nUID, this, trDbType);
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
	eUserGroup CConnection::getUserGroup() const
	{
		return m_UserGroup;
	}
}
