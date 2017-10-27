#include "stdafx.h"
#include "../../Database.h"
#include "WALTransaction.h"

namespace embDB
{

	CWALTransaction::CWALTransaction(CommonLib::alloc_t* pAlloc, eRestoreType nRestoreType,
		eTransactionDataType nTranType, IDBWALStorage* pDBStorage,  uint32 nTranCache, CPageCipher *pPageCiher, bool bMultiThread) :
		TBase(NULL, pAlloc), m_pWALStorage(pDBStorage), m_Cache(pAlloc, FilePagePtr()), m_bMultiThread(bMultiThread),
		m_nTranCache(nTranCache), m_bOneSize(true), m_nPageSize(0), m_nTranType(nTranType)
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

		int64 nRealAddr = -1;
		auto it = m_ConvertAddr.find(nAddr);
		if (it != m_ConvertAddr.end())
		{
			//TO DO get rid of copy
			FilePagePtr pStoragePage = m_pWALStorage->getTranLogPage(it->second, nSize, bRead, bNeedDecrypt);
			pPage = new CFilePage(m_pAlloc, pStoragePage->getRowData(), pStoragePage->getPageSize(), nAddr);
			pPage->setFlag(pStoragePage->getFlags(), true);
		}
		else
		{
			FilePagePtr pStoragePage = m_pWALStorage->getFilePage(nAddr, nSize, bRead, bNeedDecrypt);
			pPage = new CFilePage(m_pAlloc, pStoragePage->getRowData(), pStoragePage->getPageSize(), nAddr);
			pPage->setFlag(pStoragePage->getFlags(), true);
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
		auto it = m_TranPages.find(pPage->getRealAddr());
		if (it == m_TranPages.end())
		{
			m_TranPages.insert(std::make_pair(pPage->getRealAddr(), STranPage(pPage->getAddr(), pPage->getPageSize())));
			if (!m_nPageSize && m_nPageSize != pPage->getPageSize())
				m_bOneSize = false;
			m_nPageSize = pPage->getPageSize();
		}
#ifdef _DEBUG
		else
		{
			assert(it->second.m_nDBAddr == pPage->getAddr() && it->second.m_nSize == pPage->getPageSize());
		}
#endif
		return true;

	}

	bool CWALTransaction::commit()
	{
		auto  it = m_Cache.begin();
		while (!it.isNull())
		{
			auto pPage = it.object();
			if (pPage->getFlags() & (eFP_CHANGE | eFP_NEW))
			{
				saveFilePage(pPage);
				m_pWALStorage->saveFilePage(pPage);
			}
			it.next();
		}
 
		//Begin save tran info
		if (GetRowPageInfoSize() < PAGE_SIZE_65K)
		{

		}


		
		return true;
	}

	void CWALTransaction::setWALStorage(IDBWALStorage *pStorage)
	{
		m_pWALStorage = pStorage;
		 
	}

	uint32 CWALTransaction::GetRowPageInfoSize() const
	{
		return (2 * sizeof(int64) + sizeof(uint32)) * m_TranPages.size() + sizeof(uint32);
	}
	void CWALTransaction::SaveFilePageInTranLog(CFilePage *pPage)
	{
		if (!(pPage->getFlags() & eFP_FROM_LOG_TRAN))
		{
			m_pWALStorage->saveFilePage(pPage);
			return;
		}
		int64 nAddr = m_pWALStorage->getNewTranLogPageAddr(pPage->getPageSize());

		/*CFilePage *pNewPage = new CFilePage(m_pAlloc, pPage->getPageSize(), nAddr);
		pNewPage->setFlag(eFP_NEW_TRAN_LOG, true);
		memcpy(pNewPage->getRowData(), pPage->getRowData(), pNewPage->getPageSize());
		pNewPage*/

		m_ConvertAddr.insert(std::make_pair(pPage->getAddr(), nAddr));
		pPage->setRealAddr(nAddr);
		m_pWALStorage->saveFilePage(pPage);

	}
}