#ifndef _EMBEDDED_DATABASE_DB_H_
#define _EMBEDDED_DATABASE_DB_H_


#include "Schema.h"
#include "CommonLibrary/String.h"
#include "../embDBInternal.h"

#include "UserCryptoManager.h"
namespace embDB
{
	class CFilePage;
	class CStorage;
	class CDBTranManager;
	class Connection;
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
		int64  nUserCryptoManager;
		sDBHeader(): nMagicSymbol(0), nMajorVersion(0), nMinorVersion(0), qryptoAlg(AES128), bCheckCRC(false),
			bCheckPWD(false), nRootPage(-1), nPWDPage(-1), nUserCryptoManager(-1)
		{
			memset(szSalt, 0, sizeof(szSalt));
			memset(szSaltIV, 0, sizeof(szSaltIV));
		}


		void Read(CommonLib::IReadStream* pStream)
		{
			nMagicSymbol = pStream->readInt64();
			nMajorVersion = pStream->readIntu32();
			nMinorVersion = pStream->readIntu32();
			qryptoAlg = (QryptoALG)pStream->readIntu32();
	 
			nRootPage = pStream->readInt64();
			nUserCryptoManager= pStream->readInt64();
		
		}
		void Write(CommonLib::IWriteStream* pStream)
		{
			pStream->write(nMagicSymbol);
			pStream->write(nMajorVersion);
			pStream->write(nMinorVersion);
			pStream->write((uint32)qryptoAlg);
			pStream->write(nRootPage);
			pStream->write(nUserCryptoManager);
		}

	};
 

	struct sDBRootPage
	{
		sDBRootPage() :  nShemaPage(-1), nStoragePage(-1),   nOffcet(0)
		{}
		~sDBRootPage(){}
		int64 nOffcet;

		//int nPageSize;
		int64  nShemaPage;
		int64  nStoragePage;
	 

	


		void Read(CommonLib::IReadStream* pStream)
		{

 
			nOffcet = pStream->readInt64();
			nShemaPage = pStream->readInt64();
			nStoragePage = pStream->readInt64();
			 
		}
		void Write(CommonLib::IWriteStream* pStream)
		{
			pStream->write(nOffcet);
			pStream->write(nShemaPage);
			pStream->write(nStoragePage);
 
		}

	};
	class CDatabase : public IDBDatabase
	{
		public:
			CDatabase(eLogMode logMode = lmUndefined, const wchar_t* pszLogName = NULL, DBTranlogMode TranLogMode = eTranExclusiveLog);
			~CDatabase();
			virtual bool open(const wchar_t* pszName, DBTransactionMode mode = eTMMultiTransactions, const wchar_t* pszWorkingPath = NULL);
			virtual bool create(const wchar_t* pszDbName,  DBTransactionMode mode = eTMMultiTransactions,
				const wchar_t* pszWorkingPath = NULL,  const wchar_t* pszPassword = NULL, const SDBParams *Params = NULL) ;
			virtual bool create(const wchar_t* pszDbName,const wchar_t* pszAdmUser, const wchar_t* pszPassword , DBTransactionMode mode = eTMMultiTransactions, 
				const wchar_t* pszWorkingPath = NULL, const SDBParams *Params = NULL );
			

			virtual IConnectionPtr connect(const wchar_t* pszUser= NULL, const wchar_t* pszPassword = NULL);
			virtual bool closeConnection(IConnection *pConnection);
			
			
			virtual bool close();

 

			virtual void setLogLevel(uint32 nLevel);
			virtual uint32 getLogLevel();
			virtual eLogMode getLogMode() const;
		
			virtual IDBStoragePtr getDBStorage() const  {return m_pStorage;}

			//CStorage* getMainStorage();
			//CStorage* getTableStorage(const CommonLib::CString& sFileName, bool bCreate);

			CommonLib::alloc_t* getBTreeAlloc();
			CommonLib::alloc_t* getCommonAlloc();
			virtual bool getCheckCRC() const {return m_DBParams.bCheckCRC;}
			bool getCheckPageType() const {return !m_DBParams.qryptoAlg == NONE_ALG;}
		private:
			ITransactionPtr startTransaction(eTransactionDataType trType, uint64 nUserID, IDBConnection *pConn, eDBTransationType trDbType = eTTFullTransaction);
			bool closeTransaction(ITransaction* pTran, uint64 nUserID);
			bool load();
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
			bool m_bLoad;
			SDBParams m_DBParams;
			std::auto_ptr<CPageCipher> m_PageChiper;
			CUserCryptoManager m_UserCryptoManager;

			friend class CConnection;
			uint32 m_nLogLevel;

			DBTransactionMode m_TranMode;

			ILoggerPtr m_pLogger;
			eLogMode m_LogMode;
			CommonLib::CString m_sFileLogPath;


			typedef std::set<IConnectionPtr> TConnections;
			TConnections m_Connections;

			DBTranlogMode m_TranLogMode;

		//	typedef RBMap<CommonLib::CString, CStorage*> TTableStorages;
		//	TTableStorages m_TableStorages;
			 
			
			
	};
}

#endif