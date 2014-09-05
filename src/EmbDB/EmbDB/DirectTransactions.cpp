#include "stdafx.h"
#include "DirectTransactions.h"
namespace embDB
{
	CDirectTransactions::CDirectTransactions(CommonLib::alloc_t* pAlloc,  IDBStorage* pDBStorage) 
		: m_pDBStorage(pDBStorage)
		,m_bError(false)
	{
		assert(m_pDBStorage);
	}
	CDirectTransactions::CDirectTransactions(CommonLib::alloc_t* pAlloc, eRestoreType nRestoreType,
		eTransactionsType nTranType, const CommonLib::str_t& sFileName, IDBStorage* pDBStorage, int64 nID): m_pDBStorage(pDBStorage)
		,m_bError(false)
	{
		assert(m_pDBStorage);
	}
	CDirectTransactions::~CDirectTransactions()
	{

	}
	CFilePage* CDirectTransactions::getFilePage(int64 nAddr, bool bRead )
	{
		return  m_pDBStorage->getFilePage(nAddr, bRead);
		/*if(!pFilePage)
			return NULL;
		if(pFilePage->getFlags() & eFP_FROM_FREE_PAGES)
			m_setPagesFromFree.insert(pFilePage->getAddr());
		return pFilePage;*/
	}
	void CDirectTransactions::dropFilePage(CFilePage* pPage)
	{
		m_setRemovePages.insert(pPage->getAddr());
		//m_pDBStorage->dropFilePage(pPage);
	}
	void CDirectTransactions::dropFilePage(int64 nAddr)
	{

		m_setRemovePages.insert(nAddr);
		//m_pDBStorage->dropFilePage(nAddr);
	}
	CFilePage* CDirectTransactions::getNewPage()
	{
		CFilePage *pFilePage = m_pDBStorage->getNewPage();
		if(!pFilePage)
			return NULL;
		if(pFilePage->getFlags() & eFP_FROM_FREE_PAGES)
			m_setPagesFromFree.insert(pFilePage->getAddr());
		return pFilePage;
	}
	void CDirectTransactions::saveFilePage(CFilePage* pPage, size_t nSize,  bool bChandgeInCache)
	{
		m_pDBStorage->saveFilePage(pPage, nSize,  bChandgeInCache);
	}
	size_t CDirectTransactions::getPageSize() const
	{
		return m_pDBStorage->getPageSize();
	}

	bool CDirectTransactions::commit()
	{
		m_pDBStorage->lockWrite(this);

		{
			std::set<int64>::iterator it = m_setPagesFromFree.begin();
			std::set<int64>::iterator end = m_setPagesFromFree.end();
			for (;it != end; ++it)
			{
				m_pDBStorage->removeFromFreePage((*it));
			}

		}

		{
			std::set<int64>::iterator it = m_setRemovePages.begin();
			std::set<int64>::iterator end = m_setRemovePages.end();
			for (;it != end; ++it)
			{
				m_pDBStorage->dropFilePage((*it));
				 
			}

		}

		m_pDBStorage->commit();

		m_pDBStorage->unlockWrite(this);
		return true;
	}
}