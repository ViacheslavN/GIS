#include "stdafx.h"
#include "DBTranWalManager.h"
#include "transactions/WAL/WALTransaction.h"
#include "storage/FilePage.h"
#include "storage/storage.h"
#include "CommonLibrary/TimeUtils.h"
 
namespace embDB
{


	CDBWALTranManager::CDBWALTranManager(CommonLib::alloc_t *pAlloc, CDatabase *pDB, ILogger *pLogger) :
		m_pAlloc(pAlloc)
		, m_pDB(pDB)
		, m_pPageCliper(nullptr)
		, m_pLogger(pLogger)
	{

	}
	CDBWALTranManager::~CDBWALTranManager()
	{

	}
	
	void CDBWALTranManager::SetPageCipher(CPageCipher* pPageCliper)
	{
		m_pPageCliper = pPageCliper;
	}

 
	bool  CDBWALTranManager::close()
	{
		TDBTransactions::iterator it = m_Transactions.begin();
		for (; it != m_Transactions.end(); ++it)
		{
			IDBTransaction* pTran = (IDBTransaction*)(it->get());
			if (!pTran->isCompleted())
			{//TODO login and create  observer
				pTran->stop();
				pTran->wait();
			}
			//delete pTran;
		}
		m_pLogger.release();
		m_pPageCliper = nullptr;
		return true;
	}

	ITransactionPtr CDBWALTranManager::CreateTransaction(eTransactionDataType trType, IDBConnection *pConn, eDBTransationType trDbType)
	{
		CommonLib::CString sFileName;
		uint32 nDate = 0;
		uint32 nTime = 0;
		nDate = CommonLib::TimeUtils::GetCurrentDate(&nTime);
 
		IDBTransaction *pTran = new CWALTransaction(m_pAlloc, rtRedo, trType,  pConn, 10000, m_pPageCliper);
		 

		if (!pTran)
		{
			//TO DO log
			return  ITransactionPtr();
		}

		pTran->SetLogger(m_pLogger.get());
	 	ITransactionPtr pTranPtr(pTran);
		m_Transactions.insert(pTranPtr);
		return pTranPtr;
	}
	bool CDBWALTranManager::releaseTransaction(ITransaction* pTran)
	{
		TDBTransactions::iterator it = m_Transactions.find(ITransactionPtr(pTran));
		if (it == m_Transactions.end())
			return false; // TO DO Log
		m_Transactions.erase(it);
		return true;
	}
}