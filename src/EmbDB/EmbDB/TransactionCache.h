#ifndef _EMBEDDED_DATABASE_TRANSACTIONS_CACHE_H_
#define _EMBEDDED_DATABASE_TRANSACTIONS_CACHE_H_


#include "CacheNodes.h"
#include "TranStorage.h" 
//#include "RBSet.h"
#include "FilePage.h"
#include "IDBStorage.h"
#include "BPTreeInfoPage.h"
#include "BaseBPMapv2.h"
#include <map>
#include "TranPerfCounter.h"
namespace embDB
{
	
	class CTransactions;
	class CTransactionsCache
	{
	public:
		CTransactionsCache(CommonLib::alloc_t* pAlloc, CTranStorage *pStorage, CTransactions *pTransaction, CTranPerfCounter *pCounter, uint32 nTranCache) : 
			    m_pFileStorage(pStorage)
			  , m_nPageInMemory(0)
			  , m_nMaxPageBuf(nTranCache)
			  , m_Chache(pAlloc)
			 // , m_bInBtree(false)
			 // , m_mapPageHolder(&m_pages)
			//  , m_BTreePageHolder(NULL)
			 // , m_BPStorage(pStorage, pAlloc)
			  , m_pAlloc(pAlloc)
			  , m_pTransaction(pTransaction)
			  , m_pCounter(pCounter)
		  {
			 // m_pCurPageHolder = &m_mapPageHolder;
		  }
		  ~CTransactionsCache()
		  {
			
		  }
		  void AddPage(int64 nAddr, int64 nTranAddr, CFilePage* pPage, bool bAddBack = false);
		  void DropPage(int64 nAddr, CFilePage* pAddPage) {  }
		   size_t size() const {return m_nPageInMemory;}
		  CFilePage* GetPage(int64 nAddr, bool bNotMove = false, bool bRead = true);
		
		  void saveChange(IDBStorage *pStorage);
		  void dropChange(IDBStorage *pStorage);
		  void CheckCache();
		  void savePage(CFilePage *Page);
		  void clear();
		  bool savePageForUndo(IDBTransactions *pTran);
	public:
		typedef std::map<int64, sFileTranPageInfo> TPages; 
		TPages m_pages;
		typedef TSimpleCache<int64, CFilePage> TNodesCache;
		TNodesCache m_Chache; //страницы в памяти
		CTranStorage * m_pFileStorage;
		size_t m_nPageInMemory;
		size_t m_nMaxPageBuf;
		CommonLib::alloc_t* m_pAlloc;
		CTransactions* m_pTransaction;
		CTranPerfCounter *m_pCounter;
		
	};   


}

#endif