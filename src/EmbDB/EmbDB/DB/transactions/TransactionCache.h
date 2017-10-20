#ifndef _EMBEDDED_DATABASE_TRANSACTIONS_CACHE_H_
#define _EMBEDDED_DATABASE_TRANSACTIONS_CACHE_H_


//#include "CacheLRU.h"
#include "../../utils/CacheLRU_2Q.h"
#include "TranStorage.h" 
#include "../../storage/FilePage.h"
#include "../../embDBInternal.h"
//#include "BPTreeInfoPage.h"
//#include "BaseBPMapv2.h"
#include <map>
#include "TranPerfCounter.h"

namespace embDB
{
	class CTranRedoPageManager;
	class CTransaction;
	class CTransactionCache
	{
	public:

		struct sFileTranPageInfo
		{
			sFileTranPageInfo(int64  nAddr = -1, int nFlags = -1/*, uint32 nPageSize = 0, bool bNeedCrypt = true*/) : m_nFileAddr(nAddr), m_nFlags(nFlags),
				/* m_nPageSize(nPageSize),m_bNeedCrypt(bNeedCrypt), */m_nSaveOrignAddr(-1)
			{}
			int64  m_nFileAddr; // address in log transaction
			uint32 m_nFlags; //flags
			//uint32 m_nPageSize;
			//bool m_bNeedCrypt;
			int64 m_nSaveOrignAddr;// for undo
		};

		CTransactionCache(CommonLib::alloc_t* pAlloc, CTranStorage *pStorage, CTransaction *pTransaction, CTranPerfCounter *pCounter, uint32 nTranCache) : 
			    m_pFileStorage(pStorage)
			  , m_nMaxPageBuf(nTranCache)
			  , m_Chache(pAlloc, FilePagePtr(nullptr))
			  , m_pAlloc(pAlloc)
			  , m_pTransaction(pTransaction)
			  , m_pCounter(pCounter)
		  {  }
		  ~CTransactionCache()
		  {
			
		  }
		  void AddPage(int64 nAddr, int64 nTranAddr, CFilePage* pPage, bool bAddBack = false);
		  void DropPage(int64 nAddr, CFilePage* pAddPage) {  }
		  FilePagePtr GetPage(int64 nAddr, bool bNotMove = false, bool bRead = true, uint32 nSize = 0);
		
		  void saveChange(IDBStorage *pStorage);
		  void CheckCache();
		  void savePage(CFilePage *Page);
		  void clear();
		  bool savePageForUndo(CTransaction *pTran, IDBStorage *pStorage, int64 nRootPage);
		  bool savePageForRedo(CTransaction *pTran,  int64 nRootPage);

		  uint32 GetPageFlags(int64 nAddr);

		  bool undo(IDBStorage* pDBStorage, int64 nRootPage);
	public:
		struct TPageFreeChecker
		{
			bool IsFree(FilePagePtr& pObj)
			{


				return  true;
				//return pObj->IsFree();
			}
		};



		typedef std::map<int64, sFileTranPageInfo> TPages; 
		TPages m_pages;
		typedef TCacheLRU_2Q<int64, FilePagePtr, TPageFreeChecker> TNodesCache;
		TNodesCache m_Chache; //страницы в памяти
		CTranStorage * m_pFileStorage;
		uint32 m_nMaxPageBuf;
		CommonLib::alloc_t* m_pAlloc;
		CTransaction* m_pTransaction;
		CTranPerfCounter *m_pCounter;
		
	};   


}

#endif