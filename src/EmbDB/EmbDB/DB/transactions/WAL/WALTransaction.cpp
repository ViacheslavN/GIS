#include "stdafx.h"
#include "../../Database.h"
#include "WALTransaction.h"

#include "Utils/streams/WriteStreamPage.h"
#include "Utils/streams/ReadStreamPage.h"

namespace embDB
{

	CWALTransaction::CWALTransaction(CommonLib::alloc_t* pAlloc, eRestoreType nRestoreType,
		eTransactionDataType nTranType, IDBWALStorage* pDBStorage,  uint32 nTranCache, CPageCipher *pPageCiher, bool bMultiThread) :
		TBase(NULL, pAlloc), m_pWALStorage(pDBStorage), m_Cache(pAlloc, FilePagePtr()), m_bMultiThread(bMultiThread),
		m_nTranCache(nTranCache),  m_nTranType(nTranType)
	{

	}

	CWALTransaction::CWALTransaction(CommonLib::alloc_t* pAlloc, eRestoreType nRestoreType,
		eTransactionDataType nTranType,  IDBConnection* pConnection,  uint32 nTranCache, CPageCipher *pPageCiher, bool bMultiThread ) :
		TBase(NULL, pAlloc), m_Cache(pAlloc, FilePagePtr()), m_bMultiThread(bMultiThread),
		m_nTranCache(nTranCache), m_nTranType(nTranType)
	{

	}
	CWALTransaction::~CWALTransaction()
	{

	}
	FilePagePtr CWALTransaction::getFilePage(int64 nAddr, uint32 nSize, bool bRead, bool bNeedDecrypt, bool bAddInCache, bool bForChange)
	{
		if(!m_bMultiThread || m_nTranType == eTT_SELECT)
			return  m_pWALStorage->getFilePage(nAddr, nSize, bRead, bNeedDecrypt, bAddInCache, bForChange);

		
		
		FilePagePtr pPage = m_Cache.GetElem(nAddr);
		if (pPage.get())
			return pPage;


		
		auto itPage = m_TranPages.find(nAddr);
		if (itPage == m_TranPages.end())
		{
			if(!bForChange)
				return  m_pWALStorage->getFilePage(nAddr, nSize, bRead, bNeedDecrypt, bAddInCache, bForChange);
			else
			{
				FilePagePtr pStoragePage = m_pWALStorage->getFilePage(nAddr, nSize, bRead, bNeedDecrypt);
				pPage = new CFilePage(m_pAlloc, pStoragePage->getRowData(), pStoragePage->getPageSize(), nAddr);
				pPage->setFlag(pStoragePage->getFlags(), true);
			}
		}
		else
		{
			auto& obj = itPage->second;

			if(obj.m_nConverAddr != -1)
				pPage = m_pWALStorage->getTranLogPage(obj.m_nConverAddr, nSize, bRead, bNeedDecrypt, false);
			else
			{
				if (!bForChange)
				{
					pPage = m_pWALStorage->getFilePage(nAddr, nSize, bRead, bNeedDecrypt, bAddInCache, bForChange);
					pPage->setFlag(obj.m_nFlags, true);
					return pPage;
				}
				else
				{
					FilePagePtr pStoragePage = m_pWALStorage->getFilePage(nAddr, nSize, bRead, bNeedDecrypt);
					pPage = new CFilePage(m_pAlloc, pStoragePage->getRowData(), pStoragePage->getPageSize(), nAddr);
				}
			}
			pPage->setFlag(obj.m_nFlags, true);
		}
 
		if (m_Cache.size() > (uint32)m_nTranCache)
		{
			FilePagePtr pRemPage =  m_Cache.remove_back();
			if (!pRemPage.get() && pRemPage->getFlags()  & (eFP_CHANGE| eFP_NEW))
			{
				SaveFilePageInTranLog(pRemPage.get());
			}
		}
		m_Cache.AddElem(pPage->getAddr(), pPage);
 
		return pPage;
	}
	void CWALTransaction::dropFilePage(FilePagePtr pPage)
	{
	 
	}
	void CWALTransaction::dropFilePage(int64 nAddr, uint32 nSize)
	{
 
	}
	FilePagePtr CWALTransaction::getNewPage(uint32 nSize, bool bWrite, bool bAddInCache)
	{
		if (!m_bMultiThread)
			return m_pWALStorage->getNewPage(nSize, bWrite, bAddInCache);

		FilePagePtr pNewPage = m_pWALStorage->getNewPage(nSize, bWrite, false);
		//FilePagePtr pNewPage (new CFilePage(m_pAlloc, pStoragePage->getPageSize(), pStoragePage->getAddr()));
		//pNewPage->setRealAddr(pStoragePage->getRealAddr());

		if (m_Cache.size() > (uint32)m_nTranCache)
		{
			FilePagePtr pRemPage = m_Cache.remove_back();
			if (!pRemPage.get() && pRemPage->getFlags() & (eFP_CHANGE | eFP_NEW))
			{
				SaveFilePageInTranLog(pRemPage.get());
			}
		}
		m_Cache.AddElem(pNewPage->getAddr(), pNewPage);

		return pNewPage;

	}
	bool CWALTransaction::saveFilePage(FilePagePtr pPage, bool bChandgeInCache)
	{	 
		return saveFilePage(pPage.get(), bChandgeInCache);
	}
	bool CWALTransaction::saveFilePage(CFilePage* pPage, bool ChandgeInCache)
	{
		pPage->setFlag(eFP_CHANGE, true);
		pPage->setFlag(eFP_SAVE_IN_TRAN_LOG, false);

		auto it = m_TranPages.find(pPage->getRealAddr());
		if (it == m_TranPages.end())
		{
			m_TranPages.insert(std::make_pair(pPage->getRealAddr(), STranPageInfo(pPage->getAddr(), pPage->getFlags())));
		 
		}
		else
		{
			it->second.m_nFlags = pPage->getFlags();
		}
		return true;

	}

