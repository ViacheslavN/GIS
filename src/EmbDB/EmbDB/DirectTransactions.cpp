#include "stdafx.h"
#include "Database.h"
#include "DirectTransactions.h"

namespace embDB
{
	CDirectTransaction::CDirectTransaction(CommonLib::alloc_t* pAlloc,  IDBStorage* pDBStorage, uint32 nTranCache) 
		: TBase(NULL)
		,m_bError(false)
		
	{
		assert(m_pDBStorage);
		 m_pDBStorage =pDBStorage;
	}
	CDirectTransaction::CDirectTransaction(CommonLib::alloc_t* pAlloc, eRestoreType nRestoreType,
		eTransactionsType nTranType, const CommonLib::CString& sFileName, IDBStorage* pDBStorage, int64 nID, uint32 nTranCache) : TBase(NULL)
		,m_bError(false)
	{
		assert(m_pDBStorage);
		 m_pDBStorage = pDBStorage;
	}


	CDirectTransaction::CDirectTransaction(CommonLib::alloc_t* pAlloc, eRestoreType nRestoreType,
		eTransactionsType nTranType, const CommonLib::CString& sFileName, CDatabase* pDatabase, int64 nID, uint32 nTranCache) : TBase(pDatabase)
		,m_bError(false)
	{

	}
	CDirectTransaction::~CDirectTransaction()
	{

	}
	FilePagePtr CDirectTransaction::getFilePage(int64 nAddr, bool bRead, uint32 nSize )
	{
		return  m_pDBStorage->getFilePage(nAddr, bRead, nSize);
		/*if(!pFilePage)
			return NULL;
		if(pFilePage->getFlags() & eFP_FROM_FREE_PAGES)
			m_setPagesFromFree.insert(pFilePage->getAddr());
		return pFilePage;*/
	}
	void CDirectTransaction::dropFilePage(FilePagePtr pPage)
	{
		m_setRemovePages.insert(pPage->getAddr());
		//m_pDBStorage->dropFilePage(pPage);
	}
	void CDirectTransaction::dropFilePage(int64 nAddr)
	{

		m_setRemovePages.insert(nAddr);
		//m_pDBStorage->dropFilePage(nAddr);
	}
	FilePagePtr CDirectTransaction::getNewPage(uint32 nSize)
	{
		FilePagePtr pFilePage(m_pDBStorage->getNewPage(false, nSize));
		if(!pFilePage.get())
			return FilePagePtr(NULL);
		if(pFilePage->getFlags() & eFP_FROM_FREE_PAGES)
			m_setPagesFromFree.insert(pFilePage->getAddr());
		return FilePagePtr(pFilePage);
	}
	void CDirectTransaction::saveFilePage(FilePagePtr pPage, size_t nSize,  bool bChandgeInCache)
	{
		m_pDBStorage->saveFilePage(pPage, nSize,  bChandgeInCache);
	}
	size_t CDirectTransaction::getPageSize() const
	{
		return m_pDBStorage->getPageSize();
	}

	bool CDirectTransaction::commit()
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