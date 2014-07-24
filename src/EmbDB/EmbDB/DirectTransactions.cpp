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
		return m_pDBStorage->getFilePage(nAddr, bRead);
	}
	void CDirectTransactions::dropFilePage(CFilePage* pPage)
	{
		m_pDBStorage->dropFilePage(pPage);
	}
	void CDirectTransactions::dropFilePage(int64 nAddr)
	{
		m_pDBStorage->dropFilePage(nAddr);
	}
	CFilePage* CDirectTransactions::getNewPage()
	{
		return m_pDBStorage->getNewPage();
	}
	void CDirectTransactions::saveFilePage(CFilePage* pPage)
	{
		m_pDBStorage->saveFilePage(pPage);
	}
	size_t CDirectTransactions::getPageSize() const
	{
		return m_pDBStorage->getPageSize();
	}
}