#include "stdafx.h"
#include "TransactionCache.h"

namespace embDB
{

	void CTransactionsCache::CheckCache()
	{
		if(m_Chache.size() > m_nMaxPageBuf)
		{
			CFilePage* pRemPage = m_Chache.remove_back();
			assert(pRemPage);
			sFileTranPageInfo* pi = m_pCurPageHolder->find(pRemPage->getAddr());
			assert(pi != NULL);
			m_pFileStorage->saveFilePage(pRemPage, pi->m_nFileAddr);
			delete pRemPage;
		}
	}
	void CTransactionsCache::savePage(CFilePage *pPage)
	{
		sFileTranPageInfo* pi = m_pCurPageHolder->find_and_update(pPage->getAddr(),  pPage->getFlags());
		assert(pi);
		m_pFileStorage->saveFilePage(pPage, pi->m_nFileAddr);
	}
	void CTransactionsCache::dropChange(IDBStorage *pStorage)
	{
		
	}
	void CTransactionsCache::saveChange(IDBStorage *pStorage)
	{
		if(m_bInBtree)
			SavePages(m_BTreePageHolder,  pStorage);	 
		else
			SavePages(m_mapPageHolder,  pStorage);
	}
	void CTransactionsCache::saveInBTree()
	{
		TPages::iterator it = m_pages.begin();
		while(!it.isNull())
		{
			m_BTreePages->insert(it.key(), it.value());
			it.next();
		}
		m_pages.clear();
	}

	void  CTransactionsCache::AddPage(int64 nAddr, int64 nTranAddr, CFilePage* pPage, bool bAddBack)
	{
		m_pCurPageHolder->insert(nAddr, sFileTranPageInfo(nTranAddr, pPage->getFlags()));
		if(!m_bInBtree)
		{
			if(m_pages.size() > 1000000)
			{
				m_BTreePages.reset(new TBTreePlus(/*m_pFileStorage->getNewPageAddr()*/-1,
					&m_BPStorage, m_pAlloc, 50, false ));
				//m_BTreePages->setPageBTreeInfo(m_pFileStorage->getNewPageAddr());
				saveInBTree();
				m_BTreePageHolder.m_pSet = m_BTreePages.get();
				m_pCurPageHolder = &m_BTreePageHolder;
				m_bInBtree = true;
			}
		}

		CheckCache();
		m_Chache.AddElem(nAddr, pPage);
	}

 
	CFilePage*  CTransactionsCache::GetPage(int64 nAddr, bool bNotMove, bool bRead)
	{
		CFilePage* pPage = m_Chache.GetElem(nAddr);
		if(pPage)
			return pPage;
		sFileTranPageInfo* pPageInfo = m_pCurPageHolder->find(nAddr);
		if(!pPageInfo)
			return NULL;
		assert(pPageInfo->m_nFileAddr != -1);
		pPage = m_pFileStorage->getFilePage(pPageInfo->m_nFileAddr, bRead);
		assert(pPage);
		pPage->setAddr(nAddr);
		pPage->setFlag(pPageInfo->m_nFlags, true);
		CheckCache();
		m_Chache.AddElem(nAddr, pPage);
		return pPage;
	}
	void CTransactionsCache::clear()
	{
		m_Chache.clear();
		m_Chache.m_set.clear();
		if(m_bInBtree)
			m_BTreePageHolder.clear();
		else
			m_mapPageHolder.clear();
	}
} 