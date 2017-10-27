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
			
			if(pRemPage->getFlags() & (eFP_CHANGE | eFP_NEW | eFP_CHANGE_FROM_IN_LOG))
			{
				TPages::iterator it = m_pages.find(pRemPage->getAddr());
				assert(it != m_pages.end());
				sFileTranPageInfo& pi = it->second;
				pi.m_nFlags = pRemPage->getFlags();

				int64 nRet = m_pFileStorage->saveFilePageWithRetAddr(pRemPage.get(), pi.m_nFileAddr);
				pi.m_nFlags &= ~eFP_CHANGE_FROM_IN_LOG;

				if(pi.m_nFileAddr == -1)
				{
					pi.m_nFileAddr = nRet;
					assert(pi.m_nFileAddr != -1);
				}
	
			}

		}
	}
	void CTransactionCache::savePage(CFilePage *pPage)
	{
		assert((pPage->getFlags() & (eFP_COPY_FROM_STORAGE | eFP_NEW)) != 0);
		pPage->setFlag(eFP_CHANGE | eFP_CHANGE_FROM_IN_LOG, true);
	}
	
	 bool CTransactionCache::savePageForUndo(CTransaction *pTran, IDBStorage *pStorage, int64 nRootPage)
	 {
		 
		 auto  it = m_pages.begin();
		 auto it_end = m_pages.end();
		 uint32 nCount = 0;
		 for(;it != it_end; ++it)
		 {
			 auto& pi = it->second;
			 FilePagePtr pPage = m_Chache.GetElem(it->first, true);

			 bool bNew = false;
			 bool bChange = false;

			 if (pPage.get())
			 {
				 bNew = (pPage->getFlags() & eFP_NEW) != 0;
				 bChange = (pPage->getFlags() & eFP_CHANGE) != 0;

				 pi.m_nFlags = pPage->getFlags();
			 }
			 else
			 {
				 bNew = (pi.m_nFlags & eFP_NEW) != 0;
				 bChange = (pi.m_nFlags & eFP_CHANGE) != 0;

				
			 }
		
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
			 bool bNew = (pi.m_nFlags & eFP_NEW) != 0;
			 bool bChange = (pi.m_nFlags & eFP_CHANGE) != 0;

			 if (!bNew && bChange)
			 {
				 writeStream.write(pi.m_nSaveOrignAddr);
				 writeStream.write(it->first);
			 }

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
			 FilePagePtr pPage = m_Chache.GetElem(it->first, true);

			 bool bNew = false;
			 bool bChange = false;

			 if (pPage.get())
			 {
				 bNew = (pPage->getFlags() & eFP_NEW) != 0;
				 bChange = (pPage->getFlags() & eFP_CHANGE) != 0;

				 pi.m_nFlags = pPage->getFlags();
			 }
			 else
			 {
				 bNew = (pi.m_nFlags & eFP_NEW) != 0;
				 bChange = (pi.m_nFlags & eFP_CHANGE) != 0;
			 }

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


		 TWriteStreamPage<CTranStorage> writeStream(m_pFileStorage, PAGE_SIZE_65K, false);
		 if (!writeStream.open(nRootPage, 0, false))
			 return false;

		 writeStream.write(nCount);

		 it = m_pages.begin();
		 for (; it != it_end; ++it)
		 {
			 auto& pi = it->second;

			 bool bNew = (pi.m_nFlags & eFP_NEW) != 0;
			 bool bChange = (pi.m_nFlags & eFP_CHANGE) != 0;
			 if (bNew || bChange)
			 {
				 writeStream.write(it->first);
				 writeStream.write(pi.m_nFileAddr);
				
			 }

		 }
		 writeStream.Save();


		 return true;
	 }
	void CTransactionCache::saveChange(IDBStorage *pStorage)
	{

		TPages::iterator it = m_pages.begin();
		TPages::iterator it_end = m_pages.end();
 
		for(;it != it_end; ++it)
		{
 
			FilePagePtr pPage = m_Chache.GetElem(it->first, true);

			sFileTranPageInfo& pi = it->second;

			bool bNew = (pi.m_nFlags & eFP_NEW) != 0;
			bool bChange = (pi.m_nFlags & eFP_CHANGE) != 0;
			
						
			if(bNew || bChange)
			{
				if(!pPage.get())
				{
					assert(pi.m_nFileAddr != -1);
					pPage = m_pFileStorage->getFilePage(pi.m_nFileAddr, 0/*pi.m_nPageSize*/, true, false);

					assert(pPage.get());
					pPage->setFlag(pi.m_nFlags, true);
					pPage->setAddr(it->first);
					//pPage->setNeedEncrypt(pi.m_bNeedCrypt);
				}
				pStorage->saveFilePage(pPage, true);
				/*
				bool bNew = (pPage->getFlags() & eFP_NEW) != 0;
				bool bChange = (pPage->getFlags() & eFP_CHANGE) != 0;
				if(bNew || bChange)
				{
					pStorage->saveFilePage(pPage, true);
					if(bNew)
						m_pCounter->WriteNewDBPage();
					else
						m_pCounter->WriteDBPage();

				}*/
			
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

	CTransactionCache::sFileTranPageInfo *CTransactionCache::GetPageInfo(int64 nAddr)
	{
		TPages::iterator it = m_pages.find(nAddr);
		if (it != m_pages.end())
			return &it->second;
		return nullptr;
	}

	FilePagePtr CTransactionCache::GetPageFromCache(int64 nAddr, bool bNotMove)
	{
		return m_Chache.GetElem(nAddr, bNotMove);
	}
	FilePagePtr  CTransactionCache::ReadPage(int64 nAddr, sFileTranPageInfo* pPageInfo, uint32 nSize, bool bRead , bool bNeedDecrypt )
	{
		/*TPages::iterator it = m_pages.find(nAddr);
		if (it == m_pages.end())
		{
			return FilePagePtr();
		}*/

		//sFileTranPageInfo& PageInfo = it->second;
		assert(pPageInfo->m_nFileAddr != -1);
	
		FilePagePtr pPage = m_pFileStorage->getFilePage(pPageInfo->m_nFileAddr, nSize, bRead, bNeedDecrypt);
		assert(pPage);
		pPage->setAddr(nAddr);
		pPage->setFlag(pPageInfo->m_nFlags, true);
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

	bool CTransactionCache::redo(IDBStorage* pDBStorage, int64 nRootPage)
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