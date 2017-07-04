#include "stdafx.h"
#include "Database.h"


#include "UserCryptoManager.h"
#include "../Crypto/RandomGenerator.h"
#include "../Crypto/PBKDF2.h"
#include "storage/storage.h"

#ifdef _USE_CRYPTOPP_LIB_
#include "Crypto/CryptoPP/AES128.h"
#else
#include "Crypto/AES128.h"
#endif

namespace embDB
{
	CUserCryptoManager::CUserCryptoManager(CDatabase *pDB) : m_pDB(pDB), m_UserMode(eNoneMode), m_nOffset(0), m_nPWDPage(-1),
		m_qryptoAlg(NONE_ALG), m_bPWDCheck(false)
	{
		memset(m_szSalt, 0, sizeof(m_szSalt));
		memset(m_szSaltIV, 0, sizeof(m_szSaltIV));
	}
	CUserCryptoManager::~CUserCryptoManager()
	{

	}

	void CUserCryptoManager::close()
	{
		m_qryptoAlg = NONE_ALG;
		m_bPWDCheck = false;
		m_nOffset = 0;
		m_PageChiper.reset();
	}

	bool CUserCryptoManager::init(int64 nAddr, const wchar_t *pszPassword, QryptoALG qryptoAlg)
	{
		IDBStoragePtr pDBStorage = m_pDB->getDBStorage();
		FilePagePtr pPage = pDBStorage->getFilePage(nAddr, MIN_PAGE_SIZE);
		if(!pPage.get())
			return false; //TO DO log

		m_qryptoAlg = qryptoAlg;

		if(pszPassword == NULL || pszPassword[0] == L'\0')
			m_UserMode = eNoneMode;
		else
			m_UserMode = ePasswordMode;

		if(m_UserMode != ePasswordMode)
		{
			CommonLib::FxMemoryWriteStream stream;
			stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());
			sFilePageHeader header(stream, DATABASE_PAGE, DB_ROOT_USER_PAGE, pPage->getPageSize(),true);
			stream.write((uint16)m_UserMode);
			header.writeCRC32(stream);
			pDBStorage->saveFilePage(pPage);
			return true;
		}
		
