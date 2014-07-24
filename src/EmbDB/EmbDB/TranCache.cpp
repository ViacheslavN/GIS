#include "stdafx.h"
#include "TranCache.h"

namespace embDB
{

	 void TSimpleTranCache::CheckCache()
	 {
		 if(m_Chache.size() > m_nMaxPageBuf)
		 {
			 CFilePage* pRemPage = m_Chache.remove_back();
			 assert(pRemPage);
			 TPages::iterator it = m_pages.find(pRemPage->getAddr());
			 assert(!it.isNull());
			 sFilePageInfo& pi  = it.value(); 
			 assert(pi.m_pPage == pRemPage);
			 pi.m_pPage = NULL;
			 pi.m_nFlags = pRemPage->getFlags();
			 pi.m_nFileAddr = m_pFileStorage->saveFilePage(pRemPage, pi.m_nFileAddr);
			 delete pRemPage;
		 }
	 }
	  void TSimpleTranCache::savePage(CFilePage *Page)
	  {
		  TPages::iterator it = m_pages.find(Page->getAddr());
		  assert(!it.isNull());
		  sFilePageInfo& pi  = it.value();
		  assert(pi.m_pPage == Page);
		  pi.m_nFlags = Page->getFlags();
		  pi.m_nFileAddr = m_pFileStorage->saveFilePage(Page, pi.m_nFileAddr);
	  }
	 void TSimpleTranCache::dropChange(IDBStorage *pStorage)
	 {
		 TPages::iterator it =  m_pages.begin();
		 while(!it.isNull())
		 {
			 sFilePageInfo& pi = it.value();
			 CFilePage *pPage = pi.m_pPage;
			 if(!pPage)
			 {
				 assert(pi.m_nFileAddr != -1);
				 pPage = m_pFileStorage->getFilePage(pi.m_nFileAddr);
				 pPage->setFlag(pi.m_nFlags, true);
			 }
			 bool bNew = (pPage->getFlags() & eFP_NEW) != 0;
			 bool bChange = (pPage->getFlags() & eFP_CHANGE) != 0;
			 if(bNew || bChange)
			 {
				
				 if(bNew)
					 pStorage->dropFilePage(pPage);
				 delete pPage;
			 }
			 ++it;
		 }
	 }
	 void TSimpleTranCache::saveChange(IDBStorage *pStorage)
	 {
		 TPages::iterator it =  m_pages.begin();
		 pStorage->lockWrite();
		 while(!it.isNull())
		 {
			 sFilePageInfo& pi = it.value();
			 CFilePage *pPage = pi.m_pPage;
			 if(!pPage)
			 {
				 assert(pi.m_nFileAddr != -1);
				 pPage = m_pFileStorage->getFilePage(pi.m_nFileAddr);
				 pPage->setFlag(pi.m_nFlags, true);
			 }
			 bool bNew = (pPage->getFlags() & eFP_NEW) != 0;
			 bool bChange = (pPage->getFlags() & eFP_CHANGE) != 0;
			 if(bNew || bChange)
			 {
				 if(bNew)
					 pStorage->saveNewPage(pPage);
				 else
					pStorage->saveFilePage(pPage, true);
				
			 }
			  delete pPage;
			 ++it;
		 }
		  pStorage->unlockWrite();
		 m_Chache.clear();
		 m_Chache.m_set.destroyTree();
		 m_pages.destroyTree();
	 }
}