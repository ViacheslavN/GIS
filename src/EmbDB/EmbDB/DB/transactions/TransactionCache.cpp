#include "stdafx.h"
#include "TransactionCache.h"
#include "Transactions.h"

#include "Utils/streams/WriteStreamPage.h"
#include "Utils/streams/ReadStreamPage.h"

namespace embDB
{

	void CTransactionCache::CheckCache()
	{
		if(m_Chache.size() > m_nMaxPageBuf)
		{
			FilePagePtr pRemPage = m_Chache.remove_back();
			assert(pRemPage.get());
			TPages::iterator it = m_pages.find(pRemPage->getAddr());
			assert(it != m_pages.end());
			sFileTranPageInfo& pi = it->second;
			if(pi.m_nFlags & (eFP_CHANGE | eFP_NEW | eFP_CHANGE_FROM_IN_LOG))
			{
				int64 nRet = m_pFileStorage->saveFilePageWithRetAddr(pRemPage.get(), pi.m_nFileAddr);
				pi.m_nFlags &= ~eFP_CHANGE_FROM_IN_LOG;

				if(pi.m_nFileAddr == -1)
				{
					pi.m_nFileAddr = nRet;
					assert(pi.m_nFileAddr != -1);
				}

				/*if(m_pTransaction->getRestoreType() == rtUndo)
				{
					if(!pi.m_bOrignSave)
					{
						if( !(pi.m_nFlags & eFP_NEW) && (pi.m_nFlags & eFP_CHANGE))
						{
							m_pTransaction->addUndoPage(pRemPage->getAddr(), pRemPage->getPageSize());
						}
						pi.m_bOrignSave = true;
					}
				}
				else if(m_pTransaction->getRestoreType() == rtRedo)
				{
					pi.m_bRedoSave = true;
				}*/
				
			
			}
			//delete pRemPage;
		}
	}
	void CTransactionCache::savePage(CFilePage *pPage)
	{
		TPages::iterator it = m_pages.find(pPage->getAddr());
		assert(it != m_pages.end());
		sFileTranPageInfo& pi = it->second;
		pi.m_nFlags |= (eFP_CHANGE | eFP_CHANGE_FROM_IN_LOG);
		pPage->setFlag(eFP_CHANGE, true);
		 
	}
	
	 bool CTransactionCache::savePageForUndo(CTransaction *pTran, IDBStorage *pStorage, int64 nRootPage)
	 {
		 
		 auto  it = m_pages.begin();
		 auto it_end = m_pages.end();
		 uint32 nCount = 0;
		 for(;it != it_end; ++it)
		 {
			 auto& pi = it->second;

			 bool bNew = (pi.m_nFlags & eFP_NEW) != 0;
			 bool bChange = (pi.m_nFlags & eFP_CHANGE) != 0;
		
			 if(!bNew && bChange)
			 {			
				 nCount += 1;
				 assert(pi.m_nSaveOrignAddr == -1);
				 FilePagePtr pStoragePage = pStorage->getFilePage(it->first, 0/*pi.m_nPageSize*/, true);
				 pi.m_nSaveOrignAddr = m_pFileStorage->saveFilePageWithRetAddr(pStoragePage.get(), -1);
 			 }
			
		 }

	 
		 TWriteStreamPage<CTranStorage> writeStream(m_pFileStorage, PAGE_SIZE_65K, false);
		 if (!writeStream.open(nRootPage, 0, false))
			 return false;

		 writeStream.write(nCount);

		 it = m_pages.begin();
		 for (; it != it_end; ++it)
		 {
			 auto& pi = it->second;
			 writeStream.write(pi.m_nSaveOrignAddr);
			// writeStream.write(pi.m_nPageSize);
			 writeStream.write(it->first);
		 }
		 writeStream.Save();
	 
		 return true;
	 }
	 bool CTransactionCache::savePageForRedo(CTransaction *pTran,  int64 nRootPage)
	 {
		 TPages::iterator it = m_pages.begin();
		 TPages::iterator it_end = m_pages.end();

		 uint32 nCount = 0;
		 for(;it != it_end; ++it)
		 {
			 sFileTranPageInfo& pi = it->second;

			 bool bNew = (pi.m_nFlags & eFP_NEW) != 0;
			 bool bChange = (pi.m_nFlags & eFP_CHANGE) != 0;

		 	 if(bNew || bChange)
			 {
				 nCount += 1;
				 if (pi.m_nFlags & eFP_CHANGE_FROM_IN_LOG)
				 {
					 FilePagePtr pPage = m_Chache.GetElem(it->first, true);
					 if (!pPage.get())
					 {
						 assert(pPage); 	//TO DO Logs must be
						 return false;
					 }
					 int64 nRet = m_pFileStorage->saveFilePageWithRetAddr(pPage.get(), pi.m_nFileAddr);
					 pi.m_nFlags &= ~eFP_CHANGE_FROM_IN_LOG;
					 if (pi.m_nFileAddr == -1)
					 {
						 pi.m_nFileAddr = nRet;
						 assert(pi.m_nFileAddr != -1);
					 }
				 }
			 }
				 
		 }
		 return true;
	 }
	void CTransactionCache::saveChange(IDBStorage *pStorage)
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
					pPage = m_pFileStorage->getFilePage(pi.m_nFileAddr, 0/*pi.m_nPageSize*/, true, false);