		return InitPasswordMode(pPage.get(), pszPassword);
	}

	bool  CUserCryptoManager::InitPasswordMode(CFilePage *pPage, const wchar_t *pszPassword)
	{

		IDBStoragePtr pDBStorage = m_pDB->getDBStorage();

		CRandomGenerator::GetRandomValues(m_szSalt, ___nSALT_SIZE);
		CRandomGenerator::GetRandomValues(m_szSaltIV, ___nSALT_SIZE);


		CommonLib::CString sPWD = pszPassword;

		std::vector<byte> vecUtf8PWD;

		uint32 nUtf8Len = sPWD.calcUTF8Length();
		vecUtf8PWD.resize(nUtf8Len + 1);
		sPWD.exportToUTF8((char*)&vecUtf8PWD[0], nUtf8Len);

		m_nOffset = CalcOffset(&vecUtf8PWD[0], nUtf8Len, m_szSalt, ___nSALT_SIZE);

		m_PageChiper.reset( new CPageCipher( m_qryptoAlg));
		m_pCheckPageChiper.reset(new CPageCipher(m_qryptoAlg));
		m_PageChiper->SetKey(&vecUtf8PWD[0], nUtf8Len, m_szSalt, m_szSaltIV, ___nSALT_SIZE);
		((CStorage*)pDBStorage.get())->setPageChiper(m_PageChiper.get());

		m_nPWDPage = CreateCheckPWDPage();
		CreateNoise(m_nOffset);


		CommonLib::FxMemoryWriteStream stream;
		stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());
		sFilePageHeader header(stream, DATABASE_PAGE, DB_ROOT_USER_PAGE, pPage->getPageSize(),true);
		stream.write((uint16)m_UserMode);
		stream.write((uint16)m_qryptoAlg);
		stream.write(m_szSalt, ___nSALT_SIZE);
		stream.write(m_szSaltIV, ___nSALT_SIZE);
		stream.write(m_nPWDPage);
		header.writeCRC32(stream);

		pPage->setNeedEncrypt(false);
		pDBStorage->saveFilePage(pPage);

		pDBStorage->SetOffset(m_nOffset);
		m_bPWDCheck = true;
		return true;

	}


	int64 CUserCryptoManager::CreateCheckPWDPage()
	{
		IDBStoragePtr pDBStorage = m_pDB->getDBStorage();
		FilePagePtr pPage = pDBStorage->getNewPage(MIN_PAGE_SIZE, true);

		int64 nPWDPage = pPage->getAddr();



		CommonLib::FxMemoryWriteStream stream;
		stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());

		CRandomGenerator::GetRandomValues(pPage->getRowData() + sFilePageHeader::size(true, false), MIN_PAGE_SIZE - sFilePageHeader::size(true, false));
		sFilePageHeader fph(stream, -1, -1, MIN_PAGE_SIZE, true, false);
		fph.m_bCheckPageType = false;
		stream.seek( pPage->getPageSize(), CommonLib::soFromBegin);

		fph.writeCRC32(stream);

		pDBStorage->saveFilePage(pPage);

		return nPWDPage;

	}
	uint32 CUserCryptoManager::CalcOffset(const byte* pPWD, uint32 nLen, const byte* pSlat, uint32 nSaltLen) const
	{
		uint32 nOffset = 0;
		CPBKDF2::PBKDF2(pPWD, nLen, pSlat, nSaltLen, (byte*)&nOffset, 2, 1000);
		return nOffset;
	}


	void CUserCryptoManager::CreateNoise(uint32 nSize)
	{
		IDBStoragePtr pDBStorage = m_pDB->getDBStorage();
		std::vector<byte> vecRandomData(nSize);
		CRandomGenerator::GetRandomValues(&vecRandomData[0], nSize);
		pDBStorage->WriteRowData(&vecRandomData[0], nSize);
	}


	bool CUserCryptoManager::init(int64 nAddr, const wchar_t *pszAdmName, const wchar_t *pszPassword,   QryptoALG qryptoAlg)
	{

		IDBStoragePtr pDBStorage = m_pDB->getDBStorage();
		FilePagePtr pPage = pDBStorage->getFilePage(nAddr, MIN_PAGE_SIZE);
		if(!pPage.get())
			return false;

	

		return true;
	}
	bool CUserCryptoManager::load(int64 nAddr)
	{
		IDBStoragePtr pDBStorage = m_pDB->getDBStorage();
		FilePagePtr pPage = pDBStorage->getFilePage(nAddr, MIN_PAGE_SIZE);
		if(!pPage.get())
			return false;

		CommonLib::FxMemoryReadStream stream;
		stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());
		sFilePageHeader header(stream,  pPage->getPageSize(),true);
		if(!header.isValid())
		{
			//TO DO Logging
			return false;
		}

		if(header.m_nObjectPageType != DATABASE_PAGE || header.m_nSubObjectPageType != DB_ROOT_USER_PAGE)
		{
			//TO DO Logging
			return false;
		}

		m_UserMode = (eUserMode)stream.readintu16();

		if(m_UserMode == ePasswordMode)
			return LoadPasswordMode(&stream);

		return true;
	}

	CUserCryptoManager::eUserMode CUserCryptoManager::getMode() const
	{
		return m_UserMode;
	}


	bool CUserCryptoManager::CheckUser(const wchar_t *pszUser, const wchar_t *pszPassword)
	{
		if(m_UserMode == eNoneMode)
			return true;

		 if(m_UserMode == ePasswordMode)
			 return CheckPassword(pszPassword);


		return false;
	}
	bool CUserCryptoManager::GetKey(const wchar_t *pszUser, const wchar_t *pszPassword, CommonLib::CBlob& blob)
	{
		return true;
	}


	bool CUserCryptoManager::LoadPasswordMode(CommonLib::FxMemoryReadStream *pStream)
	{

		m_qryptoAlg = (QryptoALG)pStream->readintu16();
		pStream->read(m_szSalt, ___nSALT_SIZE);
		pStream->read(m_szSaltIV, ___nSALT_SIZE);
		m_nPWDPage = pStream->readInt64();

		m_PageChiper.reset(new CPageCipher(m_qryptoAlg));
		m_pCheckPageChiper.reset(new CPageCipher(m_qryptoAlg));
		return true;
	}

	bool CUserCryptoManager::CheckPassword(const wchar_t *pszPassword)
	{
		CommonLib::CString sPWD = pszPassword;


		std::vector<byte> vecUtf8PWD;
		uint32 nUtf8Len = sPWD.calcUTF8Length();
		vecUtf8PWD.resize(nUtf8Len + 1);
		sPWD.exportToUTF8((char*)&vecUtf8PWD[0], nUtf8Len);



		IDBStoragePtr pDBStorage = m_pDB->getDBStorage();
		
		
		m_pCheckPageChiper->SetKey(&vecUtf8PWD[0], nUtf8Len, m_szSalt, m_szSaltIV, ___nSALT_SIZE);


		byte buf[MIN_PAGE_SIZE];
		bool bRet = pDBStorage->ReadRowData(buf, MIN_PAGE_SIZE,  m_nPWDPage * MIN_PAGE_SIZE);
		if(!bRet)
			return false;

		CFilePage pwdPage(buf, MIN_PAGE_SIZE, m_nPWDPage);
		m_pCheckPageChiper->decrypt(&pwdPage);

		CommonLib::FxMemoryReadStream stream;
		stream.attachBuffer(pwdPage.getRowData(), pwdPage.getPageSize());
		sFilePageHeader header(stream, pwdPage.getPageSize(), true, false);
		if(!header.isValid())
			return false; 

		uint32 nOffset = CalcOffset(&vecUtf8PWD[0], nUtf8Len, m_szSalt, ___nSALT_SIZE);

		if(m_bPWDCheck)
		{
			
			if(nOffset != m_nOffset)
				return false;
			return true;
		}
		else
		{
			 if(nOffset > pDBStorage->getFileSize())
				 return false;

			 m_nOffset = nOffset;
			
			 m_PageChiper->SetKey(&vecUtf8PWD[0], nUtf8Len, m_szSalt, m_szSaltIV, ___nSALT_SIZE);
			 ((CStorage*)pDBStorage.get())->setPageChiper(m_PageChiper.get());
			 pDBStorage->SetOffset(m_nOffset);
			 m_bPWDCheck = true;
		}

		return true;
	}



	uint64 CUserCryptoManager::GetUserID(const wchar_t *pszUser)
	{
		if(m_UserMode == eNoneMode || m_UserMode == ePasswordMode)
			return 0;

		return 0;
	}
	eUserGroup CUserCryptoManager::GetUserGroup(const wchar_t *pszUser)
	{
		if(m_UserMode == eNoneMode || m_UserMode == ePasswordMode)
			return ugAdmin;
		return ugAdmin;
		
	}
}
