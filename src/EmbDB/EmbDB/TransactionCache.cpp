#include "stdafx.h"
#include "TransactionCache.h"
#include "Transactions.h"
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
			if(pi.m_nFlags & (eFP_CHANGE | eFP_NEW))
			{
				int64 nRet = m_pFileStorage->saveFilePage(pRemPage, pi.m_nFileAddr);
				if(pi.m_nFileAddr == -1)
				{
					pi.m_nFileAddr = nRet;
					assert(pi.m_nFileAddr != -1);
				}


				if(!pi.m_bOrignSave)
				{
					if( !(pi.m_nFlags & eFP_NEW) && (pi.m_nFlags & eFP_CHANGE) &&
						(m_pTransaction->getRestoreType() == rtUndo ||  m_pTransaction->getRestoreType() == rtUndoRedo) )
					{
						 
						m_pTransaction->addUndoPage(FilePagePtr(pRemPage), true);
					}
					pi.m_bOrignSave = true;
				}
			
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

			 if(pi.m_bOrignSave)
				 continue;

			 bool bNew = (pi.m_nFlags & eFP_NEW) != 0;
			 bool bChange = (pi.m_nFlags & eFP_CHANGE) != 0;
		//	 bool bRemove = (pi.m_nFlags & eFP_REMOVE) != 0;

			 if(!bNew && bChange)
			 {			

				 bool bRemPage = false;
				 FilePagePtr pPage(m_Chache.GetElem(it->first, true));//pi.m_pPage;
				 if(!pPage.get())
				 {
					 assert(pi.m_nFileAddr != -1);
					 pPage = m_pFileStorage->getFilePage(pi.m_nFileAddr);
					 pPage->setFlag(pi.m_nFlags, true);
					 pPage->setAddr(it->first);
					 bRemPage = true;
					// m_Chache.AddElem(it->first, pPage.get());
				 }
				 pTran->addUndoPage(pPage, true);
				 if(bRemPage)
					 delete pPage.release();
			 }
		 }
		 return true;
	 }
	void CTransactionsCache::saveChange(IDBStorage *pStorage)
	{

		TPages::iterator it = m_pages.begin();
		TPages::iterator it_end = m_pages.end();
		int64 nCount = 0;
		for(;it != it_end; ++it)
		{
			++nCount;
			sFileTranPageInfo& pi = it->second;
			bool bNew = (pi.m_nFlags & eFP_NEW) != 0;
			bool bChange = (pi.m_nFlags & eFP_CHANGE) != 0;

			
			if(bNew || bChange)
			{
				FilePagePtr pPage(m_Chache.GetElem(it->first, true));
				if(!pPage.get())
				{
					assert(pi.m_nFileAddr != -1);
					pPage = m_pFileStorage->getFilePage(pi.m_nFileAddr);
					pPage->setFlag(pi.m_nFlags, true);
					pPage->setAddr(it->first);
				}

				bool bNew = (pPage->getFlags() & eFP_NEW) != 0;
				bool bChange = (pPage->getFlags() & eFP_CHANGE) != 0;
				bool bFromFree = (pPage->getFlags() & eFP_FROM_FREE_PAGES) != 0;
				if(bNew || bChange)
				{
					if(bNew && !bFromFree)
					{
						
						pStorage->saveNewPage(pPage);
						m_pCounter->WriteNewDBPage();
					}
					else
					{
						pStorage->saveFilePage(pPage, 0,  true);
						m_pCounter->WriteDBPage();
					}

				}
				delete pPage.release();
			}
		
		}
		m_Chache.clear();
		m_pages.clear();
	}
	
	void  CTransactionsCache::AddPage(int64 nAddr, int64 nTranAddr, CFilePage* pPage, bool bAddBack)
	{

		m_pages.insert(std::make_pair(nAddr, sFileTranPageInfo(nTranAddr, pPage->getFlags())));
		CheckCache();
		m_Chache.AddElem(nAddr, pPage);
	}

 
	CFilePage*  CTransactionsCache::GetPage(int64 nAddr, bool bNotMove, bool bRead)
	{
		CFilePage* pPage = m_Chache.GetElem(nAddr);
		if(pPage)
			return pPage;
		TPages::iterator it = m_pages.find(nAddr);
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
		m_pages.clear();
	}
} 