#ifndef _EMBEDDED_DATABASE_C_TRANSACTIONS_H_
#define _EMBEDDED_DATABASE_C_TRANSACTIONS_H_

#include "../../embDBInternal.h"
#include "TransactionCache.h"
#include "TranStorage.h"
#include <vector>
#include "TranUndoPageManager.h"
#include "TranRedoPageManager.h"
#include "TranLogStateManager.h"
#include "TranPerfCounter.h"
#include "TransactionBase.h"
namespace embDB
{
	//class CTransactionsCache;

	class CDatabase;
	struct sTransactionHeader
	{
		sTransactionHeader() :
			nRestoreType(-1)
			,nPageChangeHeader(-1)
			,nPageUndoRedoHeader(-1)
			,nErrorLogHeader(-1)
			,nMesageLogHeader(-1)
			,nPageStateHeader(-1)
			,nFreeUndoPage(-1)
			{}
		int32 nRestoreType;
		int64 nPageChangeHeader;
		int64 nPageUndoRedoHeader;
		int64 nErrorLogHeader;
		int64 nMesageLogHeader;
		int64 nPageStateHeader;
		int64 nFreeUndoPage;
	};

	struct sTransactionSatateHeader
	{
		int32 nTime;
		int32 nState;
	};
	class CTransaction : public ITransactionBase<IDBTransaction>
	{
	public:
		typedef ITransactionBase<IDBTransaction> TBase;

		CTransaction(CommonLib::alloc_t* pAlloc, eRestoreType nRestoreType,
			eTransactionDataType nTranType, const CommonLib::CString& sFileName,
			IDBStorage* pDBStorage, int64 nID = -1, uint32 nTranCache = 10000, CPageCipher *pPageCiher = nullptr);

		CTransaction(CommonLib::alloc_t* pAlloc, eRestoreType nRestoreType,
			eTransactionDataType nTranType, const CommonLib::CString& sFileName, IDBConnection* pConnection, int64 nID = -1, uint32 nTranCache = 10000, CPageCipher *pPageCiher = nullptr);


		//CTransaction(CommonLib::alloc_t* pAlloc, const CommonLib::CString& sFileName, IDBStorage* pDBStorage, uint32 nTranCache = 10000);


		static const uint32 nCommonPageSize = 8192;

		

		~CTransaction();

		//ITransactions
		virtual bool begin();
		virtual bool commit();
		virtual bool rollback();



		virtual IStatementPtr createStatement(const wchar_t *pszSQLQuery) {return  IStatementPtr();}
		virtual ICursorPtr executeQuery(IStatement* pStatement) {return  ICursorPtr();}
		virtual ICursorPtr executeQuery(const wchar_t* pszQuery = NULL) {return  ICursorPtr();}

 
		virtual IUpdateCursorPtr createUpdateCursor() {return  IUpdateCursorPtr();}



		virtual bool restore(bool bForce = false);


		bool close();
		

		virtual FilePagePtr getFilePage(int64 nAddr, uint32 nSize, bool bRead = true, bool bNeedDecrypt = true);
		virtual void dropFilePage(FilePagePtr pPage);
		virtual void dropFilePage(int64 nAddr, uint32 nSize);
		virtual FilePagePtr getNewPage(uint32 nSize, bool bWrite = false);
		virtual bool saveFilePage(FilePagePtr pPage,  uint32 nSize = 0, bool bChangeInCache = false);
		virtual bool saveFilePage(CFilePage* pPage, uint32 nDataSize = 0,  bool ChandgeInCache = false);
 

 
		virtual eTransactionDataType getType() const {return (eTransactionDataType)m_nTranType;}

		virtual FilePagePtr getTranNewPage(uint32 nSize);
		virtual FilePagePtr getTranFilePage(int64 nAddr, uint32 nSize, bool bRead = true, bool bNeedDecrypt = true);
		virtual void saveTranFilePage(FilePagePtr pPage,  uint32 nSize = 0,  bool bChandgeInCache = false);

		virtual void addUndoPage(FilePagePtr pPage, bool bReadFromDB = false);
		 
		virtual void addInnerTransactions(IDBTransaction *pTran);



	



		virtual void addDBBTree(IDBBtree *pTree);

		virtual int64 getID() const {return m_nID;}
		virtual const CommonLib::CString& getFileTranName() const {return m_sFileName;}
		virtual bool isCompleted() const {return m_bIsCompleted;}
		virtual void setDBStorage(IDBStorage *pStorage)  {m_pDBStorage = pStorage;}
		virtual void wait() {}
		virtual void stop() {}

		int getRestoreType() {return m_nRestoreType;}


		void OutDebugInfo();

		void setDeleteStorage(bool bDel){m_bDeleteStorage = bDel;}

		FilePagePtr getFileDBPage(int64 nAddr, uint32 nPageSize, bool bRead = true);

		virtual eDBTransationType getDBTransationType() const {return eTTFullTransaction;}
	private:
		bool SaveDBPage(CFilePage* pPage);
		bool SaveDBPage(int64 nAddr);
		bool commit_undo();
		bool commit_redo();
		bool restore_undo(bool bForce = false);
		bool restore_redo(bool bForce = false);

	private:
		int m_nTranType;
		int m_nRestoreType;
		CommonLib::CString m_sFileName;
		typedef std::vector<IDBBtree*> TBTrees;
		TBTrees m_btrees; 
		typedef  std::vector<IDBTransaction*> TInnerTransactions;
		CTranStorage m_TranStorage;
		CTransactionsCache m_PageChache;
	
		CTranUndoPageManager m_TranUndoManager;
		CTranRedoPageManager m_TranRedoManager;
		CTranLogStateManager m_LogStateManager;
 
		sTransactionHeader m_Header;
 
		int64 m_nID;

		bool m_bIsCompleted;
		bool m_bIsBegin;
		bool m_bDeleteStorage;

		uint32 m_nPageSize;

		TInnerTransactions m_InnerTran;

		//std::set<int64> m_setRemovePages;
		//std::set<int64> m_setPagesFromFree;

		std::vector<int64> m_vecFreePages;
		std::vector<int64> m_vecRemovePages;

		///////DEBUG UTILS/////////
		CTranPerfCounter m_TranPerfCounter;

	};
}

#endif