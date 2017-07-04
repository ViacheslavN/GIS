#include "stdafx.h"
#include "Database.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "DBMagicSymbol.h"
#include "storage/storage.h"
#include "transactions/Transactions.h"
#include "transactions/DirectTran/DirectTransactions.h"
#include "DBTranManager.h"
#include "GlobalParams.h"
#include "Crypto/RandomGenerator.h"
#include "Crypto/PBKDF2.h"

#ifdef _USE_CRYPTOPP_LIB_
	#include "Crypto/CryptoPP/AES128.h"
#else
	#include "Crypto/AES128.h"
#endif
#include "Connection.h"

#include "FileLog.h"
#include "ConsolLog.h"


namespace embDB
{

	IDatabasePtr IDatabase::CreateDatabase(eLogMode logMode, const wchar_t* pszLogFile)
	{
		return IDatabasePtr(new CDatabase(logMode , pszLogFile));
	}
	

	CDatabase::CDatabase(eLogMode logMode, const wchar_t* pszLogName) :  m_bOpen(false), m_bLoad(false), m_UserCryptoManager(this), m_nLogLevel(0),
		m_TranMode(eTMMultiTransactions), m_LogMode(logMode), m_sFileLogPath(pszLogName)
	{
		m_pAlloc.reset(new CommonLib::simple_alloc_t());
		m_pStorage = (IDBStorage*)new CStorage(m_pAlloc.get());
		m_pTranManager.reset(new CDBTranManager(m_pAlloc.get(), this));
		m_pSchema = (IDBShema*)new CSchema(this);

		//AddRef(); //TO DO fix it
 

	}
	CDatabase::~CDatabase()
	{

	}

	

