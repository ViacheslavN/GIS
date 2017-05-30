#include "stdafx.h"
#include "TransactionCache.h"
#include "Transactions.h"
namespace embDB
{

	void CTransactionsCache::CheckCache()
	{
		if(m_Chache.size() > m_nMaxPageBuf)
		{
			FilePagePtr pRemPage = m_Chache.remove_back();
			assert(pRemPage.get());
			TPages::iterator it = m_pages.find(pRemPage->getAddr());
			assert(it != m_pages.end());
			sFileTranPageInfo& pi = it->second;
			if(pi.m_nFlags & (eFP_CHANGE | eFP_NEW))
			{
				int64 nRet = m_pFileStorage->saveFilePage(pRemPage.get(), pi.m_nFileAddr);
				if(pi.m_nFileAddr == -1)
				{
					pi.m_nFileAddr = nRet;
					assert(pi.m_nFileAddr != -1);
				}

				if(m_pTransaction->getRestoreType() == rtUndo)
				{
					if(!pi.m_bOrignSave)
					{
						if( !(pi.m_nFlags & eFP_NEW) && (pi.m_nFlags & eFP_CHANGE))
						{
							m_pTransaction->addUndoPage(pRemPage, true);
						}
						pi.m_bOrignSave = true;
					}
				}
				else if(m_pTransaction->getRestoreType() == rtRedo)
				{
					pi.m_bRedoSave = true;
				}
				
			
			}
			//delete pRemPage;
		}
	}
	void CTransactionsCache::savePage(CFilePage *pPage)
	{

		TPages::iterator it = m_pages.find(pPage->getAddr());
		assert(it != m_pages.end());
		sFileTranPageInfo& pi = it->second;
		pi.m_nFlags |= eFP_CHANGE;
		pPage->setFlag(eFP_CHANGE, true);
		if(m_pTransaction->getRestoreType() == rtRedo)
		{
			pi.m_bRedoSave = false;
		}
	//	m_pFileStorage->saveFilePage(pPage, pi->m_nFileAddr);
	}
	void CTransactionsCache::dropChange(IDBStorage *pStorage)
	{
		
	}

	 bool CTransactionsCache::savePageForUndo(IDBTransaction *pTran)
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

				 //bool bRemPage = false;
				 FilePagePtr pPage = m_Chache.GetElem(it->first, true);//pi.m_pPage;
				 if(!pPage.get())
				 {
					 assert(pi.m_nFileAddr != -1);
					 pPage = m_pFileStorage->getFilePage(pi.m_nFileAddr, pi.m_nPageSize);
					 pPage->setFlag(pi.m_nFlags, true);
					 pPage->setAddr(it->first);
					// bRemPage = true;
					// m_Chache.AddElem(it->first, pPage.get());
				 }
				 pTran->addUndoPage(pPage, true);
				// if(bRemPage)
				//	 delete pPage.release();
			 }
		 }
		 return true;
	 }
	 bool CTransactionsCache::savePageForRedo(CTranRedoPageManager *pRepoPageManager)
	 {
		 TPages::iterator it = m_pages.begin();
		 TPages::iterator it_end = m_pages.end();
		 for(;it != it_end; ++it)
		 {
			 sFileTranPageInfo& pi = it->second;

			 bool bNew = (pi.m_nFlags & eFP_NEW) != 0;
			 bool bChange = (pi.m_nFlags & eFP_CHANGE) != 0;
			 bool bRemove = (pi.m_nFlags & eFP_REMOVE) != 0;
			 bool bAdd = false;
			 if(bNew)
			 {
				 if(!bRemove)
					 bAdd = true;
			 }
			 else if(bRemove)
				  bAdd = true;
			 else if(bChange)
				  bAdd = true;

			 if(bAdd)
			 {
				 if(!pi.m_bRedoSave)
				 {
					FilePagePtr pPage = m_Chache.GetElem(it->first, true);
					 if(!pPage.get())
					 {
						 assert(pPage); //должна быть
						 //TO DO Logs
						 return false;
					 }
					 int64 nRet = m_pFileStorage->saveFilePage(pPage.get(), pi.m_nFileAddr);
					 if(pi.m_nFileAddr == -1)
					 {
						 pi.m_nFileAddr = nRet;
						 assert(pi.m_nFileAddr != -1);
					 }
				 }
				 pRepoPageManager->add(it->first, pi.m_nFileAddr, pi.m_nFlags, pi.m_nPageSize);
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
				FilePagePtr pPage = m_Chache.GetElem(it->first, true);
				if(!pPage.get())
				{
					assert(pi.m_nFileAddr != -1);
					pPage = m_pFileStorage->getFilePage(pi.m_nFileAddr, pi.m_nPageSize, true, false);

					assert(pPage.get());
					pPage->setFlag(pi.m_nFlags, true);
					pPage->setAddr(it->first);
					pPage->setNeedEncrypt(pi.m_bNeedCrypt);
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
				//delete pPage.release();
			}
		
		}
		m_Chache.clear();
		m_pages.clear();
	}
	
	void  CTransactionsCache::AddPage(int64 nAddr, int64 nTranAddr, CFilePage* pPage, bool bAddBack)
	{

		m_pages.insert(std::make_pair(nAddr, sFileTranPageInfo(nTranAddr, pPage->getFlags(), pPage->getPageSize(), pPage->isNeedEncrypt())));
		CheckCache();
		m_Chache.AddElem(nAddr, FilePagePtr(pPage));
	}

 
	FilePagePtr  CTransactionsCache::GetPage(int64 nAddr, bool bNotMove, bool bRead, uint32 nSize)
	{
		FilePagePtr pPage = m_Chache.GetElem(nAddr);
		if(pPage.get())
			return pPage;
		TPages::iterator it = m_pages.find(nAddr);
		if(it == m_pages.end())
			return FilePagePtr();

		sFileTranPageInfo& PageInfo = it->second;
		if(PageInfo.m_nFileAddr == -1)
		{
			FilePagePtr pPage (new CFilePage(m_pAlloc, nSize, -1));
			m_Chache.AddElem(nAddr, pPage);
			return pPage;
		}
		pPage = m_pFileStorage->getFilePage(PageInfo.m_nFileAddr, nSize, bRead, false);
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

	uint32 CTransactionsCache::GetPageFlags(int64 nAddr)
	{
		FilePagePtr pPage = m_Chache.GetElem(nAddr);
		if (pPage.get())
			return pPage->getFlags();
		TPages::iterator it = m_pages.find(nAddr);
		if (it == m_pages.end())
			return 0;
		sFileTranPageInfo& PageInfo = it->second;
		return PageInfo.m_nFlags;
	}
} 