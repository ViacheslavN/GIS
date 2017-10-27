#pragma  once


#include "../../../embDBInternal.h"
#include "CommonLibrary/alloc_t.h"
#include <set>
#include <iostream>
#include "../TransactionBase.h"
#include "../TranStorage.h"
namespace embDB
{
	class CDatabase;
	class CWALTransaction : public ITransactionBase<IDBTransaction>
	{
	public:
		typedef ITransactionBase<IDBTransaction> TBase;


		struct STranPage
		{
 			int64 m_nDBAddr;
			uint32 m_nSize;


			STranPage(int64 nDBAdd, uint32 nSize) :  m_nDBAddr(nDBAdd), m_nSize(nSize)
			{}
		};


		CWALTransaction(CommonLib::alloc_t* pAlloc, eRestoreType nRestoreType,
			eTransactionDataType nTranType, IDBWALStorage* pDBStorage,   uint32 nTranCache = 10000, CPageCipher *pPageCiher = nullptr, bool bMultiThread = true);

		CWALTransaction(CommonLib::alloc_t* pAlloc, eRestoreType nRestoreType,	eTransactionDataType nTranType,  IDBConnection* pConnection,   uint32 nTranCache = 10000, CPageCipher *pPageCiher = nullptr,bool bMultiThread = true);
	
		~CWALTransaction();

		//ITransactions
		virtual bool begin() { return true; }
		virtual bool commit();
		virtual bool rollback() { return true; }
		virtual bool restore(bool Force = false) { return true; }
		bool close() { return true; }


		virtual IStatementPtr createStatement(const wchar_t *pszSQLQuery) { return  IStatementPtr(); }
		virtual ICursorPtr executeQuery(IStatement* pStatement) { return  ICursorPtr(); }
		virtual ICursorPtr executeQuery(const wchar_t* pszQuery = NULL) { return  ICursorPtr(); }


		virtual IUpdateCursorPtr createUpdateCursor() { return  IUpdateCursorPtr(); }


		virtual FilePagePtr getFilePage(int64 nAddr, uint32 nSize = 0, bool bRead = true, bool bNeedDecrypt = true, bool bAddInCache = true, bool bForChange= true);
		virtual void dropFilePage(FilePagePtr pPage);
		virtual void dropFilePage(int64 nAddr, uint32 nSize);
		virtual FilePagePtr getNewPage(uint32 nSize = 0, bool bWrite = false, bool bAddInCache = true);
		virtual bool saveFilePage(FilePagePtr pPage,  bool bChandgeInCache = false);
		virtual bool saveFilePage(CFilePage* pPage, bool ChandgeInCache = false);



		virtual eTransactionDataType getType() const { return eTT_UNDEFINED; }

		virtual void addInnerTransactions(IDBTransaction *pTran) {}

		virtual void addUndoPage(FilePagePtr pPage, bool bReadFromDB = false) {}

		virtual void addDBBTree(IDBBtree *pTree) {}

		virtual int64 getID() const { return 0; }
		virtual const CommonLib::CString& getFileTranName() const { return m_sFileName; }
		virtual bool isCompleted() const { return true; }
		virtual void setDBStorage(IDBStorage *pStorage) { m_pDBStorage = pStorage; }
		virtual void setWALStorage(IDBWALStorage *pStorage);
		virtual void wait() {}
		virtual void stop() {}


		virtual FilePagePtr getTranFilePage(int64 nAddr, uint32 nSize, bool bRead = true, bool bNeedDecrypt = true)
		{
			return getFilePage(nAddr, nSize, bRead, bNeedDecrypt);
		}
		virtual void saveTranFilePage(FilePagePtr pPage, uint32 nSize = 0, bool bChandgeInCache = false)
		{
			return saveTranFilePage(pPage, nSize, bChandgeInCache);
		}
		virtual FilePagePtr getTranNewPage(uint32 nSize = 0)
		{
			return getNewPage(nSize);
		}
		void OutDebugInfo() {}

		virtual eDBTransationType getDBTransationType() const { return m_TranType; }
	private:
		uint32 GetRowPageInfoSize() const;
		void SaveFilePageInTranLog(CFilePage *pPage);
	private:
		IDBStorage* m_pDBStorage;
		IDBWALStoragePtr m_pWALStorage;
 

		CommonLib::CString m_sFileName;
		std::set<int64> m_setRemovePages;
		std::set<int64> m_setPagesFromFree;

		eDBTransationType m_TranType;


		struct TPageFreeChecker
		{
			bool IsFree(FilePagePtr& pObj)
			{
				return  pObj->isRemovable(1);
			}
		};

		typedef TCacheLRU_2Q<int64, FilePagePtr, TPageFreeChecker> TPageCache;
		TPageCache m_Cache;

		bool m_bMultiThread;
		uint32 m_nTranCache;

		typedef std::map<int64, STranPage> TTranPages;
		TTranPages m_TranPages;
		bool m_bOneSize;
		uint32 m_nPageSize;
		eTransactionDataType m_nTranType;

		std::map<int64, int64> m_ConvertAddr;
	};
}
 