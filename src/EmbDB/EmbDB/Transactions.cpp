#include "stdafx.h"
#include "Database.h"
#include "Transactions.h"
#include "CommonLibrary/alloc_t.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/String.h"
#include "TransactionCache.h"

namespace embDB
{

	
	CTransaction::CTransaction(CommonLib::alloc_t* pAlloc, eRestoreType nRestoreType,
		eTransactionType nTranType, const CommonLib::CString& sFileName, CDatabase* pDatabase, int64 nID, uint32 nTranCache) :
		TBase(pDatabase)
		, m_TranStorage(pAlloc, &m_TranPerfCounter)
		, m_nRestoreType(nRestoreType)
		, m_nTranType(nTranType)
		, m_sFileName(sFileName)
		, m_PageChache(pAlloc, &m_TranStorage, this, &m_TranPerfCounter, nTranCache)
		, m_pAlloc(pAlloc)
		, m_bError(false)
		, m_TranUndoManager(this, &m_TranStorage)
		, m_LogStateManager(&m_TranStorage)
		, m_nID(nID)
		, m_bIsCompleted(true)
		, m_bIsBegin(false)
		, m_bDeleteStorage(true)
		, m_TranRedoManager(this, &m_TranStorage)
		, m_nPageSize(MIN_PAGE_SIZE)
	{

	}

