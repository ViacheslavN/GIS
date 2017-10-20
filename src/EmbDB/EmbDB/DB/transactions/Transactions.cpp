#include "stdafx.h"
#include "../Database.h"
#include "Transactions.h"
#include "CommonLibrary/alloc_t.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/String.h"
#include "TransactionCache.h"

namespace embDB
{

	
	CTransaction::CTransaction(CommonLib::alloc_t* pAlloc, eRestoreType nRestoreType,
		eTransactionDataType nTranType, const CommonLib::CString& sFileName,  
		IDBConnection* pConnection, int64 nID, uint32 nTranCache, CPageCipher *pPageCiher, bool bMultiThread) :
		TBase(pConnection, pAlloc)
		, m_TranStorage(pAlloc, &m_TranPerfCounter, pConnection->getCheckCRC(), pPageCiher)
		, m_nRestoreType(nRestoreType)
		, m_nTranType(nTranType)
		, m_sFileName(sFileName)
		, m_PageChache(pAlloc, &m_TranStorage, this, &m_TranPerfCounter, nTranCache)
		, m_LogStateManager(&m_TranStorage)
		, m_nID(nID)
		, m_bIsCompleted(true)
		, m_bIsBegin(false)
		, m_bDeleteStorage(true)
		, m_TranRedoManager(this, &m_TranStorage, pConnection->getCheckCRC())
		, m_nPageSize(MIN_PAGE_SIZE)
		, m_bMultiThread(bMultiThread)
	{

	}

	CTransaction::CTransaction(CommonLib::alloc_t* pAlloc, eRestoreType nRestoreType,
		eTransactionDataType nTranType, const CommonLib::CString& sFileName,
		IDBStorage* pDBStorage, int64 nID, uint32 nTranCache, CPageCipher *pPageCiher, bool bMultiThread) :
		TBase(NULL, pAlloc)
		, m_TranStorage(pAlloc, &m_TranPerfCounter, pDBStorage->getCheckCRC(), pPageCiher)
		, m_nRestoreType(nRestoreType)
		, m_nTranType(nTranType)
		, m_sFileName(sFileName)
		, m_PageChache(pAlloc, &m_TranStorage, this, &m_TranPerfCounter, nTranCache)
		, m_LogStateManager(&m_TranStorage)
		, m_nID(nID)
		, m_bIsCompleted(true)
		, m_bIsBegin(false)
		, m_bDeleteStorage(true)
		, m_TranRedoManager(this, &m_TranStorage, pDBStorage->getCheckCRC())
		, m_nPageSize(MIN_PAGE_SIZE)
		, m_bMultiThread(bMultiThread)
	{
		m_pDBStorage = pDBStorage;
	}
	CTransaction::~CTransaction()
	{

	}

