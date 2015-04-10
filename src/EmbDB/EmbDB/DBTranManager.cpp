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
	{
		
	}
	CDBTranManager::~CDBTranManager()
	{

	}
	bool CDBTranManager::open(const CommonLib::str_t &sFileName, const CommonLib::str_t& sWorkingPath)
	{
		close();
		m_sWorkingPath = sWorkingPath;
		m_nTranID = 1;
		CommonLib::str_t sTranLogFileName = sFileName;
		sTranLogFileName += L".tran_log";
		bool bOpen = m_Storage.open(sTranLogFileName, false, false, false, true, 8192);
		if(!bOpen)
			return false;

		int64 nSize = m_Storage.getFileSzie();
		if(nSize != 0)
		{

			FilePagePtr pInfoPage (m_Storage.getFilePage(0));
			if(!pInfoPage.get())
				return false;
			if(!LoadHeader(pInfoPage.get()))
				return false;
		//	m_pBPtree.reset(new TBTreePlus(1, &m_Storage, m_pAlloc, 50));
		/*	TBTreePlus::iterator it = m_pBPtree->begin();
			while(it.isNull())
			{
				CommonLib::str_t sTranFileName((const char*)it.value().szTranName, 30);
				CTransactions tran(m_pAlloc, sTranFileName, m_pDB->getMainStorage());
				tran.restore();
			}

			m_Storage.close();

			CommonLib::FileSystem::deleteFile(sFileName);*/
		}
		else
		{
			FilePagePtr pInfoPage(m_Storage.getNewPage());
			int64 nBPRoot = m_Storage.getNewPageAddr();
			m_Info.nRootPageTree = nBPRoot;
			SaveHeader(pInfoPage.get());
			//m_pBPtree.reset(new TBTreePlus(nBPRoot, &m_Storage, m_pAlloc, 50));
			//m_pBPtree->saveBTreeInfo();
		}
		return true;
	}

	bool CDBTranManager::SaveHeader(CFilePage *pPage)
	{
		CommonLib::FxMemoryWriteStream stream;
		stream.attach(pPage->getRowData(), pPage->getPageSize());
		sFilePageHeader header(stream, TRAN_MANAGE_PAGE, TM_HEADER_PAGE);
		stream.write(m_Info.nRootPageTree);
		stream.write(m_nTranID);
		header.writeCRC32(stream);
 		m_Storage.saveFilePage(pPage);
		return true;
 
	}
	bool CDBTranManager::LoadHeader(CFilePage *pPage)
	{
		CommonLib::FxMemoryReadStream stream;
		stream.attach(pPage->getRowData(), pPage->getPageSize());
		sFilePageHeader header(stream);
		if(!header.isValid())
			return false;//TO DO DB LOg
		if(header.m_nObjectPageType != TRAN_MANAGE_PAGE || header.m_nSubObjectPageType != TM_HEADER_PAGE)
			return false;
		stream.read(m_Info.nRootPageTree);
		m_nTranID = stream.readInt64();
		return true;
	}
	bool  CDBTranManager::close()
	{
		TDBTransactions::iterator it = m_Transactions.begin();
		while(!it.isNull())
		{
			IDBTransactions* pTran = (IDBTransactions* )(*it);
			if(!pTran->isCompleted())
			{//TODO login and create  observer
				pTran->stop();
				pTran->wait();
			}
			delete pTran;
		}
		return m_Storage.close();
	}

	ITransactions* CDBTranManager::CreateTransaction(eTransactionsType trType)
	{
		CommonLib::str_t sFileName;
		long nDate = 0;
		long nTime = 0;
		nDate = CommonLib::TimeUtils::GetCurrentDate(&nTime);
		sFileName.format(_T("\\%d_%d_%I64d"), nDate, nTime, m_nTranID++);
		while(CommonLib::FileSystem::isFileExisit(sFileName))
		{
			sFileName.format(_T("\\%d_%d_%I64d"),nDate, nTime, m_nTranID++);
		}
		CTransactions *pTran = new CTransactions(m_pAlloc, rtUndo, trType, m_sWorkingPath + sFileName, m_pDB->getMainStorage(), m_nTranID++ );
		//CDirectTransactions *pTran = new CDirectTransactions(m_pAlloc, rtUndo, trType, m_sWorkingPath + sFileName, m_pDB->getMainStorage(), m_nTranID++ );
		m_Transactions.insert(pTran);
		return pTran;
	}
	bool CDBTranManager::releaseTransaction(ITransactions* pTran)
	{
		TDBTransactions::iterator it = m_Transactions.find(pTran);
		if(it.isNull())
			return false;
		m_Transactions.remove(it);
		delete pTran;
		return true;
	}
}