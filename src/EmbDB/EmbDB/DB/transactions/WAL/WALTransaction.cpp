#include "stdafx.h"
#include "../../Database.h"
#include "WALTransaction.h"

namespace embDB
{

	CWALTransaction::CWALTransaction(CommonLib::alloc_t* pAlloc, eRestoreType nRestoreType,
		eTransactionDataType nTranType, const CommonLib::CString& sFileName,
		IDBStorage* pDBStorage, int64 nID, uint32 nTranCache, CPageCipher *pPageCiher) : TBase(NULL, pAlloc), m_pDBStorage(pDBStorage)
	{

	}

	CWALTransaction::CWALTransaction(CommonLib::alloc_t* pAlloc, eRestoreType nRestoreType,
		eTransactionDataType nTranType, const CommonLib::CString& sFileName, IDBConnection* pConnection, int64 nID , uint32 nTranCache, CPageCipher *pPageCiher ) : TBase(NULL, pAlloc)
	{

	}
	CWALTransaction::~CWALTransaction()
	{

	}
	FilePagePtr CWALTransaction::getFilePage(int64 nAddr, uint32 nSize, bool bRead, bool bNeedDecrypt)
	{
		return  m_pDBStorage->getFilePage(nAddr, nSize, bRead, bNeedDecrypt);
 
	}
	void CWALTransaction::dropFilePage(FilePagePtr pPage)
	{
	 
	}
	void CWALTransaction::dropFilePage(int64 nAddr, uint32 nSize)
	{
 
	}
	FilePagePtr CWALTransaction::getNewPage(uint32 nSize, bool bWrite)
	{
		FilePagePtr pFilePage(m_pDBStorage->getNewPage(nSize, bWrite));
		if (!pFilePage.get())
			return FilePagePtr(NULL);
	 
		return FilePagePtr(pFilePage);
	}
	bool CWALTransaction::saveFilePage(FilePagePtr pPage, uint32 nSize, bool bChandgeInCache)
	{
		return m_pDBStorage->saveFilePage(pPage, nSize, bChandgeInCache);
	}
	bool CWALTransaction::saveFilePage(CFilePage* pPage, uint32 nDataSize, bool ChandgeInCache)
	{
		return m_pDBStorage->saveFilePage(pPage, nDataSize, ChandgeInCache);
	}
	/*	uint32 CDirectTransaction::getPageSize() const
	{
	return m_pDBStorage->getPageSize();
	}*/

	bool CWALTransaction::commit()
	{
 
		return true;
	}
}