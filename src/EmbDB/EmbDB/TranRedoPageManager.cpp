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
	,m_nRootPage(-1)
	,m_nLastPos(0)
	,m_RedoPages(-1, pTran->getPageSize(), TRANSACTION_PAGE, REDO_PAGES)
	,m_UndoPages(-1, pTran->getPageSize(), TRANSACTION_PAGE, REDO_PAGES)
{

}
CTranRedoPageManager::~CTranRedoPageManager()
{

}
bool CTranRedoPageManager::add(int64 nDBAddr, int64 nTranAddr, int32 nFlags)
{
	sRedoPageInfo PageInfo(nDBAddr, nTranAddr, nFlags);
	return m_RedoPages.push<CTranStorage, CFilePage*>(PageInfo, m_pStorage);
}
bool CTranRedoPageManager::add_undo(int64 nDBAddr, int64 nTranAddr, int32 nFlags)
{
	sRedoPageInfo PageInfo(nDBAddr, nTranAddr, nFlags);
	return m_UndoPages.push<CTranStorage, CFilePage*>(PageInfo, m_pStorage);
}
bool CTranRedoPageManager::save()
{
	bool bRet = m_RedoPages.save<CTranStorage, CFilePage*>(m_pStorage);
	if(!bRet)
		return false;
	bRet = m_UndoPages.save<CTranStorage, CFilePage*>(m_pStorage);
	if(!bRet)
		return false;

	CFilePage *pPage = m_pStorage->getFilePage(m_nRootPage);
	if(!pPage)
		return false; //TO DO Logs
	CommonLib::FxMemoryWriteStream stream;	
	stream.attach(pPage->getRowData(), pPage->getPageSize());
	sFilePageHeader header(stream, TRANSACTION_PAGE, REDO_PAGES);
	 stream.write(m_Header.nRedoBlock);
	 stream.write(m_Header.nUndoBlock);
	 header.writeCRC32(stream);
	 m_pStorage->saveFilePage(pPage);
	 delete pPage;
	return true;

}

bool  CTranRedoPageManager::redo(CTranStorage *pTranStorage, IDBStorage* pDBStorage)
{
	{
		TRedoPageList::iterator<CTranStorage> it =  m_UndoPages.begin(pTranStorage);
		it.load();
		while(!it.isNull())
		{			
			sRedoPageInfo& pageInfo = it.value();
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

		pDBStorage->reload();
	}
	{
		TRedoPageList::iterator<CTranStorage> it =  m_RedoPages.begin(pTranStorage);
		it.load();
		while(!it.isNull())
		{			
			sRedoPageInfo& pageInfo = it.value();
			FilePagePtr pPage(pTranStorage->getFilePage(pageInfo.nTranAddr));
			if(!pPage.get())
			{
				m_pTran->error("TRAN: Can't get page from Tran");
				return false;
			}
			bool bNew = (pageInfo.nFlags & eFP_NEW) != 0;
			bool bChange = (pageInfo.nFlags & eFP_CHANGE) != 0;
			bool bRemove = (pageInfo.nFlags & eFP_REMOVE) != 0;
			bool bFromFree = (pageInfo.nFlags & eFP_FROM_FREE_PAGES) != 0;
		

			if(bFromFree)
				pDBStorage->removeFromFreePage(pageInfo.nDBAddr);

			pPage->setAddr(pageInfo.nDBAddr);
			if(bRemove)
			{
				if(!bNew)
					pDBStorage->dropFilePage(pageInfo.nDBAddr);

			}
			else if(bNew)
			{
				if(bFromFree)
					pDBStorage->saveFilePage(pPage, 0,  true);
				else
					pDBStorage->saveNewPage(pPage);
			}
			else if(bChange)
			{			
				pDBStorage->saveFilePage(pPage);
			}		
			it.next();
		}

	}
	
	return true;
}

void CTranRedoPageManager::setFirstPage(int64 nPage, bool bCreate)
{
	m_nRootPage = nPage;
	if(bCreate)
	{
		m_Header.nRedoBlock = m_pStorage->getNewPageAddr();
		m_Header.nUndoBlock = m_pStorage->getNewPageAddr();
	}
	else
	{
		CFilePage *pPage = m_pStorage->getFilePage(m_nRootPage);
		if(!pPage)
			return; //TO DO Logs
		CommonLib::FxMemoryReadStream stream;	
		stream.attach(pPage->getRowData(), pPage->getPageSize());
		sFilePageHeader header(stream);
		if(!header.isValid())
		{
			//TO DO Log
			return;
		}
		if(header.m_nObjectPageType != TRANSACTION_PAGE || header.m_nSubObjectPageType != REDO_PAGES)
		{
			//TO DO Log
			return;
		}
		m_Header.nRedoBlock = stream.readInt64();
		m_Header.nUndoBlock = stream.readInt64();

		 delete pPage;
	}

	m_RedoPages.setRoot(m_Header.nRedoBlock);
	m_UndoPages.setRoot(m_Header.nUndoBlock);
	
}
}

