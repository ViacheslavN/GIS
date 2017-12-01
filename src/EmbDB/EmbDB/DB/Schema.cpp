#include "stdafx.h"
#include "Schema.h"
#include "DBMagicSymbol.h"
#include "Database.h"
#include "Table/Table.h"
#include "storage/storage.h"
#include "Utils/streams/WriteStreamPage.h"
#include "Utils/streams/ReadStreamPage.h"
namespace embDB
{
	CSchema::CSchema(CDatabase *pDB) : m_pDB(pDB), m_pStorage(NULL), 
		m_nAddr(-1), m_nTablesPage(-1), m_nPageSize(MIN_PAGE_SIZE)
	{

	}
	CSchema::~CSchema()
	{

	}
	bool CSchema::open(IDBStorage* pStorage, int64 nFileAddr, bool bNew)
	{
		m_pStorage = pStorage;
		m_nAddr = nFileAddr;
		if(bNew)
		{
			return save(NULL);
		}
		return LoadSchema();
	}
	bool CSchema::close()
	{
		return true;
	}
	bool CSchema::addTable(const wchar_t*  pszTableName, ITransaction *pTran )
	{
		TTablesByName::iterator it = m_TablesByName.find(pszTableName);
		if(it != m_TablesByName.end())
			return false;


		IDBTransactionPtr pDBTran((IDBTransaction *)pTran);

		if(!pDBTran.get())
		{
			return false;
		}
	 	FilePagePtr pPage = pDBTran->getNewPage(nTableInfoPageSize);
		if(!pPage.get())
			return false;
		CTable* pTable = new CTable(m_pDB, pPage.get(), pszTableName/*, m_nLastTableID++*/);
		ITablePtr pTablePtr(pTable);

		int64 nPageAddr = pPage->getAddr();
		pPage.release();
		if(!pTable->save(pDBTran.get()))
			return false;
		m_TablesByName.insert(std::make_pair(pszTableName, pTablePtr));
		m_TablesByID.insert(std::make_pair(pTable->getAddr(), pTablePtr));
		m_vecTables.push_back(pTablePtr);
		 
		m_vecTablesAddr.push_back(nPageAddr);
		return SaveTablesAddr(pDBTran.get());
		 
	}
	
	bool CSchema::LoadSchema()
	{

		FilePagePtr pPage(m_pStorage->getFilePage(m_nAddr, m_nPageSize));
		if(!pPage.get())
			return false;

		CommonLib::FxMemoryReadStream stream;
		stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());
		sFilePageHeader header(stream, pPage->getPageSize(), m_pDB->getCheckCRC());
		if(m_pDB->getCheckCRC() && !header.isValid())
		{
			//TO DO Error LOg
			return false;
		}
		if(header.m_nObjectPageType != SCHEMA_PAGE || header.m_nSubObjectPageType != SCHEMA_ROOT_PAGE)
		{
			//TO DO Error LOg
			return false;
		}
		/*int64 nSymbol = stream.readInt64();
		if(nSymbol != DB_SCHEMA_SYMBOL)
			return false;*/
		int64 nPrev = stream.readInt64();
		int64 nNext = stream.readInt64();
		m_nTablesPage = stream.readInt64(); 
		if(m_nTablesPage == -1)
			return true;

		if (!LoadTablesAddr())
			return false;