	//ITransactions
	bool CTransaction::begin()
	{
		if(m_bIsBegin)
		{
			rollback();
		}


		log(10, L"Begin Tran ID: %I64d\n", getID());

		m_bIsBegin = true;

		if(m_nTranType == eTT_SELECT)
			return true;
		
		bool bOpen =  m_TranStorage.open(m_sFileName, /*m_pDBStorage->getPageSize(),*/ true);
		if(!bOpen)
		{
			error(L"Transactions: Can't create storage %s", m_sFileName.cstr());
			return false;
		}
		m_LogStateManager.setState(eTS_BEGIN);
		FilePagePtr pFilePage = m_TranStorage.getNewPage(MIN_PAGE_SIZE);
		if(!pFilePage.get())
		{
			error(L"Transactions: Can't create page addr:0");
			return false;
		}
		m_Header.nRestoreType = m_nRestoreType;
		m_Header.nErrorLogHeader = m_TranStorage.getNewPageAddr(nCommonPageSize);
		m_Header.nMesageLogHeader = m_TranStorage.getNewPageAddr(nCommonPageSize);
		m_Header.nPageChangeHeader = m_TranStorage.getNewPageAddr(nCommonPageSize);
		m_Header.nPageUndoRedoHeader = m_TranStorage.getNewPageAddr(nCommonPageSize);
		m_Header.nPageStateHeader = m_TranStorage.getNewPageAddr(nCommonPageSize);
		m_Header.nFreeUndoPage	= -1;
 
		CommonLib::FxMemoryWriteStream stream;
		stream.attachBuffer(pFilePage->getRowData(), pFilePage->getPageSize());
		stream.write(m_Header.nRestoreType);
		stream.write(m_Header.nErrorLogHeader);
		stream.write(m_Header.nPageChangeHeader);
		stream.write(m_Header.nPageUndoRedoHeader);
		stream.write(m_Header.nPageStateHeader);
		m_TranStorage.saveFilePage(pFilePage.get(), 0);
		/*if(m_nRestoreType == rtUndo)
		else if(m_nRestoreType == rtRedo)
			m_TranRedoManager.setFirstPage(m_Header.nPageUndoRedoHeader, true);*/
		m_LogStateManager.Init(m_Header.nPageStateHeader, false);
	
		m_bIsCompleted = false;
		return true;

	}
	bool CTransaction::commit()
	{

		log(10, L"commit Tran ID: %I64d", getID());
		CommitTemp();
		m_bIsBegin = false;
		if(m_nTranType == eTT_SELECT)
			return true;
		

		bool bRet = false;
		if(m_nRestoreType == rtUndo)
		{
			bRet = commit_undo();
		};
		if(m_nRestoreType == rtRedo)
		{
			bRet = commit_redo();
		};
		m_bIsCompleted = true;
		return bRet;
	}
	bool CTransaction::rollback()
	{

		log(10, L"rollback Tran ID: %I64d", getID());
		m_bIsBegin = false;
		if(m_nTranType == eTT_SELECT)
			return true;

		if(m_LogStateManager.getState() == eTS_BEGIN_COPY_TO_DB)
		{
			restore();
		}
		close();

		return true;
	}
	bool CTransaction::restore(bool bForce)
	{
		bool bOpen =  m_TranStorage.open(m_sFileName,/* m_pDBStorage->getPageSize(),*/ false);
		if(!bOpen)
		{
			return false;
		}
		FilePagePtr pFilePage = m_TranStorage.getFilePage(0, true);
		if(!pFilePage.get())
		{
			return false;
		}
		CommonLib::FxMemoryReadStream stream;
		stream.attachBuffer(pFilePage->getRowData(), pFilePage->getPageSize());
		stream.read(m_Header.nRestoreType);
		stream.read(m_Header.nErrorLogHeader);
		stream.read(m_Header.nPageChangeHeader);
		stream.read(m_Header.nPageUndoRedoHeader);
		stream.read(m_Header.nPageStateHeader);
		m_LogStateManager.Init(m_Header.nPageStateHeader, true);
		m_nRestoreType = m_Header.nRestoreType;
		if(m_nRestoreType == rtUndo)
		{
			return restore_undo(bForce);
		}
		else if(m_nRestoreType == rtRedo)
		{
			return restore_redo(bForce);
		}
		return false;
	}

	bool CTransaction::restore_undo(bool bForce)
	{

		bool bRet = true;
		uint32 nState = m_LogStateManager.getState();
		uint64 nDBSize = m_LogStateManager.getDBSize();
		if(bForce || nState == eTS_BEGIN_COPY_TO_DB)
		{
			m_pDBStorage->lockWrite();


			bRet = m_PageChache.undo(m_pDBStorage.get(), m_Header.nPageUndoRedoHeader);
			m_pDBStorage->setFileSize(nDBSize);


			if(bRet)
			{
				m_pDBStorage->clearDirty();
				m_pDBStorage->commit();
				m_pDBStorage->reload();
			}
			m_TranStorage.close();
			m_pDBStorage->unlockWrite();
		}
		return bRet;
	}

	bool CTransaction::restore_redo(bool bForce)
	{
		m_TranRedoManager.setFirstPage(m_Header.nPageUndoRedoHeader, false);
		bool bRet = true;

		return bRet;
	}
	FilePagePtr CTransaction::getFilePage(int64 nAddr, uint32 nSize, bool bRead, bool bNeedDecrypt, bool bAddInCache)
	{
		if(m_nTranType == eTT_SELECT)
		{
			m_TranPerfCounter.ReadDBPage();
			return m_pDBStorage->getFilePage(nAddr, nSize, bRead, bNeedDecrypt, bAddInCache);
		}

		FilePagePtr pPage = m_PageChache.GetPage(nAddr, false, bRead, nSize);
		if(!pPage.get())
		{
			m_TranPerfCounter.ReadDBPage();
			FilePagePtr pStoragePage =  m_pDBStorage->getFilePage(nAddr, nSize, bRead);
			pPage = new CFilePage(m_pAlloc, pStoragePage->getRowData(), pStoragePage->getPageSize(), nAddr);
			m_PageChache.AddPage(nAddr, -1, pPage.get());
		
		}
		else if(pPage->getAddr() != -1)
		{
			m_TranPerfCounter.ReadFromChache();
		}
		if(pPage->getAddr() == -1)
		{
			m_TranPerfCounter.ReadDBPage();
			pPage->setAddr(nAddr);
			FilePagePtr pStoragePage =  m_pDBStorage->getFilePage(nAddr, nSize, bRead, bNeedDecrypt);
			assert(pStoragePage.get());
			pPage->copyFrom(pStoragePage.get());
		}

		return FilePagePtr(pPage);
	}

