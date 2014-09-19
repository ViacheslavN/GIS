#ifndef _EMBEDDED_DATABASE_C_TRANSACTIONS_H_
#define _EMBEDDED_DATABASE_C_TRANSACTIONS_H_


#include "TransactionCache.h"
#include "IDBTransactions.h"
#include "IDBStorage.h"
#include "TranStorage.h"
#include "RBMap.h"
#include <vector>
#include "TranStorage.h"
#include "DBMagicSymbol.h"
#include "TranUndoPageManager.h"
#include "TranLogStateManager.h"
#include "RBSet.h"
namespace embDB
{
	//class CTransactionsCache;
	struct sTransactionHeader
	{
		sTransactionHeader() :
			nRestoreType(-1)
			,nPageChangeHeader(-1)
			,nPageUndoHeader(-1)
			,nErrorLogHeader(-1)
			,nMesageLogHeader(-1)
			,nPageStateHeader(-1)
			{}
		int32 nRestoreType;
		int64 nPageChangeHeader;
		int64 nPageUndoHeader;
		int64 nErrorLogHeader;
		int64 nMesageLogHeader;
		int64 nPageStateHeader;
	};

	struct sTransactionSatateHeader
	{
		int32 nTime;
		int32 nState;
	};
	class CTransactions : public IDBTransactions
	{
	public:
		CTransactions(CommonLib::alloc_t* pAlloc, eRestoreType nRestoreType,
			eTransactionsType nTranType, const CommonLib::str_t& sFileName, IDBStorage* pDBStorage, int64 nID);

		CTransactions(CommonLib::alloc_t* pAlloc, const CommonLib::str_t& sFileName, IDBStorage* pDBStorage);

		~CTransactions();

		//ITransactions
		virtual bool begin();
		virtual bool commit();
		virtual bool rollback();



		virtual bool restore();


		bool close();
		

		virtual FilePagePtr getFilePage(int64 nAddr, bool bRead = true);
		virtual void dropFilePage(FilePagePtr pPage);
		virtual void dropFilePage(int64 nAddr);
		virtual FilePagePtr getNewPage();
		virtual void saveFilePage(FilePagePtr pPage,  size_t nSize = 0, bool bChandgeInCache = false);
		virtual size_t getPageSize() const;
		virtual eTransactionsType getType() const {return (eTransactionsType)m_nTranType;}

		virtual FilePagePtr getTranNewPage();
		virtual FilePagePtr getTranFilePage(int64 nAddr, bool bRead = true);
		virtual void saveTranFilePage(FilePagePtr pPage,  size_t nSize = 0,  bool bChandgeInCache = false);

		virtual void addInnerTransactions(IDBTransactions *pTran);


		virtual bool isError() const;
		virtual size_t getErrorMessageSize() const;
		virtual size_t getErroMessage(wchar_t * pBuf, size_t nSize) const;

		virtual void error(const CommonLib::str_t& sError, uint32 nErrorID = 0);
		virtual uint32 getLogLevel() const ;
		virtual void log(uint32 nLevel, const CommonLib::str_t& sMessage);

		virtual void addDBBTree(IDBBtree *pTree);

		virtual int64 getID() const {return m_nID;}
		virtual const CommonLib::str_t& getFileTranName() const {return m_sFileName;}
		virtual bool isCompleted() const {return m_bIsCompleted;}
		virtual void setDBStorage(IDBStorage *pStorage)  {m_pDBStorage = pStorage;}
		virtual void wait() {}
		virtual void stop() {}
	private:

		
		bool SaveDBPage(CFilePage* pPage);
		bool commit_undo();
		bool commit_redo();
	private:
		int m_nTranType;
		int m_nRestoreType;
		CommonLib::str_t m_sFileName;
		typedef std::vector<IDBBtree*> TBTrees;
		TBTrees m_btrees; 
		typedef  std::vector<IDBTransactions*> TInnerTransactions;
		CTranStorage m_TranStorage;
		CTransactionsCache m_PageChache;
		IDBStorage* m_pDBStorage;
		CTranUndoPageManager m_TranUndoManager;
		CTranLogStateManager m_LogStateManager;
 		CommonLib::alloc_t *m_pAlloc;
		sTransactionHeader m_Header;
		bool m_bError;
		int64 m_nID;

		bool m_bIsCompleted;
		bool m_bIsBegin;

		TInnerTransactions m_InnerTran;

		//std::set<int64> m_setRemovePages;
		//std::set<int64> m_setPagesFromFree;

		std::vector<int64> m_vecFreePages;
		std::vector<int64> m_vecRemovePages;
	};
}

#endif