	CTransaction::CTransaction(CommonLib::alloc_t* pAlloc, eRestoreType nRestoreType,
		eTransactionType nTranType, const CommonLib::CString& sFileName, IDBStorage* pDBStorage, int64 nID, uint32 nTranCache) :
		TBase(NULL)
		, m_TranStorage(pAlloc, &m_TranPerfCounter)
		, m_nRestoreType(nRestoreType)
		, m_nTranType(nTranType)
		, m_sFileName(sFileName)
		, m_PageChache(pAlloc, &m_TranStorage, this, &m_TranPerfCounter, nTranCache)
		, m_pAlloc(pAlloc)
		, m_bError(false)
		, m_TranUndoManager(this, &m_TranStorage)
		, m_LogStateManager(&m_TranStorage)
		, m_nID(nID)
		, m_bIsCompleted(true)
		, m_bIsBegin(false)
		, m_bDeleteStorage(true)
		, m_TranRedoManager(this, &m_TranStorage)
		, m_nPageSize(MIN_PAGE_SIZE)
	{
		m_pDBStorage = pDBStorage;
	}
	CTransaction::CTransaction(CommonLib::alloc_t* pAlloc, const CommonLib::CString& sFileName, IDBStorage* pDBStorage, uint32 nTranCache) :
		TBase(NULL)
		,m_TranStorage(pAlloc, &m_TranPerfCounter)
		,m_nRestoreType(rtUndefined)
		,m_nTranType(eTT_UNDEFINED)
		,m_sFileName(sFileName)
		,m_PageChache(pAlloc, &m_TranStorage, this, &m_TranPerfCounter, nTranCache)
		,m_pAlloc(pAlloc)
		,m_bError(false)
		,m_TranUndoManager(this, &m_TranStorage)
		, m_LogStateManager(&m_TranStorage)
		, m_nID(-1)
		, m_bIsCompleted(true)
		, m_bIsBegin(false)
		, m_bDeleteStorage(true)
		, m_TranRedoManager(this, &m_TranStorage)
		, m_nPageSize(MIN_PAGE_SIZE)
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
		CFilePage* pFilePage = m_TranStorage.getNewPage(MIN_PAGE_SIZE);
		if(!pFilePage)
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
		stream.attach(pFilePage->getRowData(), pFilePage->getPageSize());
		stream.write(m_Header.nRestoreType);
		stream.write(m_Header.nErrorLogHeader);
		stream.write(m_Header.nPageChangeHeader);
		stream.write(m_Header.nPageUndoRedoHeader);
		stream.write(m_Header.nPageStateHeader);
		m_TranStorage.saveFilePage(pFilePage, 0);
		if(m_nRestoreType == rtUndo)
			m_TranUndoManager.setFirstPage(m_Header.nPageUndoRedoHeader);
		else if(m_nRestoreType == rtRedo)
			m_TranRedoManager.setFirstPage(m_Header.nPageUndoRedoHeader, true);
		m_LogStateManager.Init(m_Header.nPageStateHeader, false);
		delete pFilePage;
		m_bIsCompleted = false;
		return true;

	}
	bool CTransaction::commit()
	{
		CommitTemp();
		m_bIsBegin = false;
		if(m_nTranType == eTT_SELECT)
			return true;
		

		if(m_nRestoreType == rtUndo)
		{
			return commit_undo();
		};
		if(m_nRestoreType == rtRedo)
		{
			return commit_redo();
		};
		m_bIsCompleted = true;
		return true;
	}
	bool CTransaction::rollback()
	{
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
		CFilePage* pFilePage = m_TranStorage.getFilePage(0, true);
		if(!pFilePage)
		{
			return false;
		}
		CommonLib::FxMemoryReadStream stream;
		stream.attach(pFilePage->getRowData(), pFilePage->getPageSize());
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
		m_TranUndoManager.setFirstPage(m_Header.nPageUndoRedoHeader);
		bool bRet = true;
		uint32 nState = m_LogStateManager.getState();
		uint64 nDBSize = m_LogStateManager.getDBSize();
		if(bForce || nState == eTS_BEGIN_COPY_TO_DB)
		{
			m_pDBStorage->lockWrite();


			if(m_Header.nRestoreType == rtUndo)
			{
				bRet = m_TranUndoManager.undo(&m_TranStorage, m_pDBStorage.get());
				m_pDBStorage->setFileSize(nDBSize);
			}


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
	FilePagePtr CTransaction::getFilePage(int64 nAddr, uint32 nSize, bool bRead)
	{
		if(m_nTranType == eTT_SELECT)
		{
			m_TranPerfCounter.ReadDBPage();
			return m_pDBStorage->getFilePage(nAddr, nSize, bRead);
		}
		CFilePage* pPage = m_PageChache.GetPage(nAddr, false, bRead, nSize);
		if(!pPage)
		{
			m_TranPerfCounter.ReadDBPage();
			FilePagePtr pStoragePage =  m_pDBStorage->getFilePage(nAddr, nSize, bRead);
			pPage = new CFilePage(m_pAlloc, pStoragePage->getRowData(), pStoragePage->getPageSize(), nAddr);
			/*if(m_nRestoreType == rtUndo || m_nRestoreType == rtUndoRedo)
			{
				SaveDBPage(pStoragePage.get());
			}*/
			//int64 nTranAddr = m_TranStorage.getNewPageAddr();
			m_PageChache.AddPage(nAddr, -1, pPage);
		}
		else if(pPage->getAddr() != -1)
		{
			m_TranPerfCounter.ReadFromChache();
		}
		if(pPage->getAddr() == -1)
		{
			m_TranPerfCounter.ReadDBPage();
			pPage->setAddr(nAddr);
			FilePagePtr pStoragePage =  m_pDBStorage->getFilePage(nAddr, bRead);
			pPage->copyFrom(pStoragePage.get());
		}

		return FilePagePtr(pPage);
	}

	FilePagePtr CTransaction::getFileDBPage(int64 nAddr,  uint32 nPageSize, bool bRead)
	{
		m_TranPerfCounter.ReadDBPage();
		return m_pDBStorage->getFilePage(nAddr, nPageSize, bRead);
	}

	void CTransaction::addUndoPage(FilePagePtr pPage, bool bReadFromDB )
	{
		if(bReadFromDB)
		{
			FilePagePtr pDBPage = m_pDBStorage->getFilePage(pPage->getAddr(), pPage->getPageSize());
			SaveDBPage(pDBPage.get());
		}
		else
			SaveDBPage(pPage.get());
	}

	FilePagePtr CTransaction::getTranNewPage(uint32 nSize)
	{
		int64 nTranAddr = m_TranStorage.getNewPageAddr(nSize);
		CFilePage *pFilePage = new CFilePage(m_pAlloc, nSize, nTranAddr);
		pFilePage->setFlag(eFP_NEW|eFP_INNER_TRAN_PAGE, true);
		m_PageChache.AddPage(-1, nTranAddr, pFilePage);
		return FilePagePtr(pFilePage);
	}
	FilePagePtr CTransaction::getTranFilePage(int64 nAddr, uint32 nSize, bool bRead)
	{
		CFilePage* pPage = m_PageChache.GetPage(nAddr, false, bRead, nSize);
	 	return FilePagePtr(pPage);
	}
	void CTransaction::saveTranFilePage(FilePagePtr pPage,  size_t nSize,  bool bChandgeInCache)
	{
		m_TranStorage.saveFilePage(pPage.get(), pPage->getAddr());
	}
	void CTransaction::dropFilePage(FilePagePtr pPage)
	{
		addUndoPage(pPage, true);
		m_vecRemovePages.push_back(pPage->getAddr());
	}
	void CTransaction::dropFilePage(int64 nAddr, uint32 nSize)
	{
		FilePagePtr pRemPage = m_pDBStorage->getFilePage(nAddr, nSize);
		addUndoPage(pRemPage);
		m_vecRemovePages.push_back(nAddr);
	}
	FilePagePtr CTransaction::getNewPage(uint32 nSize, bool bWrite)
	{
		if((nSize%m_nPageSize) != 0)
			return FilePagePtr();
 

		assert(m_nTranType != eTT_SELECT);
	//	uint32 nType = 0;
		int64 nAddr = m_pDBStorage->getNewPageAddr(nSize/*, &nType*/);

		int64 nTranAddr = m_TranStorage.getNewPageAddr(m_nPageSize);
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
	bool CTransaction::saveFilePage(FilePagePtr pPage,  size_t nSize, bool bChangeInCache )
	{
		m_PageChache.savePage(pPage.get());
		return true;
	}
	bool CTransaction::saveFilePage(CFilePage* pPage, size_t nDataSize,  bool ChandgeInCache)
	{
		m_PageChache.savePage(pPage);
		return true;
	}

	/*size_t CTransaction::getPageSize() const
	{
		return m_pDBStorage->getPageSize();
	}*/


	void CTransaction::error(const wchar_t *pszFormat, ...)
	{
		m_bError = true;
		va_list args;
		wchar_t* buffer;
		int len;

		va_start(args, pszFormat);
#if defined(_WIN32) && !defined(_WIN32_WCE)
		len = _vscwprintf(pszFormat, args);
#else
		len = 1000;
#endif
		buffer = (wchar_t*)m_pAlloc->alloc((len + 1)* sizeof (wchar_t)); 
		vswprintf(buffer, len, pszFormat, args);

		//TO DO WRITE

		m_pAlloc->free(buffer);


	}
	uint32 CTransaction::getLogLevel() const
	{
		return 1;
	}
	void CTransaction::log(uint32 nLevel, const wchar_t *pszFormat, ...)
	{

	}

	void CTransaction::addDBBTree(IDBBtree *pTree)
	{
		m_btrees.push_back(pTree);
	}

	bool CTransaction::SaveDBPage(CFilePage* pPage)
	{
		m_TranPerfCounter.AddUndoPage();

		//FilePagePtr pDBPage = m_pDBStorage->getFilePage(pPage->getAddr());

		 
		int64 nTranAddr = m_TranStorage.saveFilePage(pPage, -1);
		if(nTranAddr == -1)
		{
			CommonLib::CString sMsg;
			error(L"Transactions: Error save page: %I64d", pPage->getAddr());
			return false;
		}
		if(m_nRestoreType == rtUndo)
			return m_TranUndoManager.add( pPage->getAddr(), nTranAddr,  pPage->getFlags(), pPage->getPageSize());
		else if(m_nRestoreType == rtRedo)
			return m_TranRedoManager.add_undo( pPage->getAddr(), nTranAddr,  pPage->getFlags(), pPage->getPageSize());
		return false;
	}
	
	bool CTransaction::commit_undo()
	{
		for (size_t i = 0, sz = m_InnerTran.size(); i < sz; ++i)
		{
			m_InnerTran[i]->commit();
		}
		m_LogStateManager.setState(eTS_BEGIN);
		m_pDBStorage->lockWrite(this);
		int64  nStorageSize = m_pDBStorage->getFileSize();
		m_LogStateManager.setDBSize(nStorageSize);

		if(!m_vecFreePages.empty() || !m_vecRemovePages.empty())
		{
				//FilePagePtr pPage = getTranNewPage();
				m_pDBStorage->saveForUndoState(this);
		}

		m_PageChache.savePageForUndo(this);
		m_TranUndoManager.save();
	
	
		m_LogStateManager.save();
	
		m_LogStateManager.setState(eTS_BEGIN_COPY_TO_DB);
		m_TranStorage.Flush();


		for (size_t i = 0, sz = m_vecFreePages.size(); i < sz; ++i)
		{
			m_pDBStorage->removeFromFreePage(m_vecFreePages[i]);
		}
		for (size_t i = 0, sz = m_vecRemovePages.size(); i < sz; ++i)
		{
			m_TranPerfCounter.RemoveDBPage();
			m_pDBStorage->dropFilePage(m_vecRemovePages[i]);
		}
		
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
		for (size_t i = 0, sz = m_InnerTran.size(); i < sz; ++i)
		{
			m_InnerTran[i]->commit();
		}
		m_LogStateManager.setState(eTS_BEGIN);
		m_pDBStorage->lockWrite(this);
		int64  nStorageSize = m_pDBStorage->getFileSize();
		m_LogStateManager.setDBSize(nStorageSize);

		if(!m_vecFreePages.empty() || !m_vecRemovePages.empty())
		{
			//FilePagePtr pPage = getTranNewPage();
			m_pDBStorage->saveForUndoState(this);
		}




			m_PageChache.savePageForRedo(&m_TranRedoManager);
			m_TranRedoManager.save();
		

		m_LogStateManager.save();

		m_LogStateManager.setState(eTS_BEGIN_COPY_TO_DB);
		m_TranStorage.Flush();


		for (size_t i = 0, sz = m_vecFreePages.size(); i < sz; ++i)
		{
			m_pDBStorage->removeFromFreePage(m_vecFreePages[i]);
		}
		for (size_t i = 0, sz = m_vecRemovePages.size(); i < sz; ++i)
		{
			m_TranPerfCounter.RemoveDBPage();
			m_pDBStorage->dropFilePage(m_vecRemovePages[i]);
		}

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



	bool  CTransaction::isError() const
	{
		return m_bError;
	}
	size_t  CTransaction::getErrorMessageSize() const
	{
		return 0;
	}
	size_t  CTransaction::getErroMessage(wchar_t * pBuf, size_t nSize) const
	{
		return 0;
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