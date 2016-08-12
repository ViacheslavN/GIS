#ifndef _EMBEDDED_DATABASE_DB_H_
#define _EMBEDDED_DATABASE_DB_H_


#include "Schema.h"
#include "CommonLibrary/String.h"
#include "DBConfig.h"
#include "embDBInternal.h"
#include "PageCipher.h"
namespace embDB
{
	class CFilePage;
	class CStorage;
	class CDBTranManager;
	struct sDBHeader
	{
		enum
		{
			SALT_SIZE = 16 
		};


		int64  nMagicSymbol;
		uint32 nMajorVersion;
		uint32 nMinorVersion;
		QryptoALG qryptoAlg;
		bool bCheckPWD;
		bool bCheckCRC;
		bool bCheckPage;
		byte szSalt[SALT_SIZE];
		byte szSaltIV[SALT_SIZE];
		int64  nRootPage;
		int64  nPWDPage;
		sDBHeader(): nMagicSymbol(0), nMajorVersion(0), nMinorVersion(0), qryptoAlg(AES128), bCheckCRC(false),
			bCheckPWD(false), nRootPage(-1), nPWDPage(-1)
		{
			memset(szSalt, 0, sizeof(szSalt));
			memset(szSaltIV, 0, sizeof(szSaltIV));
		}


		void Read(CommonLib::IReadStream* pStream)
		{
			pStream->read(szSalt, SALT_SIZE);
			pStream->read(szSaltIV, SALT_SIZE);
			nMagicSymbol = pStream->readInt64();
			nMajorVersion = pStream->readIntu32();
			nMinorVersion = pStream->readIntu32();
			qryptoAlg = (QryptoALG)pStream->readIntu32();
			bCheckPWD = pStream->readBool();
			nRootPage = pStream->readInt64();
			nPWDPage= pStream->readInt64();
		
		}
		void Write(CommonLib::IWriteStream* pStream)
		{
			pStream->write(szSalt, SALT_SIZE);
			pStream->write(szSaltIV, SALT_SIZE);
			pStream->write(nMagicSymbol);
			pStream->write(nMajorVersion);
			pStream->write(nMinorVersion);
			pStream->write((uint32)qryptoAlg);
			pStream->write(bCheckPWD);
			pStream->write(nRootPage);
			pStream->write(nPWDPage);
		}

	};
 

	struct sDBRootPage
	{
		sDBRootPage() :  nShemaPage(-1), nStoragePage(-1), nUserPage(-1), nOffcet(0)
		{}
		~sDBRootPage(){}
		int64 nOffcet;

		//int nPageSize;
		int64  nShemaPage;
		int64  nStoragePage;
		int64  nUserPage;
	


		void Read(CommonLib::IReadStream* pStream)
		{

 
			nOffcet = pStream->readInt64();
			nShemaPage = pStream->readInt64();
			nStoragePage = pStream->readInt64();
			nUserPage = pStream->readInt64();
		}
		void Write(CommonLib::IWriteStream* pStream)
		{
			pStream->write(nOffcet);
			pStream->write(nShemaPage);
			pStream->write(nStoragePage);
			pStream->write(nUserPage);
		}

	};
	class CDatabase : public IDBDatabase
	{
		public:
			CDatabase();
			~CDatabase();
			virtual bool open(const wchar_t* pszName, DBTransactionMode mode = eTMMultiTransactions, const wchar_t* pszWorkingPath = NULL, const wchar_t* pszPassword = NULL);
			virtual bool create(const wchar_t* pszDbName,  DBTransactionMode mode = eTMMultiTransactions,
				const wchar_t* pszWorkingPath = NULL, const wchar_t* pszPassword = NULL, const SDBParams *Params = NULL) ;
			virtual bool close();
			virtual ITransactionPtr startTransaction(eTransactionType trType);
			virtual bool closeTransaction(ITransaction* );
			virtual ISchemaPtr getSchema() const {return ISchemaPtr(m_pSchema.get());}
			virtual IDBStoragePtr getDBStorage() const  {return m_pStorage;}

			//CStorage* getMainStorage();
			//CStorage* getTableStorage(const CommonLib::CString& sFileName, bool bCreate);

			CommonLib::alloc_t* getBTreeAlloc();
			bool getCheckCRC() const {return m_DBParams.bCheckCRC;}
			bool getCheckPageType() const {return !m_DBParams.qryptoAlg == NONE_ALG;}
		private:
			bool readHeadPage(CFilePage* pPage);
			bool readRootPage(CFilePage* pPage);
			bool CheckDirty();
			int64 InitCrypto(byte* pPWD, uint32 nLen);
			int64 CalcOffset(const byte* pPWD, uint32 nLen, const byte* pSlat, uint32 nSaltLen) const;
			void CreateNoise(int64 nSize);
			void CreateCheckPWDPage();
		private:
			std::auto_ptr<CommonLib::alloc_t > m_pAlloc;
			IDBStoragePtr m_pStorage;
			IDBShemaPtr m_pSchema;
			sDBHeader m_dbHeader;
			sDBRootPage m_dbRootPage;
			std::auto_ptr<CDBTranManager>  m_pTranManager;
			bool m_bOpen;
			SDBParams m_DBParams;
			std::auto_ptr<CPageCipher> m_PageChiper;
		//	typedef RBMap<CommonLib::CString, CStorage*> TTableStorages;
		//	TTableStorages m_TableStorages;
			 
			
			
	};
}

#endif