#include "stdafx.h"
#include "TranUndoPageManager.h"
#include "FilePage.h"
#include "CommonLibrary/FixedMemoryStream.h"

namespace embDB
{
	CTranUndoPageManager::CTranUndoPageManager(IDBTransaction *pTran, CTranStorage *pStorage, bool bCheckCRC) :
		m_pTran(pTran)
		,m_pStorage(pStorage)
		,m_nCurPage(-1)
		,m_nLastPos(0)
		,m_undoPages(-1, 8192, TRANSACTION_PAGE, UNDO_PAGES, bCheckCRC)
	{

	}
	CTranUndoPageManager::~CTranUndoPageManager()
	{

	}
	bool CTranUndoPageManager::add(int64 nDBAddr, int64 nTranAddr, uint32 nFlags, uint32 nPageSize)
	{
		return m_undoPages.push<CTranStorage>(sUndoPageInfo(nDBAddr, nTranAddr, nFlags, nPageSize), m_pStorage);
	}

	bool CTranUndoPageManager::save()
	{
		return m_undoPages.save<CTranStorage>(m_pStorage);
	}
	
	bool  CTranUndoPageManager::undo(CTranStorage *pTranStorage, IDBStorage* pDBStorage)
	{
		TUndoPageList::iterator<CTranStorage> it =  m_undoPages.begin(pTranStorage);
		it.load();
		TUndoDBPages undoPage;
		while(!it.isNull())
		{			
			sUndoPageInfo& pageInfo = it.value();
			FilePagePtr pFilePage = pTranStorage->getFilePage(pageInfo.nTranAddr, pageInfo.nPageSize); // TO DO fix memory leak
			if(!pFilePage.get())
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
