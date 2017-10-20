#pragma once
 

#include "CommonLibrary/String.h"
#include "../embDBInternal.h"
#include "../storage/storage.h"
#include "../storage/FilePage.h"
#include "Database.h"
namespace embDB
{
 
	class CDBWALTranManager : public IDBTranManager
	{
	public:
 
		CDBWALTranManager(CommonLib::alloc_t *pAlloc, CDatabase *pDB, ILogger *pLogger);
		~CDBWALTranManager();
		bool open(const CommonLib::CString &sFileName, const CommonLib::CString& sWorkingPath, ILogger *pLogger);
		virtual bool close();
		virtual ITransactionPtr CreateTransaction(eTransactionDataType trType, IDBConnection *pConn, eDBTransationType trDbType);
		virtual bool releaseTransaction(ITransaction* pTran);
		void SetPageCipher(CPageCipher* pPageCliper);
	private:
	 
		CommonLib::alloc_t *m_pAlloc;
		typedef std::set<ITransactionPtr> TDBTransactions;
		TDBTransactions m_Transactions;
		CDatabase* m_pDB;
		CommonLib::CString m_sWorkingPath;
		ILoggerPtr m_pLogger;
		CPageCipher* m_pPageCliper;
	};
}

 