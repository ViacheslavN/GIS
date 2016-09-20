#include "stdafx.h"
#include "DBTranManager.h"
#include "Transactions.h"
#include "FilePage.h"
#include "storage.h"
#include "CommonLibrary/TimeUtils.h"
#include "DirectTransactions.h"
static const int64 nMaxTransactions_ID = 999999999999999;
namespace embDB
{


	CDBTranManager::CDBTranManager(CommonLib::alloc_t *pAlloc, CDatabase *pDB) :
		m_pAlloc(pAlloc)
		,m_Storage(pAlloc)
		,m_nTranID(1)
		,m_pDB(pDB)
		,m_pPageCliper(nullptr)
	{
		
	}
	CDBTranManager::~CDBTranManager()
	{

	}
	bool CDBTranManager::open(const CommonLib::CString &sFileName, const CommonLib::CString& sWorkingPath, ILogger *pLogger)
	{
		close();
		m_pLogger = pLogger;
		m_sWorkingPath = sWorkingPath;
		m_nTranID = 1;
		CommonLib::CString sTranLogFileName = sFileName;
		sTranLogFileName += L".tran_log";
		bool bOpen = m_Storage.open(sTranLogFileName.cwstr(), false, false, false, true);
		if(!bOpen)
			return false;

		int64 nSize = m_Storage.getFileSize();
		if(nSize != 0)
		{

			FilePagePtr pInfoPage (m_Storage.getFilePage(0, 8192)); //TO DO fix
			if(!pInfoPage.get())
				return false;
			if(!LoadHeader(pInfoPage.get()))
				return false;
		//	m_pBPtree.reset(new TBTreePlus(1, &m_Storage, m_pAlloc, 50));
		/*	TBTreePlus::iterator it = m_pBPtree->begin();
			while(it.isNull())
			{
				CommonLib::CString sTranFileName((const char*)it.value().szTranName, 30);
				CTransactions tran(m_pAlloc, sTranFileName, m_pDB->getMainStorage());
				tran.restore();
			}

			m_Storage.close();

			CommonLib::FileSystem::deleteFile(sFileName);*/
		}
		else
		{
			FilePagePtr pInfoPage(m_Storage.getNewPage(8192));
			int64 nBPRoot = m_Storage.getNewPageAddr(8192);
			m_Info.nRootPageTree = nBPRoot;
			SaveHeader(pInfoPage.get());
			//m_pBPtree.reset(new TBTreePlus(nBPRoot, &m_Storage, m_pAlloc, 50));
			//m_pBPtree->saveBTreeInfo();
		}
		return true;
	}


	void CDBTranManager::SetPageCipher(CPageCipher* pPageCliper)
	{
		m_pPageCliper = pPageCliper;
	}

	bool CDBTranManager::SaveHeader(CFilePage *pPage)
	{
		CommonLib::FxMemoryWriteStream stream;
		stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());
		sFilePageHeader header(stream, TRAN_MANAGE_PAGE, TM_HEADER_PAGE, pPage->getPageSize(), m_pDB->getCheckCRC());
		stream.write(m_Info.nRootPageTree);
		stream.write(m_nTranID);
		header.writeCRC32(stream);
 		m_Storage.saveFilePage(pPage);
		return true;
 
	}
	bool CDBTranManager::LoadHeader(CFilePage *pPage)
	{
		CommonLib::FxMemoryReadStream stream;
		stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());
		sFilePageHeader header(stream, pPage->getPageSize(), m_pDB->getCheckCRC());
		if(!header.isValid())
			return false;//TO DO Log
		if(header.m_nObjectPageType != TRAN_MANAGE_PAGE || header.m_nSubObjectPageType != TM_HEADER_PAGE)
			return false;
		stream.read(m_Info.nRootPageTree);
		m_nTranID = stream.readInt64();
		return true;
	}
	bool  CDBTranManager::close()
	{
		TDBTransactions::iterator it = m_Transactions.begin();
		for(; it != m_Transactions.end(); ++it)
		{
			IDBTransaction* pTran = (IDBTransaction* )(it->get());
			if(!pTran->isCompleted())
			{//TODO login and create  observer
				pTran->stop();
				pTran->wait();
			}
			//delete pTran;
		}
		m_pLogger.release();
		m_pPageCliper = nullptr;
		return m_Storage.close();
	}

	ITransactionPtr CDBTranManager::CreateTransaction(eTransactionDataType trType, IDBConnection *pConn, eDBTransationType trDbType)
	{
		CommonLib::CString sFileName;
		long nDate = 0;
		long nTime = 0;
		nDate = CommonLib::TimeUtils::GetCurrentDate(&nTime);
		sFileName.format(L"\\%d_%d_%I64d", nDate, nTime, m_nTranID++);
		while(CommonLib::FileSystem::isFileExisit(sFileName.wstr()))
		{
			sFileName.format(L"\\%d_%d_%I64d", nDate, nTime, m_nTranID++);
		}
		IDBTransaction *pTran = nullptr;
		switch(trDbType)
		{
			case  eTTFullTransaction:
				pTran	= new CTransaction(m_pAlloc, rtUndo, trType, m_sWorkingPath + sFileName, pConn, m_nTranID++, 10000, m_pPageCliper);
				
				
				break;
			case  eTTDirectTransaction:
			case  eTTDirectTransactionUndo:
				pTran	= new CDirectTransaction(m_pAlloc, rtUndo, trType, m_sWorkingPath + sFileName, pConn, m_nTranID++ );
				break;
		}
		
		if(!pTran)
		{
			//TO DO log
			return  ITransactionPtr();
		}

		pTran->SetLogger(m_pLogger.get());
		//CDirectTransactions *pTran = new CDirectTransactions(m_pAlloc, rtUndo, trType, m_sWorkingPath + sFileName, m_pDB->getMainStorage(), m_nTranID++ );
		ITransactionPtr pTranPtr(pTran);
		m_Transactions.insert(pTranPtr);
		return pTranPtr;
	}
	bool CDBTranManager::releaseTransaction(ITransaction* pTran)
	{
		TDBTransactions::iterator it = m_Transactions.find(ITransactionPtr(pTran));
		if(it == m_Transactions.end())
			return false;
		m_Transactions.erase(it);
		return true;
	}
}