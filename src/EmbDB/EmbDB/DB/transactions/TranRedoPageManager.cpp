#include "stdafx.h"
#include "TranRedoPageManager.h"
#include "stdafx.h"
#include "storage/FilePage.h"
#include "CommonLibrary/FixedMemoryStream.h"

namespace embDB
{
	CTranRedoPageManager::CTranRedoPageManager(IDBTransaction *pTran, CTranStorage *pStorage, bool bCheckCRC) :
	m_pTran(pTran)
	,m_pStorage(pStorage)
	,m_nRootPage(-1)
	,m_nLastPos(0)
	,m_bCheckCRC(bCheckCRC)
	,m_RedoPages(-1, PAGE_SIZE_8K, TRANSACTION_PAGE, REDO_PAGES, bCheckCRC)
	,m_UndoPages(-1, PAGE_SIZE_8K, TRANSACTION_PAGE, REDO_PAGES, bCheckCRC)
{

}
CTranRedoPageManager::~CTranRedoPageManager()
{

}
bool CTranRedoPageManager::add(int64 nDBAddr, int64 nTranAddr, uint32 nFlags, uint32 nPageSize)
{
	sRedoPageInfo PageInfo(nDBAddr, nTranAddr, nFlags, nPageSize);
	return m_RedoPages.push<CTranStorage>(PageInfo, m_pStorage);
}
bool CTranRedoPageManager::add_undo(int64 nDBAddr, int64 nTranAddr, uint32 nFlags, uint32 nPageSize)
{
	sRedoPageInfo PageInfo(nDBAddr, nTranAddr, nFlags, nPageSize);
	return m_UndoPages.push<CTranStorage>(PageInfo, m_pStorage);
}
bool CTranRedoPageManager::save()
{
	bool bRet = m_RedoPages.save<CTranStorage>(m_pStorage);
	if(!bRet)
		return false;
	bRet = m_UndoPages.save<CTranStorage>(m_pStorage);
	if(!bRet)
		return false;

	FilePagePtr pPage = m_pStorage->getFilePage(m_nRootPage, PAGE_SIZE_8K);
	if(!pPage.get())
		return false; //TO DO Logs
	CommonLib::FxMemoryWriteStream stream;	
	stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());
	sFilePageHeader header(stream, TRANSACTION_PAGE, REDO_PAGES, pPage->getPageSize(), m_bCheckCRC);
	 stream.write(m_Header.nRedoBlock);
	 stream.write(m_Header.nUndoBlock);
	 header.writeCRC32(stream);
	 m_pStorage->saveFilePage(pPage.get());
	 //delete pPage;
	return true;

}

bool  CTranRedoPageManager::redo(CTranStorage *pTranStorage, IDBStorage* pDBStorage)
{
	/*{
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
	}*/
	{
		TRedoPageList::iterator<CTranStorage> it =  m_RedoPages.begin(pTranStorage);
		it.load();
		while(!it.isNull())
		{			
			sRedoPageInfo& pageInfo = it.value();
			FilePagePtr pPage(pTranStorage->getFilePage(pageInfo.nTranAddr, pageInfo.nPageSize));
			if(!pPage.get())
			{
				m_pTran->error(L"TRAN: Can't get page from Tran");
				return false;
			}
			bool bNew = (pageInfo.nFlags & eFP_NEW) != 0;
			bool bChange = (pageInfo.nFlags & eFP_CHANGE) != 0;
			bool bRemove = (pageInfo.nFlags & eFP_REMOVE) != 0;
			bool bFromFree = (pageInfo.nFlags & eFP_FROM_FREE_PAGES) != 0;
		

			if(bFromFree)
				pDBStorage->removeFromFreePage(pageInfo.nDBAddr);

			pPage->setAddr(pageInfo.nDBAddr);
			pPage->setNeedEncrypt(false);
			if(bRemove)
			{
				if(!bNew)
					pDBStorage->dropFilePage(pageInfo.nDBAddr);

			}
			else if(bNew)
			{
				if(bFromFree)
					pDBStorage->saveFilePage(pPage, true);
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
		m_Header.nRedoBlock = m_pStorage->getNewPageAddr(PAGE_SIZE_8K);
		m_Header.nUndoBlock = m_pStorage->getNewPageAddr(PAGE_SIZE_8K);
	}
	else
	{
		FilePagePtr pPage = m_pStorage->getFilePage(m_nRootPage, PAGE_SIZE_8K);
		if(!pPage.get())
			return; //TO DO Logs
		CommonLib::FxMemoryReadStream stream;	
		stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());
		sFilePageHeader header(stream, pPage->getPageSize(), m_bCheckCRC);
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

		 
	}

	m_RedoPages.setRoot(m_Header.nRedoBlock);
	m_UndoPages.setRoot(m_Header.nUndoBlock);
	
}
}

