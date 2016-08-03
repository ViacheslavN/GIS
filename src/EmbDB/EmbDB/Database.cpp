#include "stdafx.h"
#include "Database.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "DBMagicSymbol.h"
#include "storage.h"
#include "Transactions.h"
#include "DBTranManager.h"
#include "GlobalParams.h"
namespace embDB
{

	IDatabasePtr IDatabase::CreateDatabase()
	{
		return IDatabasePtr(new CDatabase());
	}
	

	CDatabase::CDatabase() :  m_bOpen(false)
	{
		m_pAlloc.reset(new CommonLib::simple_alloc_t());
		m_pStorage = (IDBStorage*)new CStorage(m_pAlloc.get());
		m_pTranManager.reset(new CDBTranManager(m_pAlloc.get(), this));
		m_pSchema = (IDBShema*)new CSchema(this);
	}
	CDatabase::~CDatabase()
	{

	}

	

	bool CDatabase::open(const wchar_t* pszName, DBTransactionMode mode, const wchar_t* pszWorkingPath, const  wchar_t* pszPassword)
	{
		close();
		bool bOpen =  m_pStorage->open(pszName, false, false, false, false/*, DEFAULT_PAGE_SIZE*/);
		if(!bOpen)
			return false;
		bOpen = m_pTranManager->open(pszName, pszWorkingPath);
		if(!bOpen)
			return false;
		int64 nfSize = m_pStorage->getFileSize();
		if(nfSize < MIN_PAGE_SIZE + HEADER_DB_PAGE_SIZE)
			return false;
		//m_pStorage->setPageSize(DEFAULT_PAGE_SIZE);
		FilePagePtr pHeadDBFile(m_pStorage->getFilePage(0, HEADER_DB_PAGE_SIZE));
		if(!pHeadDBFile.get())
			return false;

		if(!readHeadPage(pHeadDBFile.get()))
			return false;

		FilePagePtr pRootFile(m_pStorage->getFilePage(1, MIN_PAGE_SIZE));
		if(!pRootFile.get())
			return false;

		m_bOpen =  readRootPage(pRootFile.get());
		return m_bOpen;
	}
	bool CDatabase::create(const wchar_t* pszName, /*uint32 nPageSize,*/ DBTransactionMode mode, const wchar_t* pszWorkingPath, const wchar_t* pszPassword)
	{
		close();

		CGlobalParams::Instance().SetCheckCRC(true);

		m_dbHeader.bCheckCRC = true;
		m_dbHeader.nCryptoAlg = NONE_ALG;


	//	m_dbHeader.nCRC = 10;
	//	m_dbHeader.nVersion = 1;
	//	m_dbHeader.nMagicSymbol = DB_SYMBOL;
		
	 
		bool bOpen = m_pStorage->open(pszName, false, false, true, false);
		if(!bOpen)
			return false;
		bOpen = m_pTranManager->open(pszName, pszWorkingPath);
		if(!bOpen)
			return false;

		FilePagePtr pDBHeaderPage(m_pStorage->getNewPage(HEADER_DB_PAGE_SIZE, true));
		if(!pDBHeaderPage.get())
			return false;
		FilePagePtr pDBRootPage(m_pStorage->getNewPage(MIN_PAGE_SIZE, true));
		if(!pDBRootPage.get())
			return false;

		FilePagePtr pDBStoragePage(m_pStorage->getNewPage(MIN_PAGE_SIZE, true));
		if(!pDBStoragePage.get())
			return false;
		FilePagePtr pDBShemaPage(m_pStorage->getNewPage(MIN_PAGE_SIZE, true));
		if(!pDBShemaPage.get())
			return false;
		FilePagePtr pDBUserPage(m_pStorage->getNewPage(MIN_PAGE_SIZE, true));
		if(!pDBUserPage.get())
			return false;

		m_dbRootPage.nStoragePage = pDBStoragePage->getAddr();
		m_dbRootPage.nShemaPage = pDBShemaPage->getAddr();
		m_dbRootPage.nUserPage = pDBUserPage->getAddr();
		
		m_pStorage->initStorage(m_dbRootPage.nStoragePage);


		{

			CommonLib::FxMemoryWriteStream stream;
			stream.attachBuffer(pDBHeaderPage->getRowData(), pDBHeaderPage->getPageSize());
			sFilePageHeader header(stream, DATABASE_PAGE, DB_ROOT_PAGE, pDBHeaderPage->getPageSize(), m_DBParams.bCheckCRC);
			m_dbHeader.Write(&stream);
			header.writeCRC32(stream);
			m_pStorage->saveFilePage(pDBHeaderPage);
		}

		{

			CommonLib::FxMemoryWriteStream stream;
			stream.attachBuffer(pDBRootPage->getRowData(), pDBRootPage->getPageSize());
			sFilePageHeader header(stream, DATABASE_PAGE, DB_ROOT_PAGE, pDBRootPage->getPageSize(), m_DBParams.bCheckCRC);
			m_dbRootPage.Write(&stream);
			header.writeCRC32(stream);
			m_pStorage->saveFilePage(pDBRootPage);
		}


		if(!m_pSchema->open(m_pStorage.get(), m_dbRootPage.nShemaPage, true))
			return false;
		m_pStorage->saveStorageInfo();
		m_pStorage->commit();
		m_bOpen = true;
		return true;
	}
	bool CDatabase::close()
	{
		if(!m_bOpen)
			return false;

		m_bOpen = false;
		bool bRet = m_pStorage->close();
		bRet = m_pTranManager->close();
		bRet = m_pSchema->close();
		return bRet;
	}
	bool CDatabase::readHeadPage(CFilePage* pFilePage)
	{
		CommonLib::FxMemoryReadStream stream(m_pAlloc.get());
		stream.attachBuffer(pFilePage->getRowData(), pFilePage->getPageSize());
		sFilePageHeader header(stream, pFilePage->getPageSize(), true);
		if(!header.isValid())
		{
			//TO DO Logging
			return false;
		}

		if(header.m_nObjectPageType != DATABASE_PAGE || header.m_nSubObjectPageType != DB_HEADER_PAGE)
		{
			//TO DO Logging
			return false;
		}

		m_dbHeader.Read(&stream);

		return true;
	}
	bool CDatabase::readRootPage(CFilePage* pFilePage)
	{
		CommonLib::FxMemoryReadStream stream(m_pAlloc.get());
		stream.attachBuffer(pFilePage->getRowData(), pFilePage->getPageSize());
		sFilePageHeader header(stream, pFilePage->getPageSize(), true);
		if(!header.isValid())
		{
			//TO DO Logging
			return false;
		}
		if(header.m_nObjectPageType != DATABASE_PAGE || header.m_nSubObjectPageType != DB_ROOT_PAGE)
		{
			//TO DO Logging
			return false;
		}
		m_dbRootPage.Read(&stream);
	/*	if(m_dbRootPage.nMagicSymbol != DB_SYMBOL)
			return false;*/
		//if(m_dbHeader.nPageSize <= 0 )
		//	return false;
		//m_pStorage->setPageSize(m_dbHeader.nPageSize);
		m_pStorage->setStoragePageInfo(m_dbRootPage.nStoragePage);
		if(!m_pStorage->loadStorageInfo())
			return false;
		if(!CheckDirty())
			return false;
		if(m_dbRootPage.nShemaPage <= 0 )
			return false;
		if(!m_pSchema->open(m_pStorage.get(), m_dbRootPage.nShemaPage))
			return false;

		
		return true;
	}
	 