		for (size_t i = 0; i < m_vecTablesAddr.size(); ++i)
		{
			CTable* pTable = new CTable(m_pDB, m_vecTablesAddr[i]);
			if (!pTable->load())
			{
				delete pTable;
				return false;
			}
			m_TablesByName.insert(std::make_pair(pTable->getName(), ITablePtr(pTable)));
			m_TablesByID.insert(std::make_pair(pTable->getAddr(), ITablePtr(pTable)));
			m_vecTables.push_back(ITablePtr(pTable));
		}
		return true;
	}

	bool CSchema::saveHead(IDBTransaction *pTran)
	{
		FilePagePtr pPage(pTran ? pTran->getFilePage(m_nAddr, m_nPageSize) : m_pStorage->getFilePage(m_nAddr, m_nPageSize));
		if(!pPage.get())
			return false;

 
		CommonLib::FxMemoryWriteStream stream;
		stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());
		sFilePageHeader header(stream, SCHEMA_PAGE, SCHEMA_ROOT_PAGE, pPage->getPageSize(), m_pDB->getCheckCRC());
		stream.write((int64)-1); //next
		stream.write((int64)-1); //prev
		stream.write(m_nTablesPage); //TablePage
		header.writeCRC32(stream);
		
		return SaveTablesAddr(pTran);
	}
	bool CSchema::save(IDBTransaction *pTran)
	{
		if(!pTran && !m_pStorage)
			return false;
 
		return saveHead(pTran);
	}

	bool CSchema::dropTable(ITable *pTable, ITransaction *pTran)
	{
		assert(pTable);
		if(!pTable || !pTran)
			return false;

		IDBTransaction* pDBTran = (IDBTransaction*)pTran;

		TTables::iterator it = std::find(m_vecTables.begin(), m_vecTables.end(), ITablePtr(pTable));
		assert(it == m_vecTables.end());
		if (it == m_vecTables.end())
			return false;
		 
	 

	    CTable *pDBTable = (CTable*)pTable;

		pDBTable->lock(); //TO DO add in wait graf
		if(!pDBTable->isCanBeRemoving())
		{
			pDBTable->unlock();
			pDBTran->error(L""); //TO DO Error msg
			return false;
		}
 
		
		auto it_addr = std::find(m_vecTablesAddr.begin(), m_vecTablesAddr.end(), pDBTable->getAddr());
		m_vecTablesAddr.erase(it_addr);
		

		m_TablesByName.erase(pDBTable->getName());
		m_TablesByID.erase(pDBTable->getAddr());
		m_vecTables.erase(it);

		SaveTablesAddr(pDBTran);
 
		return true;
	}

	bool CSchema::dropTable(const CommonLib::CString& sTableName, ITransaction *Tran)
	{
		TTablesByName::iterator it = m_TablesByName.find(sTableName);
		if(it == m_TablesByName.end())
			return false;
		return dropTable(it->second.get(), Tran);
	}
	bool CSchema::dropTable(int64 nID, ITransaction *Tran)
	{
		TTablesByID::iterator it = m_TablesByID.find(nID);
		if(it == m_TablesByID.end())
			return false;
		return dropTable(it->second.get(), Tran);
	}

	uint32 CSchema::getTableCnt() const
	{
		return (uint32)m_vecTables.size();
	}
	ITablePtr CSchema::getTable(uint32 nIndex) const
	{
		if(nIndex < m_vecTables.size())
			return m_vecTables[nIndex];
		return ITablePtr();
	}
	ITablePtr CSchema::getTableByName(const wchar_t* pszTableName) const
	{

		TTablesByName::const_iterator it = m_TablesByName.find(pszTableName);
		if(it != m_TablesByName.end())
			return it->second;
		return ITablePtr();
	}
	ITablePtr CSchema::getTableByID(int64 nID) const
	{
		TTablesByID::const_iterator it = m_TablesByID.find(nID);
		if(it != m_TablesByID.end())
			return it->second;
		return ITablePtr();
	}


	bool CSchema::LoadTablesAddr()
	{
		ReadStreamPage stream(m_pStorage, PAGE_SIZE_8K, true, 0, 0);
		if (!stream.open(m_nTablesPage, 0))
			return false;
		uint32 nTable = stream.readIntu32();
		for (uint32 i = 0; i < nTable; ++i)
			m_vecTablesAddr.push_back(stream.readIntu64());

		return true;

	}
	bool CSchema::SaveTablesAddr(IDBTransaction *pTran)
	{
		WriteStreamPage stream(pTran ? (IFilePage*)pTran : (IFilePage*)m_pStorage, true, 0, 0);
		 
		if (m_nTablesPage == -1)
		{
			auto pPage = m_pStorage->getNewPage();
			if (pPage.get())
				return false;

			m_nTablesPage = pPage->getAddr();
			stream.open(pPage, 0, false);
		}
		else
		{
			if (!stream.open(m_nTablesPage, 0, true))
				return false;
		}

		stream.write(uint32(m_vecTablesAddr.size()));

		for (size_t i = 0; i < m_vecTablesAddr.size(); ++i)
			stream.write(m_vecTablesAddr[i]);
		stream.Save();
		return true;
	}
 

}