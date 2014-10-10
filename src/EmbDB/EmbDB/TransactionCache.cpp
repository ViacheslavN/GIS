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
			TPages::iterator it = m_pages.find(pRemPage->getAddr());
			assert(it != m_pages.end());
			sFileTranPageInfo& pi = it->second;
			if((pi.m_nFlags & (eFP_CHANGE | eFP_NEW)) &&  ! (pi.m_nFlags & eFP_REMOVE))
			{
				int64 nRet = m_pFileStorage->saveFilePage(pRemPage, pi.m_nFileAddr);
				if(pi.m_nFileAddr == -1)
					nRet = pi.m_nFileAddr;
			}
			delete pRemPage;
		}
	}
	void CTransactionsCache::savePage(CFilePage *pPage)
	{

		TPages::iterator it = m_pages.find(pPage->getAddr());
		assert(it != m_pages.end());
		sFileTranPageInfo& pi = it->second;
		pi.m_nFlags |= eFP_CHANGE;
		pPage->setFlag(eFP_CHANGE, true);
	//	m_pFileStorage->saveFilePage(pPage, pi->m_nFileAddr);
	}
	void CTransactionsCache::dropChange(IDBStorage *pStorage)
	{
		
	}

	 bool CTransactionsCache::savePageForUndo(IDBTransactions *pTran)
	 {

		 TPages::iterator it = m_pages.begin();
		 TPages::iterator it_end = m_pages.end();
		 for(;it != it_end; ++it)
		 {
			 sFileTranPageInfo& pi = it->second;
			 FilePagePtr pPage(m_Chache.GetElem(it->first, true));//pi.m_pPage;
			 if(!pPage.get())
			 {
				 assert(pi.m_nFileAddr != -1);
				 pPage = m_pFileStorage->getFilePage(pi.m_nFileAddr);
				 pPage->setFlag(pi.m_nFlags, true);
				 pPage->setAddr(it->first);
			 }

			 bool bNew = (pPage->getFlags() & eFP_NEW) != 0;
			 bool bChange = (pPage->getFlags() & eFP_CHANGE) != 0;
			 bool bRemove = (pPage->getFlags() & eFP_REMOVE) != 0;

			 if(!bNew && (bChange || bRemove))
				 pTran->addUndoPage(pPage);
		 }
		 return true;
	 }
	void CTransactionsCache::saveChange(IDBStorage *pStorage)
	{
		/*if(m_bInBtree)
			SavePages(m_BTreePageHolder,  pStorage);	 
		else
			SavePages(m_mapPageHolder,  pStorage);*/

		TPages::iterator it = m_pages.begin();
		TPages::iterator it_end = m_pages.end();
		int64 nCount = 0;
		for(;it != it_end; ++it)
		{
			++nCount;
			sFileTranPageInfo& pi = it->second;
			FilePagePtr pPage(m_Chache.GetElem(it->first, true));//pi.m_pPage;
			if(!pPage.get())
			{
				assert(pi.m_nFileAddr != -1);
				pPage = m_pFileStorage->getFilePage(pi.m_nFileAddr);
				pPage->setFlag(pi.m_nFlags, true);
				pPage->setAddr(it->first);
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
		}
		m_Chache.clear();
		m_Chache.m_set.destroyTree();
		m_pages.clear();
	}
	void CTransactionsCache::saveInBTree()
	{
		/*TPages::iterator it = m_pages.begin();
		while(!it.isNull())
		{
			m_BTreePages->insert(it.key(), it.value());
			it.next();
		}
		m_pages.clear();*/
	}

	void  CTransactionsCache::AddPage(int64 nAddr, int64 nTranAddr, CFilePage* pPage, bool bAddBack)
	{

		m_pages.insert(std::make_pair(nAddr, sFileTranPageInfo(nTranAddr, pPage->getFlags())));
		/*m_pCurPageHolder->insert(nAddr, sFileTranPageInfo(nTranAddr, pPage->getFlags()));
		if(!m_bInBtree)
		{
			if(m_pages.size() > 1000000)
			{
				m_BTreePages.reset(new TBTreePlus(-1, &m_BPStorage, m_pAlloc, 1000, false ));
				saveInBTree();
				m_BTreePageHolder.m_pSet = m_BTreePages.get();
				m_pCurPageHolder = &m_BTreePageHolder;
				m_bInBtree = true;
			}
		}*/

		CheckCache();
		m_Chache.AddElem(nAddr, pPage);
	}

 
	CFilePage*  CTransactionsCache::GetPage(int64 nAddr, bool bNotMove, bool bRead)
	{
		CFilePage* pPage = m_Chache.GetElem(nAddr);
		if(pPage)
			return pPage;
		TPages::iterator it = m_pages.find(nAddr);//m_pCurPageHolder->find(nAddr);
		if(it == m_pages.end())
			return NULL;

		sFileTranPageInfo& PageInfo = it->second;
		if(PageInfo.m_nFileAddr == -1)
		{
			CFilePage* pPage = new CFilePage(m_pAlloc, m_pFileStorage->getPageSize(), -1);
			m_Chache.AddElem(nAddr, pPage);
			return pPage;
		}
				
		pPage = m_pFileStorage->getFilePage(PageInfo.m_nFileAddr, bRead);
		assert(pPage);
		pPage->setAddr(nAddr);
		pPage->setFlag(PageInfo.m_nFlags, true);
		CheckCache();
		m_Chache.AddElem(nAddr, pPage);
		return pPage;
	}
	void CTransactionsCache::clear()
	{
		m_Chache.clear();
		m_Chache.m_set.clear();
		m_pages.clear();
	/*	if(m_bInBtree)
			m_BTreePageHolder.clear();
		else
			m_mapPageHolder.clear();*/
	}
} 