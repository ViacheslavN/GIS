#ifndef _EMBEDDED_DATABASE_C_DIRECT_TRANSACTIONS_H_
#define _EMBEDDED_DATABASE_C_DIRECT_TRANSACTIONS_H_

#include "../../../embDBInternal.h"
#include "CommonLibrary/alloc_t.h"
#include <set>
#include <iostream>
#include "../TransactionBase.h"
#include "../TranStorage.h"
namespace embDB
{
	class CDatabase;
	class CDirectTransaction :  public ITransactionBase<IDBTransaction>
	{
	public:
		typedef ITransactionBase<IDBTransaction> TBase;
		 
		//for compatible tests
		CDirectTransaction(CommonLib::alloc_t* pAlloc, eRestoreType nRestoreType,
			eTransactionDataType nTranType, const CommonLib::CString& sFileName, 
			IDBStorage* pDBStorage, int64 nID = -1, uint32 nTranCache = 10000,
			eDBTransationType tranType = eTTDirectTransaction);

		CDirectTransaction(CommonLib::alloc_t* pAlloc, eRestoreType nRestoreType,
			eTransactionDataType nTranType, const CommonLib::CString& sFileName, 
			IDBConnection* pConnection, int64 nID = -1, uint32 nTranCache = 10000,
			eDBTransationType tranType = eTTDirectTransaction);
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


		virtual FilePagePtr getFilePage(int64 nAddr, uint32 nSize, bool bRead = true, bool bNeedDecrypt = true, bool bAddInCache = true);
		virtual void dropFilePage(FilePagePtr pPage);
		virtual void dropFilePage(int64 nAddr, uint32 nSize);
		virtual FilePagePtr getNewPage(uint32 nSize = 0, bool bWrite = false, bool bAddInCache = true);
		virtual bool saveFilePage(FilePagePtr pPage,   bool bChandgeInCache = false);
		virtual bool saveFilePage(CFilePage* pPage,   bool ChandgeInCache = false);
 

 
		virtual eTransactionDataType getType() const {return eTT_UNDEFINED;}

		virtual void addInnerTransactions(IDBTransaction *pTran){}

		virtual void addUndoPage(FilePagePtr pPage, bool bReadFromDB = false){}
	 
		virtual void addDBBTree(IDBBtree *pTree){}

		virtual int64 getID() const {return 0;}
		virtual const CommonLib::CString& getFileTranName() const {return m_sFileName;}
		virtual bool isCompleted() const {return true;}
		virtual void setDBStorage(IDBStorage *pStorage)  {m_pDBStorage = pStorage;}
		virtual void wait() {}
		virtual void stop() {}


		virtual FilePagePtr getTranFilePage(int64 nAddr, uint32 nSize, bool bRead = true, bool bNeedDecrypt = true)
		{
			return getFilePage(nAddr, nSize, bRead, bNeedDecrypt);
		}
		virtual void saveTranFilePage(FilePagePtr pPage,  uint32 nSize = 0,  bool bChandgeInCache = false) 
		{
			return saveTranFilePage(pPage, nSize, bChandgeInCache);
		}
		virtual FilePagePtr getTranNewPage(uint32 nSize = 0)
		{
			return getNewPage(nSize);
		}
		void OutDebugInfo(){}

		virtual eDBTransationType getDBTransationType() const {return m_TranType;}
	private:
		IDBStorage* m_pDBStorage;
		CTranStorage m_TranStorage;
 
		CommonLib::CString m_sFileName;
		std::set<int64> m_setRemovePages;
		std::set<int64> m_setPagesFromFree;

		eDBTransationType m_TranType;

 
};
}
#endif