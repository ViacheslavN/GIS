#ifndef _EMBEDDED_DATABASE_I_SIMPLE_TRANSACTIONS_CACHE_H_
#define _EMBEDDED_DATABASE_I_SIMPLE_TRANSACTIONS_CACHE_H_
#include "CacheNodes.h"
#include "SimpleTranStorage.h"
#include "RBSet.h"
#include "FilePage.h"
#include "IDBStorage.h"
namespace embDB
{
	struct sFilePageInfo
	{
		sFilePageInfo() : m_pPage(0), m_nFileAddr(-1), m_nFlags(0)
		{}
		sFilePageInfo(CFilePage* pPage, int64  nAddr = -1) : m_pPage(pPage), m_nFileAddr(nAddr)
		{}
		CFilePage* m_pPage; //если указатель нудевой значит страница выгружена в файл транзации
		int64  m_nFileAddr; // адрес страницы в файле транзакций
		int m_nFlags; //флаги выгружаемой страницы
	};


	class TSimpleTranCache
	{
	public:
		TSimpleTranCache(CommonLib::alloc_t* pAlloc, CSimpleTranStorage *pStorage) : 
			  m_pages(pAlloc)
			  ,m_pFileStorage(pStorage)
			  ,m_nPageInMemory(0)
			  ,m_nMaxPageBuf(100000)
			  ,m_Chache(pAlloc)
		  {}
		  ~TSimpleTranCache()
		  {
			
		  }

		/*  struct SCacheElem
		  {
			  SCacheElem() : pListEl(0)
			  {}
			  TCacheNode<int64> *pListEl;
			  sFilePageInfo m_PageInfo; 
		  };*/

		  void AddPage(int64 nAddr, CFilePage* pAddPage, bool bAddBack = false)
		  {
			  m_pages.insert(nAddr, sFilePageInfo(pAddPage, -1));
			  CheckCache();
			  m_Chache.AddElem(pAddPage->getAddr(), pAddPage);
		  }

		  size_t size() const {return m_nPageInMemory;}

		  CFilePage* GetPage(int64 nAddr, bool bNotMove = false)
		  {

			  CFilePage* pPage = m_Chache.GetElem(nAddr);
			  if(pPage)
				  return pPage;

			 TPages::iterator it = m_pages.find(nAddr);
			 if(it.isNull())
				 return NULL;
			 sFilePageInfo& pageInfo = it.value();
			 assert(pageInfo.m_pPage);
			 assert(pageInfo.m_nFileAddr != -1);
             pPage = m_pFileStorage->getFilePage(pageInfo.m_nFileAddr);
			 assert(pPage);
			 pageInfo.m_pPage = pPage;
			 pPage->setFlag(pageInfo.m_nFlags, true);
			 CheckCache();
			 m_Chache.AddElem(pPage->getAddr(), pPage);
			 return pPage;
		  }

		  void saveChange(IDBStorage *pStorage);
		  void dropChange(IDBStorage *pStorage);
		  void CheckCache();
		  void savePage(CFilePage *Page);
	public:
		typedef RBMap<int64, sFilePageInfo> TPages; //все страницы
		//typedef RBSet<int64> TPageInMemory;
		//TPageInMemory m_memory_pages;
		TPages m_pages;
		typedef TSimpleCache<int64, CFilePage> TNodesCache;
		 TNodesCache m_Chache; //страницы в памяти
		

		CSimpleTranStorage * m_pFileStorage;
		size_t m_nPageInMemory;
		size_t m_nMaxPageBuf;

	};   


}

#endif