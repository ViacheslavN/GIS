#include "stdafx.h"
#include "Database.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "DBMagicSymbol.h"
#include "storage.h"
#include "Transactions.h"
#include "DBTranManager.h"
#include "GlobalParams.h"
#include "RandomGenerator.h"
#include "PBKDF2.h"

#ifdef _USE_CRYPTOPP_LIB_
	#include "Crypto/CryptoPP/AES128.h"
#else
	#include "Crypto/AES128.h"
#endif
#include "Connection.h"

namespace embDB
{

	IDatabasePtr IDatabase::CreateDatabase()
	{
		return IDatabasePtr(new CDatabase());
	}
	

	CDatabase::CDatabase() :  m_bOpen(false), m_bLoad(false), m_UserCryptoManager(this)
	{
		m_pAlloc.reset(new CommonLib::simple_alloc_t());
		m_pStorage = (IDBStorage*)new CStorage(m_pAlloc.get());
		m_pTranManager.reset(new CDBTranManager(m_pAlloc.get(), this));
		m_pSchema = (IDBShema*)new CSchema(this);
	}
	CDatabase::~CDatabase()
	{

	}

	

	bool CDatabase::open(const wchar_t* pszName, DBTransactionMode mode, const wchar_t* pszWorkingPath)
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

		if(!m_UserCryptoManager.load(m_dbHeader.nUserCryptoManager))
			return false;

		m_bOpen = true;

		return true;

		/*CommonLib::CString sPassword = pszPassword;

		if(sPassword.length() && m_dbHeader.qryptoAlg == NONE_ALG)
			return false; //TO DO log

		if(!sPassword.length() && m_dbHeader.qryptoAlg != NONE_ALG)
			return false; //TO DO log

		if(sPassword.length())
		{
 
			int64 nOffset = CalcOffset((const byte*)sPassword.cstr(), sPassword.length(), m_dbHeader.szSalt, sDBHeader::SALT_SIZE);
			m_PageChiper.reset( new CPageCipher((const byte*)sPassword.cstr(), sPassword.length(), (const byte*)m_dbHeader.szSalt, m_dbHeader.szSaltIV, sDBHeader::SALT_SIZE, m_DBParams.qryptoAlg));
			((CStorage*)m_pStorage.get())->setPageChiper(m_PageChiper.get());

			FilePagePtr pPagePWD = m_pStorage->getFilePage(m_dbHeader.nPWDPage, MIN_PAGE_SIZE);
			CommonLib::FxMemoryReadStream stream;
			stream.attachBuffer(pPagePWD->getRowData(), pPagePWD->getPageSize());
			sFilePageHeader header(stream, pPagePWD->getPageSize(), true, false);
			if(!header.isValid())
				return false; //TO DO log

			((CStorage*)m_pStorage.get())->SetOffset(nOffset);
		
		}



		FilePagePtr pRootFile(m_pStorage->getFilePage(m_dbHeader.nRootPage, MIN_PAGE_SIZE));
		if(!pRootFile.get())
			return false;

		m_bOpen =  readRootPage(pRootFile.get());
		return m_bOpen;*/
	}
	bool CDatabase::create(const wchar_t* pszName,  DBTransactionMode mode, const wchar_t* pszWorkingPath,
		  const wchar_t* pszPassword, const SDBParams *pParams)
	{
		close();

		CGlobalParams::Instance().SetCheckCRC(true);


		


	//	m_dbHeader.nCRC = 10;
	//	m_dbHeader.nVersion = 1;
	//	m_dbHeader.nMagicSymbol = DB_SYMBOL;
		
	 
		bool bOpen = m_pStorage->open(pszName, false, false, true, false);
		if(!bOpen)
			return false;
		bOpen = m_pTranManager->open(pszName, pszWorkingPath);
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
		return true;
	}

	bool CDatabase::create(const wchar_t* pszDbName,const wchar_t* pszAdmUser, const wchar_t* pszPassword , DBTransactionMode mode, 
		const wchar_t* pszWorkingPath, const SDBParams *Params)
	{
		return true;
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
	
	ITransactionPtr CDatabase::startTransaction(eTransactionType trType, uint64 nUserID, IDBConnection *pConn)
	{
		if(!m_bOpen)
			return ITransactionPtr();
		return m_pTranManager->CreateTransaction(trType, pConn);

		
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
		
		CTransaction tran(m_pAlloc.get(), rtUndo,  eTT_UNDEFINED, m_pStorage->getTranFileName(), m_pStorage.get(), -1);
		return tran.restore();
		
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
		CRandomGenerator::GetRandomValues(&vecRandomData[0], nSize);
		m_pStorage->WriteRowData(&vecRandomData[0], nSize);
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

	return IConnectionPtr(new CConnection(this, m_pSchema.get(), pszUser, nUserID, userGroup));
	}
	bool CDatabase::closeConnection(IConnection *pConnection)
	{
		return false;
	}

	bool CDatabase::load()
	{
		FilePagePtr pRootFile(m_pStorage->getFilePage(m_dbHeader.nRootPage, MIN_PAGE_SIZE));
		if(!pRootFile.get())
			return false;
		return readRootPage(pRootFile.get());
	}
	 
}