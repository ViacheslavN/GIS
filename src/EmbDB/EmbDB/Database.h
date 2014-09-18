#ifndef _EMBEDDED_DATABASE_DB_H_
#define _EMBEDDED_DATABASE_DB_H_


#include "Schema.h"
#include "CommonLibrary/str_t.h"
#include "DBConfig.h"
#include "ITransactions.h"
#include "IDatabase.h"

namespace embDB
{
	class CFilePage;
	class CStorage;
	class CDBTranManager;
	struct sDBHeader
	{
		sDBHeader() : nMagicSymbol(-1), nCRC(-1), nVersion(-1), nPageSize(DEFAULT_PAGE_SIZE), nShemaPage(-1), nStoragePage(-1), nUserPage(-1)
		{}
		~sDBHeader(){}
		int64  nMagicSymbol;
		int nCRC;
		int nVersion;
		int nPageSize;
		int64  nShemaPage;
		int64  nStoragePage;
		int64  nUserPage;


		void Read(CommonLib::IReadStream* pStream)
		{
			nMagicSymbol = pStream->readInt64();
			nCRC = pStream->readInt32();
			nVersion= pStream->readInt32();
			nPageSize = pStream->readInt32();
			nShemaPage = pStream->readInt64();
			nStoragePage = pStream->readInt64();
			nUserPage = pStream->readInt64();
		}
		void Write(CommonLib::IWriteStream* pStream)
		{
			pStream->write(nMagicSymbol);
			pStream->write(nCRC);
			pStream->write(nVersion);
			pStream->write(nPageSize);
			pStream->write(nShemaPage);
			pStream->write(nStoragePage);
			pStream->write(nUserPage);
		}

	};
	class CDatabase : public IDatabase
	{
		public:
			CDatabase();
			~CDatabase();
			virtual bool open(const CommonLib::str_t& sDbName, const CommonLib::str_t& sWorkingPath = "");
			virtual bool create(const CommonLib::str_t& sDbName, size_t nPageSize, const CommonLib::str_t& sWorkingPath = "");
			virtual bool close();
			virtual ITransactions* startTransaction(eTransactionsType trType);
			virtual bool closeTransaction(ITransactions* );

			CSchema* getSchema();
			CStorage* getMainStorage();
			CStorage* getTableStorage(const CommonLib::str_t& sFileName, bool bCreate);

			CommonLib::alloc_t* getBTreeAlloc();
				
		private:
			bool readRootPage(CFilePage* pPage);
			bool CheckDirty();
		private:
			std::auto_ptr<CommonLib::alloc_t > m_pAlloc;
			std::auto_ptr<CStorage> m_pStorage;
			CSchema m_schema;
			sDBHeader m_dbHeader;
			std::auto_ptr<CDBTranManager>  m_pTranManager;
			bool m_bOpen;
			typedef RBMap<CommonLib::str_t, CStorage*> TTableStorages;
			TTableStorages m_TableStorages;
			 
			
			
	};
}

#endif