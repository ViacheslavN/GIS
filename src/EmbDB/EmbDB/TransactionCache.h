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
			  , m_BPStorage(pStorage, pAlloc)
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
		  void saveInBTree();
		  void clear();
		  bool savePageForUndo(IDBTransactions *pTran);
	public:
		/*typedef RBMap<int64, sFileTranPageInfo> TPages; //все страницы
			typedef TBPMapV2<
			int64, //key
			sFileTranPageInfo, //val
			embDB::comp<int64>, //comp
			BPNewPageStorage,
			 BPInnerNodeSimpleCompressorV2<int64>,
			BPNewPageLeafNodeCompressor //leafcomp
			  //innercomp
		> 	TBTreePlus;*/


		typedef std::map<int64, sFileTranPageInfo> TPages; 

		/*
		template <class _TKey, class _TValue, class _TLink, class _TComp, class _Transaction,
		class _TBreeNode = BPTreeNodeMap<_TKey, _TLink, _TComp, _Transaction> >
		*/
		TPages m_pages;
		//std::auto_ptr<TBTreePlus> m_BTreePages;// если страниц становиться "много" переключаться на B-tree
		typedef TSimpleCache<int64, CFilePage> TNodesCache;
		TNodesCache m_Chache; //страницы в памяти


		/*class PageHolder
		{
		public:
			virtual void insert (int64 nAddr, const sFileTranPageInfo& pi) = 0;
			//virtual void update (int64 nAddr, int nFlag) = 0;
			virtual sFileTranPageInfo* find(int64 nAddr) = 0;
			virtual sFileTranPageInfo* find_and_update(int64 nAddr, int nFlag) = 0;

		};

		template < typename _TMemset>
		class TPageHolder : public PageHolder
		{
		public:
			typedef _TMemset TSet;
			typedef typename TSet::iterator iterator;

			TPageHolder(TSet *pSet) : m_pSet(pSet)
			{}

			virtual void insert (int64 nAddr, const sFileTranPageInfo& pi)
			{
				m_pSet->insert(nAddr, pi);
			}
			virtual sFileTranPageInfo* find(int64 nAddr) 
			{
				iterator it = m_pSet->find(nAddr);
				if(it.isNull())
					return NULL;
				 return &it.value();

			}
			virtual sFileTranPageInfo*  find_and_update (int64 nAddr, int nFlag)
			{
				iterator it = m_pSet->find(nAddr);
				if(it.isNull())
					return NULL;
				sFileTranPageInfo&  pi = it.value();
				pi.m_nFlags = nFlag;
				it.update();
				return &pi;
			}
			iterator begin()
			{
				return m_pSet->begin();
			}
			void clear()
			{
				m_pSet->clear();
			}
		public:
			TSet* m_pSet;
		};

		template <typename TPageHolder>
		void SavePages(TPageHolder& pages, IDBStorage *pStorage)
		{
			//pStorage->lockWrite();
			TPageHolder::iterator it = pages.begin();
			int64 nCount = 0;
			while(!it.isNull())
			{
				++nCount;
				sFileTranPageInfo& pi = it.value();
				FilePagePtr pPage(m_Chache.GetElem(it.key(), true));//pi.m_pPage;
				if(!pPage.get())
				{
					assert(pi.m_nFileAddr != -1);
					pPage = m_pFileStorage->getFilePage(pi.m_nFileAddr);
					pPage->setFlag(pi.m_nFlags, true);
					pPage->setAddr(it.key());
				}
				
				bool bNew = (pPage->getFlags() & eFP_NEW) != 0;
				bool bChange = (pPage->getFlags() & eFP_CHANGE) != 0;
				bool bRemove = (pPage->getFlags() & eFP_REMOVE) != 0;
				if(bRemove)
				{
					if(!bNew)
						pStorage->dropFilePage(pPage);
				}
				else if(bNew || bChange)
				{
					if(bNew)
						pStorage->saveNewPage(pPage);
					else
						pStorage->saveFilePage(pPage, 0,  true);

				}
				delete pPage.release();
				it.next();
			}
			//pStorage->unlockWrite();
			m_Chache.clear();
			m_Chache.m_set.destroyTree();
			pages.clear();
		}*/
		//typedef TPageHolder<TPages> TPageMapHolder;
		//typedef TPageHolder<TBTreePlus> TPageBTreeHolder;


		//PageHolder *m_pCurPageHolder;
	//	TPageMapHolder m_mapPageHolder;
	//	TPageBTreeHolder m_BTreePageHolder;
		CTranStorage * m_pFileStorage;
		size_t m_nPageInMemory;
		size_t m_nMaxPageBuf;
		//bool m_bInBtree;
		BPNewPageStorage m_BPStorage;
		CommonLib::alloc_t* m_pAlloc;
		CTransactions* m_pTransaction;
		CTranPerfCounter *m_pCounter;
		
	};   


}

#endif