	bool CDatabase::open(const wchar_t* pszName, DBTransactionMode mode, const wchar_t* pszWorkingPath)
	{
		close();
		m_TranMode = mode;

		if(m_LogMode == lmConsole)
			m_pLogger = new CConsolLogger(mode != eTMSingleTransactions && mode != eTMSingleReadTransactions);
		else if(m_LogMode == lmFile)
			m_pLogger = new CFileLogger(m_sFileLogPath.cwstr(), false, mode != eTMSingleTransactions && mode != eTMSingleReadTransactions);

		bool bOpen =  m_pStorage->open(pszName, false, false, false, false/*, DEFAULT_PAGE_SIZE*/);
		if(!bOpen)
			return false;
		bOpen = m_pTranManager->open(pszName, pszWorkingPath, m_pLogger.get());
		if(!bOpen)
			return false;
		int64 nfSize = m_pStorage->getFileSize();
		if(nfSize < MIN_PAGE_SIZE + HEADER_DB_PAGE_SIZE)
			return false;
		//m_pStorage->setPageSize(DEFAULT_PAGE_SIZE);
		FilePagePtr pHeadDBFile = m_pStorage->getFilePage(0, HEADER_DB_PAGE_SIZE);
		if(!pHeadDBFile.get())
			return false;

		if(!readHeadPage(pHeadDBFile.get()))
			return false;

		if(!m_UserCryptoManager.load(m_dbHeader.nUserCryptoManager))
			return false;

		m_bOpen = true;
		m_pTranManager->SetPageCipher(m_UserCryptoManager.GetPageCipher());
		return true;

		 
	}
	bool CDatabase::create(const wchar_t* pszName,  DBTransactionMode mode, const wchar_t* pszWorkingPath,
		  const wchar_t* pszPassword, const SDBParams *pParams)
	{
		close();
		m_TranMode = mode;
		CGlobalParams::Instance().SetCheckCRC(true);



		if(m_LogMode == lmConsole)
			m_pLogger = new CConsolLogger(mode != eTMSingleTransactions && mode != eTMSingleReadTransactions);
		else if(m_LogMode == lmFile)
			m_pLogger = new CFileLogger(m_sFileLogPath.cwstr(), false, mode != eTMSingleTransactions && mode != eTMSingleReadTransactions);
	
	//	m_dbHeader.nCRC = 10;
	//	m_dbHeader.nVersion = 1;
	//	m_dbHeader.nMagicSymbol = DB_SYMBOL;
		
	 
		bool bOpen = m_pStorage->open(pszName, false, false, true, false);
		if(!bOpen)
			return false;
		bOpen = m_pTranManager->open(pszName, pszWorkingPath, m_pLogger.get());
		if(!bOpen)
			return false;


	
		if(pParams)
		{
			m_dbHeader.qryptoAlg = pParams->qryptoAlg;
			m_dbHeader.bCheckCRC =  pParams->bCheckCRC;
			m_dbHeader.bCheckPage =  pParams->bCheckPage;
			m_dbHeader.bCheckPWD =  pParams->bCheckPWD;
		}


		


		FilePagePtr pDBHeaderPage(m_pStorage->getNewPage(HEADER_DB_PAGE_SIZE, true));
		if(!pDBHeaderPage.get())
			return false;

		FilePagePtr pDBUserPage(m_pStorage->getNewPage(MIN_PAGE_SIZE, true));
		if(!pDBUserPage.get())
			return false;

		

		

		

		/*int64 nOffset = 0;
		if(m_UserCryptoManager.getMode() == CUserCryptoManager::ePasswordMode || m_UserCryptoManager.getMode() == CUserCryptoManager::eMultiUser)
		{
			InitCrypto((byte*)sPassword.cstr(), sPassword.length()); 

			 
		}*/
		 
	
		FilePagePtr pDBRootPage(m_pStorage->getNewPage(MIN_PAGE_SIZE, true));
		if(!pDBRootPage.get())
			return false;


		m_dbHeader.nRootPage = pDBRootPage->getAddr();

		FilePagePtr pDBStoragePage(m_pStorage->getNewPage(MIN_PAGE_SIZE, true));
		if(!pDBStoragePage.get())
			return false;
		FilePagePtr pDBShemaPage(m_pStorage->getNewPage(MIN_PAGE_SIZE, true));
		if(!pDBShemaPage.get())
			return false;
	

		m_dbRootPage.nStoragePage = pDBStoragePage->getAddr();
		m_dbRootPage.nShemaPage = pDBShemaPage->getAddr();
		m_dbHeader.nUserCryptoManager = pDBUserPage->getAddr();
	
		
		m_pStorage->initStorage(m_dbRootPage.nStoragePage);


		m_UserCryptoManager.init(pDBUserPage->getAddr(),  pszPassword, m_dbHeader.qryptoAlg);

		{

			CommonLib::FxMemoryWriteStream stream;
			stream.attachBuffer(pDBHeaderPage->getRowData(), pDBHeaderPage->getPageSize());
			sFilePageHeader header(stream, DATABASE_PAGE, DB_HEADER_PAGE, pDBHeaderPage->getPageSize(), m_DBParams.bCheckCRC);
			m_dbHeader.Write(&stream);
			header.writeCRC32(stream);
			pDBHeaderPage->setNeedEncrypt(false);

		 
			int64 nOffset = m_pStorage->GetOffset();
			m_pStorage->SetOffset(0);
			m_pStorage->saveFilePage(pDBHeaderPage);
			m_pStorage->SetOffset(nOffset);

			//if(nOffset != 0)
			//		((CStorage*)m_pStorage.get())->SetOffset(nOffset);
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


		m_pTranManager->SetPageCipher(m_UserCryptoManager.GetPageCipher());

		return true;
	}

	bool CDatabase::create(const wchar_t* pszDbName,const wchar_t* pszAdmUser, const wchar_t* pszPassword , DBTransactionMode mode, 
		const wchar_t* pszWorkingPath, const SDBParams *Params)
	{
		return false;
	}

	bool CDatabase::close()
	{
		if(!m_bOpen)
			return false;

		m_bOpen = false;
		m_bLoad = false;
		bool bRet = m_pStorage->close();
		bRet = m_pTranManager->close();
		bRet = m_pSchema->close();
		m_UserCryptoManager.close();
		m_pLogger.release();
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
	CommonLib::alloc_t*  CDatabase::getCommonAlloc()
	{
		return m_pAlloc.get();
	}
	ITransactionPtr CDatabase::startTransaction(eTransactionDataType trType, uint64 nUserID, IDBConnection *pConn, eDBTransationType trDbType)
	{
		if(!m_bOpen)
			return ITransactionPtr();
		return m_pTranManager->CreateTransaction(trType, pConn, trDbType);

		
	}
	bool CDatabase::closeTransaction(ITransaction* pTran, uint64 nUserID)
	{
		if(!m_bOpen)
			return false;
		return m_pTranManager->releaseTransaction(pTran);
	}
	bool CDatabase::CheckDirty()
	{
		if(!m_pStorage->isDirty())
			return true;

		if(m_pStorage->getTranDBType() == eTTFullTransaction)
		{
			CTransaction tran(m_pAlloc.get(), rtUndo,  eTT_UNDEFINED, m_pStorage->getTranFileName(), m_pStorage.get(), -1);
			return tran.restore();
		}
		else
		{
			CDirectTransaction tran(m_pAlloc.get(), rtUndo,  eTT_UNDEFINED, m_pStorage->getTranFileName(), m_pStorage.get(), -1, 10000, m_pStorage->getTranDBType());
			return tran.restore();
		}
		
		
	}
	int64 CDatabase::InitCrypto(byte* pPWD, uint32 nLen)
	{

		CRandomGenerator::GetRandomValues(m_dbHeader.szSalt, sDBHeader::SALT_SIZE);
		CRandomGenerator::GetRandomValues(m_dbHeader.szSaltIV, sDBHeader::SALT_SIZE);

		int64 nOffset = CalcOffset(pPWD, nLen, m_dbHeader.szSalt, sDBHeader::SALT_SIZE);
	 


		m_PageChiper.reset( new CPageCipher(m_DBParams.qryptoAlg));
		m_PageChiper->SetKey(pPWD, nLen, m_dbHeader.szSalt, m_dbHeader.szSaltIV, sDBHeader::SALT_SIZE);
		((CStorage*)m_pStorage.get())->setPageChiper(m_PageChiper.get());
		CreateCheckPWDPage();
		CreateNoise(nOffset);


		return nOffset;
	
	}
	void CDatabase::CreateCheckPWDPage()
	{
		FilePagePtr pPage = m_pStorage->getNewPage(MIN_PAGE_SIZE, true);
		
		m_dbHeader.nPWDPage = pPage->getAddr();



		CommonLib::FxMemoryWriteStream stream;
		stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());

		CRandomGenerator::GetRandomValues(pPage->getRowData() + sFilePageHeader::size(true, false), MIN_PAGE_SIZE - sFilePageHeader::size(true, false));
		sFilePageHeader fph(stream, -1, -1, MIN_PAGE_SIZE, true, false);
		fph.m_bCheckPageType = false;
		stream.seek( pPage->getPageSize(), CommonLib::soFromBegin);

		fph.writeCRC32(stream);

		m_pStorage->saveFilePage(pPage);
		 
	}
	int64 CDatabase::CalcOffset(const byte* pPWD, uint32 nLen, const byte* pSlat, uint32 nSaltLen) const
	{
		int64 nOffset = 0;
		CPBKDF2::PBKDF2(pPWD, nLen, pSlat, nSaltLen, (byte*)&nOffset, 2, 1000);
		return nOffset;
	}


	void CDatabase::CreateNoise(int64 nSize)
	{
		std::vector<byte> vecRandomData(nSize);
		CRandomGenerator::GetRandomValues(&vecRandomData[0], (uint32)nSize);
		m_pStorage->WriteRowData(&vecRandomData[0], (uint32)nSize);
	}

	IConnectionPtr CDatabase::connect(const wchar_t* pszUser, const wchar_t* pszPassword)
	{
		if(!m_UserCryptoManager.CheckUser(pszUser, pszPassword))
		{
			return IConnectionPtr(); // TO DO Log error
		}

		if(!m_bLoad)
		{
			if(!load())
				return IConnectionPtr(); // TO DO Log error

			m_bLoad = true;
		}

		uint64 nUserID = m_UserCryptoManager.GetUserID(pszUser);
		eUserGroup userGroup = m_UserCryptoManager.GetUserGroup(pszUser);

		CConnection *pConnection = new CConnection(this, m_pSchema.get(), pszUser, nUserID, userGroup);

		m_Connections.insert(IConnectionPtr(pConnection));

		return IConnectionPtr(pConnection);
	}
	bool CDatabase::closeConnection(IConnection *pConnection)
	{
	 
		TConnections::iterator it = m_Connections.find(IConnectionPtr(pConnection));
		if(it == m_Connections.end())
			return false; // TO DO Log
		m_Connections.erase(it);
		return true;
	}

	bool CDatabase::load()
	{
		FilePagePtr pRootFile(m_pStorage->getFilePage(m_dbHeader.nRootPage, MIN_PAGE_SIZE));
		if(!pRootFile.get())
			return false;
		return readRootPage(pRootFile.get());
	}

 
	eLogMode CDatabase::getLogMode() const
	{
		if(!m_pLogger.get())
			return lmUndefined;

		return m_pLogger->GetLogMode();
	}


	void  CDatabase::setLogLevel(uint32 nLevel)
	{
		if(m_pLogger.get())
			m_pLogger->SetLogLevel(nLevel);
	}
	uint32  CDatabase::getLogLevel()
	{
		if(m_pLogger.get())
			m_pLogger->GetLogLevel();

		return 0;
	}
	 
}