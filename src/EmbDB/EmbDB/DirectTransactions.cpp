#include "stdafx.h"
#include "Database.h"
#include "DirectTransactions.h"

namespace embDB
{
	CDirectTransaction::CDirectTransaction(CommonLib::alloc_t* pAlloc,  IDBStorage* pDBStorage, uint32 nTranCache,
		eDBTransationType tranType) 
		: TBase(NULL, pAlloc), m_TranType(tranType)
		, m_TranStorage(pAlloc, NULL, pDBStorage->getCheckCRC())
 
		
	{
		assert(m_pDBStorage);
		 m_pDBStorage =pDBStorage;
	}
	CDirectTransaction::CDirectTransaction(CommonLib::alloc_t* pAlloc, eRestoreType nRestoreType,
		eTransactionDataType nTranType, const CommonLib::CString& sFileName, IDBStorage* pDBStorage, int64 nID, uint32 nTranCache,
		eDBTransationType tranType) : TBase(NULL, pAlloc), m_TranType(tranType)
		, m_TranStorage(pAlloc, NULL, pDBStorage->getCheckCRC())
 
	{
		assert(m_pDBStorage);
		 m_pDBStorage = pDBStorage;
	}


	CDirectTransaction::CDirectTransaction(CommonLib::alloc_t* pAlloc, eRestoreType nRestoreType,
		eTransactionDataType nTranType, const CommonLib::CString& sFileName, IDBConnection* pConnection, int64 nID, uint32 nTranCache,
		eDBTransationType tranType) : TBase(pConnection, pAlloc), m_TranType(tranType)
		, m_TranStorage(pAlloc, NULL, pConnection->getCheckCRC())
 
	{

	}
	CDirectTransaction::~CDirectTransaction()
	{

	}
	FilePagePtr CDirectTransaction::getFilePage(int64 nAddr, uint32 nSize , bool bRead, bool bNeedDecrypt)
	{
		return  m_pDBStorage->getFilePage(nAddr, nSize, bRead, bNeedDecrypt);
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
	void CDirectTransaction::dropFilePage(int64 nAddr, uint32 nSize)
	{

		m_setRemovePages.insert(nAddr);
		//m_pDBStorage->dropFilePage(nAddr);
	}
	FilePagePtr CDirectTransaction::getNewPage(uint32 nSize, bool bWrite)
	{
		FilePagePtr pFilePage(m_pDBStorage->getNewPage( nSize, bWrite));
		if(!pFilePage.get())
			return FilePagePtr(NULL);
		if(pFilePage->getFlags() & eFP_FROM_FREE_PAGES)
			m_setPagesFromFree.insert(pFilePage->getAddr());
		return FilePagePtr(pFilePage);
	}
	bool CDirectTransaction::saveFilePage(FilePagePtr pPage, uint32 nSize,  bool bChandgeInCache)
	{
		return m_pDBStorage->saveFilePage(pPage, nSize,  bChandgeInCache);
	}
	bool CDirectTransaction::saveFilePage(CFilePage* pPage, uint32 nDataSize,  bool ChandgeInCache)
	{
		return m_pDBStorage->saveFilePage(pPage, nDataSize,  ChandgeInCache);
	}
/*	uint32 CDirectTransaction::getPageSize() const
	{
		return m_pDBStorage->getPageSize();
	}*/

	bool CDirectTransaction::commit()
	{
		CommitTemp();

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