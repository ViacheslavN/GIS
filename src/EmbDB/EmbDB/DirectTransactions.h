#ifndef _EMBEDDED_DATABASE_C_DIRECT_TRANSACTIONS_H_
#define _EMBEDDED_DATABASE_C_DIRECT_TRANSACTIONS_H_

#include "embDBInternal.h"
#include "CommonLibrary/alloc_t.h"
#include <set>
#include <iostream>
#include "TransactionBase.h"
namespace embDB
{
	class CDatabase;
	class CDirectTransaction :  public ITransactionBase<IDBTransaction>
	{
	public:
		typedef ITransactionBase<IDBTransaction> TBase;
		CDirectTransaction(CommonLib::alloc_t* pAlloc,  IDBStorage* pDBStorage, uint32 nTranCache = 10000);
		//for compatible tests
		CDirectTransaction(CommonLib::alloc_t* pAlloc, eRestoreType nRestoreType,
			eTransactionType nTranType, const CommonLib::CString& sFileName, IDBStorage* pDBStorage, int64 nID, uint32 nTranCache = 10000);

		CDirectTransaction(CommonLib::alloc_t* pAlloc, eRestoreType nRestoreType,
			eTransactionType nTranType, const CommonLib::CString& sFileName, CDatabase* pDatabase, int64 nID, uint32 nTranCache = 10000);
		~CDirectTransaction();

		//ITransactions
		virtual bool begin(){return true;}
		virtual bool commit();
		virtual bool rollback(){return true;}
		virtual bool restore(bool Force = false){return true;}
		bool close(){return true;}


		virtual IStatementPtr createStatement(const wchar_t *pszSQLQuery) {return  IStatementPtr();}
		virtual ICursorPtr executeQuery(IStatement* pStatement) {return  ICursorPtr();}
		virtual ICursorPtr executeQuery(const wchar_t* pszQuery = NULL) {return  ICursorPtr();}

 
		virtual IUpdateCursorPtr createUpdateCursor() {return  IUpdateCursorPtr();}


		virtual FilePagePtr getFilePage(int64 nAddr, uint32 nSize, bool bRead = true);
		virtual void dropFilePage(FilePagePtr pPage);
		virtual void dropFilePage(int64 nAddr, uint32 nSize);
		virtual FilePagePtr getNewPage(uint32 nSize, bool bWrite = false);
		virtual bool saveFilePage(FilePagePtr pPage,  size_t nSize = 0, bool bChandgeInCache = false);
		virtual bool saveFilePage(CFilePage* pPage, size_t nDataSize = 0,  bool ChandgeInCache = false);
 

 
		virtual eTransactionType getType() const {return eTT_UNDEFINED;}

		virtual void addInnerTransactions(IDBTransaction *pTran){}

		virtual void addUndoPage(FilePagePtr pPage, bool bReadFromDB = false){}
		virtual bool isError() const
		{
			return m_bError;
		}
		virtual size_t getErrorMessageSize() const
		{
			return 0;
		}
		virtual size_t getErroMessage(wchar_t * pBuf, size_t nSize) const
		{
			return 0;
		}

		virtual void error(const wchar_t *pszFormat, ...)
		{
			m_bError = true;
		}
		virtual uint32 getLogLevel() const 
		{
			return 0;
		}
		virtual void log(uint32 nLevel, const wchar_t *pszFormat, ...)
		{

		}

		virtual void addDBBTree(IDBBtree *pTree){}

		virtual int64 getID() const {return 0;}
		virtual const CommonLib::CString& getFileTranName() const {return m_sFileName;}
		virtual bool isCompleted() const {return true;}
		virtual void setDBStorage(IDBStorage *pStorage)  {m_pDBStorage = pStorage;}
		virtual void wait() {}
		virtual void stop() {}


		virtual FilePagePtr getTranFilePage(int64 nAddr, uint32 nSize, bool bRead = true)
		{
			return getFilePage(nAddr, nSize, bRead);
		}
		virtual void saveTranFilePage(FilePagePtr pPage,  size_t nSize = 0,  bool bChandgeInCache = false) 
		{
			return saveTranFilePage(pPage, nSize, bChandgeInCache);
		}
		virtual FilePagePtr getTranNewPage(uint32 nSize = 0)
		{
			return getNewPage(nSize);
		}
		void OutDebugInfo(){}
	private:
		IDBStorage* m_pDBStorage;
		bool m_bError;
		CommonLib::CString m_sFileName;
		std::set<int64> m_setRemovePages;
		std::set<int64> m_setPagesFromFree;
};
}
#endif