	FilePagePtr CTransaction::getFileDBPage(int64 nAddr,  uint32 nPageSize, bool bRead)
	{
		m_TranPerfCounter.ReadDBPage();
		return m_pDBStorage->getFilePage(nAddr, nPageSize, bRead);
	}
 
	FilePagePtr CTransaction::getTranNewPage(uint32 nSize)
	{
		int64 nTranAddr = m_TranStorage.getNewPageAddr(nSize);
		CFilePage *pFilePage = new CFilePage(m_pAlloc, nSize, nTranAddr);
		pFilePage->setFlag(eFP_NEW|eFP_INNER_TRAN_PAGE, true);
		m_PageChache.AddPage(-1, nTranAddr, pFilePage);
		return FilePagePtr(pFilePage);
	}
	FilePagePtr CTransaction::getTranFilePage(int64 nAddr, uint32 nSize, bool bRead, bool bNeedDecrypt )
	{
	 
	 	return m_PageChache.GetPage(nAddr, false, bRead, nSize);
	}
	void CTransaction::saveTranFilePage(FilePagePtr pPage,  uint32 nSize,  bool bChandgeInCache)
	{
		m_TranStorage.saveFilePage(pPage.get(), pPage->getAddr());
	}
	void CTransaction::dropFilePage(FilePagePtr pPage)
	{
		dropFilePage(pPage->getAddr(), pPage->getPageSize());
	}
	void CTransaction::dropFilePage(int64 nAddr, uint32 nSize)
	{
	/*	uint32 nFlags = m_PageChache.GetPageFlags(nAddr);
		if (nFlags & (eFP_NEW | eFP_INNER_TRAN_PAGE))
			return;

		FilePagePtr pRemPage = m_pDBStorage->getFilePage(nAddr, nSize);
		addUndoPage(pRemPage);
		m_vecRemovePages.push_back(nAddr);*/
	}
	FilePagePtr CTransaction::getNewPage(uint32 nSize, bool bWrite, bool bAddInCache)
	{
		if((nSize%m_nPageSize) != 0)
			return FilePagePtr();
 

		assert(m_nTranType != eTT_SELECT);
	//	uint32 nType = 0;
		int64 nAddr = m_pDBStorage->getNewPageAddr(nSize/*, &nType*/);

		int64 nTranAddr = m_TranStorage.getNewPageAddr(nSize);
		CFilePage *pFilePage = new CFilePage(m_pAlloc, nSize, nAddr);
		pFilePage->setFlag(eFP_NEW, true);


	/*	if(nType & eFP_FROM_FREE_PAGES)
		{
			m_vecFreePages.push_back(nAddr);
			pFilePage->setFlag(eFP_NEW | eFP_FROM_FREE_PAGES, true);
		}*/


		m_PageChache.AddPage(nAddr, nTranAddr, pFilePage);
		return FilePagePtr(pFilePage);
	}
	bool CTransaction::saveFilePage(FilePagePtr pPage,  bool bChangeInCache )
	{		 
		return saveFilePage(pPage.get(), bChangeInCache);
	}
	bool CTransaction::saveFilePage(CFilePage* pPage, bool ChandgeInCache)
	{
		m_PageChache.savePage(pPage);
		return true;
	}

	/*uint32 CTransaction::getPageSize() const
	{
		return m_pDBStorage->getPageSize();
	}*/


	 
	void CTransaction::addDBBTree(IDBBtree *pTree)
	{
		m_btrees.push_back(pTree);
	}

