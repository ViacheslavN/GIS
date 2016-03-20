#ifndef _EMBEDDED_DATABASE_TRANSACTIONS_CACHE_H_
#define _EMBEDDED_DATABASE_TRANSACTIONS_CACHE_H_


#include "CacheLRU.h"
#include "TranStorage.h" 
#include "FilePage.h"
#include "embDBInternal.h"
//#include "BPTreeInfoPage.h"
//#include "BaseBPMapv2.h"
#include <map>
#include "TranPerfCounter.h"

namespace embDB
{
	class CTranRedoPageManager;
	class CTransaction;
	class CTransactionsCache
	{
	public:

		struct sFileTranPageInfo
		{
			sFileTranPageInfo(int64  nAddr = -1, int nFlags = -1, uint32 nPageSize = 0) : m_nFileAddr(nAddr), m_nFlags(nFlags), m_bOrignSave(false),
				m_bRedoSave(false), m_nPageSize(nPageSize)
			{}
			int64  m_nFileAddr; // адрес страницы в файле транзакций
			uint32 m_nFlags; //флаги выгружаемой страницы
			bool m_bOrignSave; // for undo
			bool m_bRedoSave; // for redo
			uint32 m_nPageSize;
		};

		CTransactionsCache(CommonLib::alloc_t* pAlloc, CTranStorage *pStorage, CTransaction *pTransaction, CTranPerfCounter *pCounter, uint32 nTranCache) : 
			    m_pFileStorage(pStorage)
			  , m_nPageInMemory(0)
			  , m_nMaxPageBuf(nTranCache)
			  , m_Chache(pAlloc)
			  , m_pAlloc(pAlloc)
			  , m_pTransaction(pTransaction)
			  , m_pCounter(pCounter)
		  {  }
		  ~CTransactionsCache()
		  {
			
		  }
		  void AddPage(int64 nAddr, int64 nTranAddr, CFilePage* pPage, bool bAddBack = false);
		  void DropPage(int64 nAddr, CFilePage* pAddPage) {  }
		   uint32 size() const {return m_nPageInMemory;}
		  CFilePage* GetPage(int64 nAddr, bool bNotMove = false, bool bRead = true, uint32 nSize = 0);
		
		  void saveChange(IDBStorage *pStorage);
		  void dropChange(IDBStorage *pStorage);
		  void CheckCache();
		  void savePage(CFilePage *Page);
		  void clear();
		  bool savePageForUndo(IDBTransaction *pTran);
		  bool savePageForRedo(CTranRedoPageManager *pRepoPageManager);
	public:
		typedef std::map<int64, sFileTranPageInfo> TPages; 
		TPages m_pages;
		typedef TCacheLRU<int64, CFilePage> TNodesCache;
		TNodesCache m_Chache; //страницы в памяти
		CTranStorage * m_pFileStorage;
		uint32 m_nPageInMemory;
		uint32 m_nMaxPageBuf;
		CommonLib::alloc_t* m_pAlloc;
		CTransaction* m_pTransaction;
		CTranPerfCounter *m_pCounter;
		
	};   


}

#endif