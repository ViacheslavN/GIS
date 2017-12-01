#include "stdafx.h"
#include "WALStorage.h"

namespace embDB
{

	CWALStorage::CWALStorage(CommonLib::alloc_t *pAlloc, int32 nCacheSize, bool bCheckCRC, bool bMultiThread) 
		: m_bMultiThread(false)
	{

	}

	FilePagePtr CWALStorage::getFilePage(int64 nAddr, uint32 nSize, bool bRead, bool bNeedDecrypt, bool bForChanghe)
	{
		stopCopy();

		// TO DO Lock
		auto it = m_PageAddrs.find(nAddr);
		if (it == m_PageAddrs.end())
			return m_pDBStorage->getFilePage(nAddr, nSize, bRead, bNeedDecrypt);
		else
		{
			auto pPage = m_pDBLogStorage->getFilePage(it->second, nSize, bRead, bNeedDecrypt);
			if (pPage.get())
			{
				pPage->setAddr(nAddr);
				pPage->setRealAddr(it->second);
				pPage->setFlag(eFP_FROM_LOG_TRAN, true);
			}
			
			return pPage;
		}
	}
		

	bool CWALStorage::saveFilePage(CFilePage* pPage,  bool ChandgeInCache )
	{		 
		assert(pPage->getRealAddr() != -1);
		return m_pDBLogStorage->saveFilePage(pPage, ChandgeInCache);
	}

	FilePagePtr CWALStorage::getNewPage(uint32 nSize, bool bWrite )
	{
		int64 nAddr = m_pDBStorage->getNewPageAddr(nSize);
		FilePagePtr pFilePage = m_pDBLogStorage->getNewPage(nSize, bWrite);
		pFilePage->setFlag(eFP_NEW|eFP_NEW_TRAN_LOG, true);

		int64 nRealSize = pFilePage->getAddr();
		pFilePage->setAddr(nAddr);
		pFilePage->setRealAddr(nRealSize);

		return pFilePage;

	}

	FilePagePtr CWALStorage::getNewTranLogPage(uint32 nSize, bool bWrite, bool bAddCache)
	{
		return m_pDBLogStorage->getNewPage(nSize, bWrite, bAddCache);
	}
	FilePagePtr CWALStorage::getTranLogPage(int64 nAddr, uint32 nSize, bool bRead, bool bNeedDecrypt, bool bAddCache)
	{
		return m_pDBLogStorage->getFilePage(nAddr, nSize, bRead, bNeedDecrypt, bAddCache);
	}
	int64 CWALStorage::getNewTranLogPageAddr(uint32 nSize)
	{
		return m_pDBLogStorage->getNewPageAddr(nSize);
	}

	void CWALStorage::stopCopy()
	{

	}

	void CWALStorage::copyPage()
	{
		while (true)
		{
			//TO DO lock

			if (!IsCopy())
				break;

		
			if (m_PageAddrs.empty())
				break;
			bool bNeedSave = false;

			for (auto it = m_PageAddrs.begin(); it != m_PageAddrs.end(); )
			{
				if (!IsCopy())
					break;

				auto pPage = m_pDBLogStorage->getFilePage(it->first, true, false);
				pPage->setAddr(it->second);
				m_pDBStorage->saveFilePage(pPage, it->second);
				it = m_PageAddrs.erase(it);
				bNeedSave = true;
			}
			if(bNeedSave)
				m_pDBStorage->commit();
		}


		
	}

	bool IsCopy()
	{
		return false;
	}

	bool CWALStorage::intit(IDBStorage *pDBStorage, IDBStorage *pTranLogStorage, bool bNew)
	{
		m_pDBStorage = pDBStorage;
		m_pDBLogStorage = pTranLogStorage;
 
		m_pCheckPointStream.reset(new TCheckPointStream(m_pDBLogStorage.get(), PAGE_SIZE_8K, true, 0, 0));

		if (bNew)
		{

		}
		else
		{


		}

		return true;
	}


	bool CWALStorage::commit(IDBWALTransaction *pTran)
	{
		//Lock

		//pTran->

		return true;
	}

	void CWALStorage::SetOffset(uint32 nLogTranOffset, uint32 nDBFileOffset)
	{
		m_pDBLogStorage->SetOffset(nLogTranOffset);
		m_pDBStorage->SetOffset(nDBFileOffset);
	}

	void CWALStorage::UpdateCheckPoint(int64 nAddr)
	{
		m_pCheckPointStream->write(nAddr);
	}
}