	CommonLib::alloc_t* CDatabase::getBTreeAlloc()
	{
		return m_pAlloc.get();
	}
	
	ITransactionPtr CDatabase::startTransaction(eTransactionType trType)
	{
		if(!m_bOpen)
			return ITransactionPtr();
		return m_pTranManager->CreateTransaction(trType);

		
	}
	bool CDatabase::closeTransaction(ITransaction* pTran)
	{
		if(!m_bOpen)
			return false;
		return m_pTranManager->releaseTransaction(pTran);
	}
	bool CDatabase::CheckDirty()
	{
		if(!m_pStorage->isDirty())
			return true;
		
		CTransaction tran(m_pAlloc.get(), rtUndo,  eTT_UNDEFINED, m_pStorage->getTranFileName(), m_pStorage.get(), -1);
		return tran.restore();
		
	}
	/*CStorage* CDatabase::getTableStorage(const CommonLib::CString& sFileName, bool bCreate)
	{
		TTableStorages::iterator it = m_TableStorages.find(sFileName);
		if(it.isNull())
		{
			if(!bCreate)
				return NULL;

			CStorage* pStorage = new CStorage(m_pAlloc.get());
			if(!pStorage->open(sFileName.cwstr(), false, false, false, true, m_pStorage->getPageSize()))
			{
				delete pStorage;
				return NULL;
			}
			if(!pStorage->getFileSzie())
			{
				FilePagePtr pPage(pStorage->getNewPage());
				pStorage->setStoragePageInfo(pPage->getAddr());
				pStorage->saveStorageInfo();
			}
			else
			{
				if(pStorage->isDirty())
				{
					CTransaction tran(m_pAlloc.get(), rtUndo,  eTT_UNDEFINED, pStorage->getTranFileName(), pStorage, -1);
					if(!tran.restore())
					{
						delete pStorage;
						return NULL;
					}
				}
			}
			m_TableStorages.insert(sFileName, pStorage);
			return pStorage;
		}
		return it.value();
	}*/
}