	bool CWALTransaction::commit()
	{
		auto  it = m_Cache.begin();
		while (!it.isNull())
		{
			auto pPage = it.object();
			if (pPage->getFlags() & (eFP_CHANGE | eFP_NEW) && !(pPage->getFlags() &eFP_SAVE_IN_TRAN_LOG))
			{
				saveFilePage(pPage);
				SaveFilePageInTranLog(pPage.get());
			}
			it.next();
		}
 
		int64 nCheckPointAddr =  m_pWALStorage->getNewTranLogPageAddr();

		TWriteStreamPage<IDBStorage> writeStream(m_pWALStorage->GetTranLogStorage().get(), PAGE_SIZE_8K, true);
		if (!writeStream.open(nCheckPointAddr, 0))
			return false;

		writeStream.write((uint32)m_TranPages.size());
		for (auto it = m_TranPages.begin(); it != m_TranPages.end(); ++it)
		{
			auto& nPageInfo = it->second;

			writeStream.write(it->first);
			writeStream.write(nPageInfo.m_nConverAddr != -1 ? nPageInfo.m_nConverAddr : nPageInfo.m_nLogTranAddr);
		}

		writeStream.Save();
 
		m_pWALStorage->lock();
		for (auto it = m_TranPages.begin(); it != m_TranPages.end(); ++it)
		{
			auto& nPageInfo = it->second;
			m_pWALStorage->addPageToCheckPoint(nCheckPointAddr, it->first, nPageInfo.m_nConverAddr != -1 ? nPageInfo.m_nConverAddr : nPageInfo.m_nLogTranAddr);
 
		}



		m_pWALStorage->unlock();
		
		return true;
	}

	void CWALTransaction::setWALStorage(IDBWALStorage *pStorage)
	{
		m_pWALStorage = pStorage;
		 
	}

 
	void CWALTransaction::SaveFilePageInTranLog(CFilePage *pPage)
	{
		auto itPage = m_TranPages.find(pPage->getAddr());
		assert(itPage == m_TranPages.end()); //must be
		auto& PageInto = itPage->second;

		PageInto.m_nFlags |= eFP_SAVE_IN_TRAN_LOG;


		if (!(pPage->getFlags() & eFP_FROM_LOG_TRAN))
		{
			m_pWALStorage->saveFilePage(pPage);
			return;
		}

		if (PageInto.m_nConverAddr == -1)
		{
			PageInto.m_nConverAddr = m_pWALStorage->getNewTranLogPageAddr(pPage->getPageSize());
		}
		pPage->setRealAddr(PageInto.m_nConverAddr); 
		m_pWALStorage->saveFilePage(pPage);

	}
}