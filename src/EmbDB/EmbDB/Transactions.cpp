#include "stdafx.h"
#include "Transactions.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/str_t.h"
#include "TransactionCache.h"
namespace embDB
{

	

	CTransactions::CTransactions(CommonLib::alloc_t* pAlloc, eRestoreType nRestoreType,
		eTransactionsType nTranType, const CommonLib::str_t& sFileName, IDBStorage* pDBStorage, int64 nID, uint32 nTranCache) :
		m_TranStorage(pAlloc, &m_TranPerfCounter)
		, m_nRestoreType(nRestoreType)
		, m_nTranType(nTranType)
		, m_sFileName(sFileName)
		, m_pDBStorage(pDBStorage)
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
	{
		
	}
	CTransactions::CTransactions(CommonLib::alloc_t* pAlloc, const CommonLib::str_t& sFileName, IDBStorage* pDBStorage, uint32 nTranCache) :
	m_TranStorage(pAlloc, &m_TranPerfCounter)
		,m_nRestoreType(rtUndefined)
		,m_nTranType(eTT_UNDEFINED)
		,m_sFileName(sFileName)
		,m_pDBStorage(pDBStorage)
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
	{

	}
	CTransactions::~CTransactions()
	{

	}

	//ITransactions
	bool CTransactions::begin()
	{
		if(m_bIsBegin)
		{
			rollback();
		}

		m_bIsBegin = true;

		if(m_nTranType == eTT_SELECT)
			return true;
		
		bool bOpen =  m_TranStorage.open(m_sFileName, m_pDBStorage->getPageSize(), true);
		if(!bOpen)
		{
			CommonLib::str_t sMsg;
			sMsg.format(_T("Transactions: Can't create storage %s"), m_sFileName.cstr());
			error(sMsg);
			return false;
		}
		m_LogStateManager.setState(eTS_BEGIN);
		CFilePage* pFilePage = m_TranStorage.getNewPage();
		if(!pFilePage)
		{
			CommonLib::str_t sMsg;
			sMsg.format(_T("Transactions: Can't create page addr:0"));
			error(sMsg);
			return false;
		}
		m_Header.nRestoreType = m_nRestoreType;
		m_Header.nErrorLogHeader = m_TranStorage.getNewPageAddr();
		m_Header.nMesageLogHeader = m_TranStorage.getNewPageAddr();
		m_Header.nPageChangeHeader = m_TranStorage.getNewPageAddr();
		m_Header.nPageUndoRedoHeader = m_TranStorage.getNewPageAddr();
		m_Header.nPageStateHeader = m_TranStorage.getNewPageAddr();
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
	bool CTransactions::commit()
	{
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
	bool CTransactions::rollback()
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
	bool CTransactions::restore(bool bForce)
	{
		bool bOpen =  m_TranStorage.open(m_sFileName, m_pDBStorage->getPageSize(), false);
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

	bool CTransactions::restore_undo(bool bForce)
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
				bRet = m_TranUndoManager.undo(&m_TranStorage, m_pDBStorage);
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

	bool CTransactions::restore_redo(bool bForce)
	{
		m_TranRedoManager.setFirstPage(m_Header.nPageUndoRedoHeader, false);
		bool bRet = true;

		return bRet;
	}
	FilePagePtr CTransactions::getFilePage(int64 nAddr, bool bRead)
	{
		if(m_nTranType == eTT_SELECT)
		{
			m_TranPerfCounter.ReadDBPage();
			return m_pDBStorage->getFilePage(nAddr, bRead);
		}
		CFilePage* pPage = m_PageChache.GetPage(nAddr, false, bRead);
		if(!pPage)
		{
			m_TranPerfCounter.ReadDBPage();
			FilePagePtr pStoragePage =  m_pDBStorage->getFilePage(nAddr, bRead);
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

	FilePagePtr CTransactions::getFileDBPage(int64 nAddr, bool bRead)
	{
		m_TranPerfCounter.ReadDBPage();
		return m_pDBStorage->getFilePage(nAddr, bRead);
	}

	void CTransactions::addUndoPage(FilePagePtr pPage, bool bReadFromDB )
	{
		if(bReadFromDB)
		{
			FilePagePtr pDBPage = m_pDBStorage->getFilePage(pPage->getAddr());
			SaveDBPage(pDBPage.get());
		}
		else
			SaveDBPage(pPage.get());
	}

	FilePagePtr CTransactions::getTranNewPage()
	{
		int64 nTranAddr = m_TranStorage.getNewPageAddr();
		CFilePage *pFilePage = new CFilePage(m_pAlloc, m_pDBStorage->getPageSize(), nTranAddr);
		pFilePage->setFlag(eFP_NEW|eFP_INNER_TRAN_PAGE, true);
		m_PageChache.AddPage(-1, nTranAddr, pFilePage);
		return FilePagePtr(pFilePage);
	}
	FilePagePtr CTransactions::getTranFilePage(int64 nAddr, bool bRead)
	{
		CFilePage* pPage = m_PageChache.GetPage(nAddr, false, bRead);
	 	return FilePagePtr(pPage);
	}
	void CTransactions::saveTranFilePage(FilePagePtr pPage,  size_t nSize,  bool bChandgeInCache)
	{
		m_TranStorage.saveFilePage(pPage.get(), pPage->getAddr());
	}
	void CTransactions::dropFilePage(FilePagePtr pPage)
	{
		addUndoPage(pPage, true);
		m_vecRemovePages.push_back(pPage->getAddr());
	}
	void CTransactions::dropFilePage(int64 nAddr)
	{
		FilePagePtr pRemPage = m_pDBStorage->getFilePage(nAddr);
		addUndoPage(pRemPage);
		m_vecRemovePages.push_back(nAddr);
	}
	FilePagePtr CTransactions::getNewPage()
	{
		assert(m_nTranType != eTT_SELECT);
		uint32 nType = 0;
		int64 nAddr = m_pDBStorage->getNewPageAddr(&nType);

		int64 nTranAddr = m_TranStorage.getNewPageAddr();
		CFilePage *pFilePage = new CFilePage(m_pAlloc, m_pDBStorage->getPageSize(), nAddr);
		pFilePage->setFlag(eFP_NEW, true);


		if(nType & eFP_FROM_FREE_PAGES)
		{
			m_vecFreePages.push_back(nAddr);
			pFilePage->setFlag(eFP_NEW | eFP_FROM_FREE_PAGES, true);
		}


		m_PageChache.AddPage(nAddr, nTranAddr, pFilePage);
		return FilePagePtr(pFilePage);
	}
	void CTransactions::saveFilePage(FilePagePtr pPage,  size_t nSize, bool bChandgeInCache )
	{
		m_PageChache.savePage(pPage.get());
	}
	size_t CTransactions::getPageSize() const
	{
		return m_pDBStorage->getPageSize();
	}


	void CTransactions::error(const CommonLib::str_t& sError, uint32 nErrorID)
	{
		m_bError = true;

	}
	uint32 CTransactions::getLogLevel() const
	{
		return 1;
	}
	void CTransactions::log(uint32 nLevel, const CommonLib::str_t& sMessage)
	{

	}

	void CTransactions::addDBBTree(IDBBtree *pTree)
	{
		m_btrees.push_back(pTree);
	}

	bool CTransactions::SaveDBPage(CFilePage* pPage)
	{
		m_TranPerfCounter.AddUndoPage();

		//FilePagePtr pDBPage = m_pDBStorage->getFilePage(pPage->getAddr());

		 
		int64 nTranAddr = m_TranStorage.saveFilePage(pPage, -1);
		if(nTranAddr == -1)
		{
			CommonLib::str_t sMsg;
			sMsg.format(_T("Transactions: Error save page: %I64d"), pPage->getAddr());
			error(sMsg);
			return false;
		}
		if(m_nRestoreType == rtUndo)
			return m_TranUndoManager.add( pPage->getAddr(), nTranAddr,  pPage->getFlags());
		else if(m_nRestoreType == rtRedo)
			return m_TranRedoManager.add_undo( pPage->getAddr(), nTranAddr,  pPage->getFlags());
		return false;
	}
	
	bool CTransactions::commit_undo()
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
		
		m_PageChache.saveChange(m_pDBStorage);
		m_pDBStorage->commit();
		m_LogStateManager.setState(eTS_FINISH_COPY_TO_DB);
		m_LogStateManager.save();
		m_TranStorage.Flush();

		m_pDBStorage->unlockWrite(this);
		m_TranStorage.close(m_bDeleteStorage);
		m_bIsCompleted = true;
		return true;
	}
	bool CTransactions::commit_redo()
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

		m_PageChache.saveChange(m_pDBStorage);
		m_pDBStorage->commit();
		m_LogStateManager.setState(eTS_FINISH_COPY_TO_DB);
		m_LogStateManager.save();
		m_TranStorage.Flush();

		m_pDBStorage->unlockWrite(this);
		m_TranStorage.close(m_bDeleteStorage);
		m_bIsCompleted = true;
		return true;
	}



	bool  CTransactions::isError() const
	{
		return m_bError;
	}
	size_t  CTransactions::getErrorMessageSize() const
	{
		return 0;
	}
	size_t  CTransactions::getErroMessage(wchar_t * pBuf, size_t nSize) const
	{
		return 0;
	}

	bool CTransactions::close()
	{
		m_TranStorage.close();
		m_PageChache.clear();
		return true;
	}

	void  CTransactions::addInnerTransactions(IDBTransactions *pTran)
	{
		m_InnerTran.push_back(pTran);
	}


	void  CTransactions::OutDebugInfo()
	{
		 m_TranPerfCounter.OutDebugInfo();
	}
}