#include "stdafx.h"
#include "TranRedoPageManager.h"
#include "stdafx.h"
#include "FilePage.h"
#include "CommonLibrary/FixedMemoryStream.h"

namespace embDB
{
	CTranRedoPageManager::CTranRedoPageManager(IDBTransactions *pTran, CTranStorage *pStorage) :
	m_pTran(pTran)
	,m_pStorage(pStorage)
	,m_nCurPage(-1)
	,m_nLastPos(0)
	,m_undoPages(-1, pTran->getPageSize(), TRANSACTION_PAGE, UNDO_PAGES)
{

}
CTranRedoPageManager::~CTranRedoPageManager()
{

}
bool CTranRedoPageManager::add(const sUndoPageInfo& PageInfo)
{
	return m_undoPages.push<CTranStorage, CFilePage*>(PageInfo, m_pStorage);
}

bool CTranRedoPageManager::save()
{
	return m_undoPages.save<CTranStorage, CFilePage*>(m_pStorage);
}

bool  CTranRedoPageManager::redo(CTranStorage *pTranStorage, IDBStorage* pDBStorage)
{
	TUndoPageList::iterator<CTranStorage> it =  m_undoPages.begin(pTranStorage);
	it.load();
	TUndoDBPages undoPage;
	while(!it.isNull())
	{			
		sUndoPageInfo& pageInfo = it.value();
		CFilePage *pFilePage = pTranStorage->getFilePage(pageInfo.nTranAddr);
		if(!pFilePage)
		{
			m_pTran->error("TRAN: Can't get page from Tran");
			return false;
		}
		pFilePage->setAddr(pageInfo.nDBAddr);
		pDBStorage->saveFilePage(pFilePage);
		it.next();
	}

	return true;
}
}

