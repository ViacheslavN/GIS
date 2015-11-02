#ifndef _EMBEDDED_DATABASE_SCHEMA_H_
#define _EMBEDDED_DATABASE_SCHEMA_H_
#include "stdafx.h"
#include "RBMap.h"
#include "CommonLibrary/String.h"
#include "CommonLibrary/stream.h"
#include "PageVector.h"
namespace embDB
{
	class CDatabase;
	class CTable;
	class CStorage;
	struct IDBTransaction;
	class CSchema
	{
		public:
			CSchema(CDatabase *pDB);
			~CSchema();
			bool open(CStorage* pStorage, __int64 nFileAddr, bool bNew = false);
			bool close();
			bool addTable(const CommonLib::CString& sTableName, const CommonLib::CString& sStorageName, IDBTransaction *Tran = NULL);
			CTable* getTable(const CommonLib::CString& sTableName);
			CTable* getTable(int64 nID);

			bool dropTable(const CommonLib::CString& sTableName, IDBTransaction *Tran = NULL);
			bool dropTable(int64 nID, IDBTransaction *Tran = NULL);
			bool dropTable(CTable *pTable, IDBTransaction *Tran = NULL);

			uint32 getTableCount();
			
			bool save(IDBTransaction *pTran);
		private:
			bool LoadSchema();
			bool saveHead(IDBTransaction *Tran);
			bool readTablePage(CommonLib::IReadStream* pStream);
		
		private:
			CStorage* m_pStorage;
			typedef RBMap<CommonLib::CString, CTable*> TTablesByName;
			typedef RBMap<int64, CTable*> TTablesByID;
			typedef std::vector<CTable*> TTables;
			typedef TPageVector<int64> TTablePages;

			TTablesByName m_TablesByName;
			TTablesByID	  m_TablesByID;
			//TTableVector  m_Tables;
			int64 m_nAddr;
			int64 m_nTablesPage;
			CDatabase* m_pDB;
			TTablePages m_nTablesAddr;
			//int64 m_nLastTableID;


	};

}
#endif