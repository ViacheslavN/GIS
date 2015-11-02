#include "stdafx.h"
#include "TranUndoPageManager.h"
#include "FilePage.h"
#include "CommonLibrary/FixedMemoryStream.h"

namespace embDB
{
	CTranUndoPageManager::CTranUndoPageManager(IDBTransaction *pTran, CTranStorage *pStorage) :
		m_pTran(pTran)
		,m_pStorage(pStorage)
		,m_nCurPage(-1)
		,m_nLastPos(0)
		,m_undoPages(-1, pTran->getPageSize(), TRANSACTION_PAGE, UNDO_PAGES)
	{

	}
	CTranUndoPageManager::~CTranUndoPageManager()
	{

	}
	bool CTranUndoPageManager::add(int64 nDBAddr, int64 nTranAddr, uint32 nFlags)
	{
		return m_undoPages.push<CTranStorage, CFilePage*>(sUndoPageInfo(nDBAddr, nTranAddr, nFlags), m_pStorage);
	}

	bool CTranUndoPageManager::save()
	{
		return m_undoPages.save<CTranStorage, CFilePage*>(m_pStorage);
	}
	
	bool  CTranUndoPageManager::undo(CTranStorage *pTranStorage, IDBStorage* pDBStorage)
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
				m_pTran->error(L"TRAN: Can't get page from Tran");
				return false;
			}
			pFilePage->setAddr(pageInfo.nDBAddr);
			pFilePage->setNeedEncrypt(false);
			pDBStorage->saveFilePage(pFilePage);
			it.next();
		}
		
		return true;
	}
}
