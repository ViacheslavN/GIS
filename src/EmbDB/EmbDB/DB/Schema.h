#ifndef _EMBEDDED_DATABASE_SCHEMA_H_
#define _EMBEDDED_DATABASE_SCHEMA_H_
#include "stdafx.h"
#include "CommonLibrary/String.h"
#include "CommonLibrary/stream.h"
#include "../embDBInternal.h"
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
			virtual bool open(IDBStorage* pStorage, int64 nFileAddr, bool bNew = false);
			virtual bool close();
			virtual bool addTable(const wchar_t*  pszTableName, ITransaction *Tran);
 			virtual bool save(IDBTransaction *pTran);
			virtual uint32 getTableCnt() const;
			virtual ITablePtr getTable(uint32 nIndex) const;
			virtual ITablePtr getTableByName(const wchar_t* pszTableName) const;
			virtual ITablePtr getTableByID(int64 nID) const;


			virtual bool dropTable(const CommonLib::CString& sTableName, ITransaction *Tran );
			virtual bool dropTable(int64 nID, ITransaction *Tran );
			virtual bool dropTable(ITable *pTable, ITransaction *Tran );

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
			virtual uint32 getLinkCnt() const
			{
				return 0;
			}
			virtual ILinkPtr getLink(uint32 nIndex) const
			{
				return ILinkPtr();
			}


		private:
			static const uint32 nTableListPageSize = MIN_PAGE_SIZE;
			static const uint32 nTableInfoPageSize = MIN_PAGE_SIZE;

			bool LoadSchema();
			bool saveHead(IDBTransaction *Tran);
	 
		
			bool LoadTablesAddr();
			bool SaveTablesAddr(IDBTransaction *Tran);
		
		private:
			IDBStorage* m_pStorage;
			typedef std::map<CommonLib::CString, ITablePtr> TTablesByName;
			typedef std::map<int64, ITablePtr> TTablesByID;
			typedef std::vector<ITablePtr> TTables;
			typedef std::vector<int64> TTablePages;

			TTablesByName m_TablesByName;
			TTablesByID	  m_TablesByID;
			TTables       m_vecTables;
			//TTableVector  m_Tables;
			int64 m_nAddr;
			int64 m_nTablesPage;
			CDatabase* m_pDB;
			TTablePages m_vecTablesAddr;
			uint32 m_nPageSize;
			//int64 m_nLastTableID;


	};

}
#endif