	bool CTransaction::SaveDBPage(CFilePage* pPage)
	{
		m_TranPerfCounter.AddUndoPage();

		/*if (m_nRestoreType == rtUndo)
		{
			m_TranUndoManager.add(pPage->getAddr(), pPage->getPageSize());
			return true;
		}*/

		//FilePagePtr pDBPage = m_pDBStorage->getFilePage(pPage->getAddr());

		 
		int64 nTranAddr = m_TranStorage.saveFilePageWithRetAddr(pPage, -1);
		if(nTranAddr == -1)
		{
			CommonLib::CString sMsg;
			error(L"Transactions: Error save page: %I64d", pPage->getAddr());
			return false;
		}
		
		 if(m_nRestoreType == rtRedo)
			return m_TranRedoManager.add_undo( pPage->getAddr(), nTranAddr,  pPage->getFlags(), pPage->getPageSize());
		return false;
	}
	
	bool CTransaction::commit_undo()
	{
		for (uint32 i = 0, sz = (uint32)m_InnerTran.size(); i < sz; ++i)
		{
			m_InnerTran[i]->commit();
		}
		m_LogStateManager.setState(eTS_BEGIN);
		m_pDBStorage->lockWrite(this);
		int64  nStorageSize = m_pDBStorage->getFileSize();
		m_LogStateManager.setDBSize(nStorageSize);

		/*if(!m_vecFreePages.empty() || !m_vecRemovePages.empty())
		{
			m_pDBStorage->saveForUndoState(this);
		}
		*/

		m_PageChache.savePageForUndo(this, m_pDBStorage.get(), m_Header.nPageUndoRedoHeader);
		m_LogStateManager.save();
	
		m_LogStateManager.setState(eTS_BEGIN_COPY_TO_DB);
		m_TranStorage.Flush();


		/*for (uint32 i = 0, sz = (uint32)m_vecFreePages.size(); i < sz; ++i)
		{
			m_pDBStorage->removeFromFreePage(m_vecFreePages[i]);
		}
		for (uint32 i = 0, sz = (uint32)m_vecRemovePages.size(); i < sz; ++i)
		{
			m_TranPerfCounter.RemoveDBPage();
			m_pDBStorage->dropFilePage(m_vecRemovePages[i]);
		}*/
		
		m_PageChache.saveChange(m_pDBStorage.get());
		m_pDBStorage->commit();
		m_LogStateManager.setState(eTS_FINISH_COPY_TO_DB);
		m_LogStateManager.save();
		m_TranStorage.Flush();

		m_pDBStorage->unlockWrite(this);
		m_TranStorage.close(m_bDeleteStorage);
		m_bIsCompleted = true;
		return true;
	}
	bool CTransaction::commit_redo()
	{
		for (uint32 i = 0, sz = (uint32)m_InnerTran.size(); i < sz; ++i)
		{
			m_InnerTran[i]->commit();
		}
		m_LogStateManager.setState(eTS_BEGIN);
		m_pDBStorage->lockWrite(this);
		int64  nStorageSize = m_pDBStorage->getFileSize();
		m_LogStateManager.setDBSize(nStorageSize);

		/*if(!m_vecFreePages.empty() || !m_vecRemovePages.empty())
		{
			m_pDBStorage->saveForUndoState(this);
		}
		*/



			m_PageChache.savePageForRedo(this, m_Header.nPageUndoRedoHeader);
			m_TranRedoManager.save();
		

		m_LogStateManager.save();

		m_LogStateManager.setState(eTS_BEGIN_COPY_TO_DB);
		m_TranStorage.Flush();


	/*	for (uint32 i = 0, sz = (uint32)m_vecFreePages.size(); i < sz; ++i)
		{
			m_pDBStorage->removeFromFreePage(m_vecFreePages[i]);
		}
		for (uint32 i = 0, sz = (uint32)m_vecRemovePages.size(); i < sz; ++i)
		{
			m_TranPerfCounter.RemoveDBPage();
			m_pDBStorage->dropFilePage(m_vecRemovePages[i]);
		}
		*/
		m_PageChache.saveChange(m_pDBStorage.get());
		m_pDBStorage->commit();
		m_LogStateManager.setState(eTS_FINISH_COPY_TO_DB);
		m_LogStateManager.save();
		m_TranStorage.Flush();

		m_pDBStorage->unlockWrite(this);
		m_TranStorage.close(m_bDeleteStorage);
		m_bIsCompleted = true;
		return true;
	}



	 

	bool CTransaction::close()
	{
		m_TranStorage.close();
		m_PageChache.clear();
		return true;
	}

	void  CTransaction::addInnerTransactions(IDBTransaction *pTran)
	{
		m_InnerTran.push_back(pTran);
	}


	void  CTransaction::OutDebugInfo()
	{
		 m_TranPerfCounter.OutDebugInfo();
	}

 
}