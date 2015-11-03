#ifndef _EMBEDDED_DATABASE_SCHEMA_H_
#define _EMBEDDED_DATABASE_SCHEMA_H_
#include "stdafx.h"
//#include "RBMap.h"
#include "CommonLibrary/String.h"
#include "CommonLibrary/stream.h"
#include "PageVector.h"
#include "embDBInternal.h"
#include <map>
namespace embDB
{
	class CDatabase;
	class CTable;
	class CStorage;
	struct IDBTransaction;
	class CSchema : IDBShema
	{
		public:
			CSchema(CDatabase *pDB);
			~CSchema();
			virtual bool open(IDBStorage* pStorage, __int64 nFileAddr, bool bNew = false);
			virtual bool close();
			virtual bool addTable(const wchar_t*  pszTableName, ITransaction *Tran = NULL);
 			virtual bool save(IDBTransaction *pTran);
			virtual size_t getTableCnt() const;
			virtual ITablePtr getTable(size_t nIndex) const;
			virtual ITablePtr getTableByName(const wchar_t* pszTableName) const;
			virtual ITablePtr getTableByID(int64 nID) const;


			virtual bool dropTable(const CommonLib::CString& sTableName, ITransaction *Tran = NULL);
			virtual bool dropTable(int64 nID, ITransaction *Tran = NULL);
			virtual bool dropTable(ITable *pTable, ITransaction *Tran = NULL);

			uint32 getTableCount();
			
			


			virtual ILinkPtr AddLink(ITable* pTableFrom, IField* pFieldFrom, ITable* pTableTo, IField* pFieldTo,  eLinkType nLinkType)
			{
				return ILinkPtr();
			}
			virtual ILinkPtr AddLink(const CommonLib::CString& sTableFrom, const CommonLib::CString& sFieldFrom, const CommonLib::CString& sTableTo, const CommonLib::CString& sFieldTo,  eLinkType nLinkType)
			{
				return ILinkPtr();
			}
			virtual bool deleteLink(ILink *pLink)
			{
				return false;
			}
			virtual size_t getLinkCnt() const
			{
				return 0;
			}
			virtual ILinkPtr getLink(size_t nIndex) const
			{
				return ILinkPtr();
			}


		private:
			bool LoadSchema();
			bool saveHead(IDBTransaction *Tran);
			bool readTablePage(CommonLib::IReadStream* pStream);
		
		
		private:
			IDBStorage* m_pStorage;
			typedef std::map<CommonLib::CString, ITablePtr> TTablesByName;
			typedef std::map<int64, ITablePtr> TTablesByID;
			typedef std::vector<ITablePtr> TTables;
			typedef TPageVector<int64> TTablePages;

			TTablesByName m_TablesByName;
			TTablesByID	  m_TablesByID;
			TTables       m_vecTables;
			//TTableVector  m_Tables;
			int64 m_nAddr;
			int64 m_nTablesPage;
			CDatabase* m_pDB;
			TTablePages m_nTablesAddr;
			//int64 m_nLastTableID;


	};

}
#endif