					assert(pPage.get());
					pPage->setFlag(pi.m_nFlags, true);
					pPage->setAddr(it->first);
					//pPage->setNeedEncrypt(pi.m_bNeedCrypt);
				}

				bool bNew = (pPage->getFlags() & eFP_NEW) != 0;
				bool bChange = (pPage->getFlags() & eFP_CHANGE) != 0;
				if(bNew || bChange)
				{
					pStorage->saveFilePage(pPage, true);
					if(bNew)
						m_pCounter->WriteNewDBPage();
					else
						m_pCounter->WriteDBPage();

				}
			
			}
		
		}
		m_Chache.clear();
		m_pages.clear();
	}
	
	void  CTransactionCache::AddPage(int64 nAddr, int64 nTranAddr, CFilePage* pPage, bool bAddBack)
	{

		m_pages.insert(std::make_pair(nAddr, sFileTranPageInfo(nTranAddr, pPage->getFlags()/*, pPage->getPageSize(), pPage->isNeedEncrypt()*/)));
		CheckCache();
		m_Chache.AddElem(nAddr, FilePagePtr(pPage));
	}

 
	FilePagePtr  CTransactionCache::GetPage(int64 nAddr, bool bNotMove, bool bRead, uint32 nSize)
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
	void CTransactionCache::clear()
	{
		m_Chache.clear();
		m_pages.clear();
	}

	uint32 CTransactionCache::GetPageFlags(int64 nAddr)
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

	bool CTransactionCache::undo(IDBStorage* pDBStorage, int64 nRootPage)
	{
		TReadStreamPage<CTranStorage> readStream(m_pFileStorage, PAGE_SIZE_65K, false);
		if (!readStream.open(nRootPage, 0))
			return false;

		int64 nDBAddr = -1, nTranAddr = -1;
		uint32 nPageSize = 0;
		uint32 nSize = readStream.readIntu32();

		for (size_t i = 0; i < nSize; ++i)
		{

			nDBAddr = readStream.readInt64();
			//nSize = readStream.readIntu32();
			nTranAddr = readStream.readInt64();


			FilePagePtr pFilePage = m_pFileStorage->getFilePage(nTranAddr, true);
			if (!pFilePage.get())
			{
				m_pTransaction->error(L"TRAN: Can't get page from Tran");
				return false;
			}
			pFilePage->setAddr(nDBAddr);
			pDBStorage->saveFilePage(pFilePage);

		}

		return true;
	}
} 