#include "stdafx.h"
#include "Schema.h"
#include "DBMagicSymbol.h"
#include "Database.h"
#include "Table.h"
#include "storage.h"
namespace embDB
{
	CSchema::CSchema(CDatabase *pDB) : m_pDB(pDB), m_pStorage(NULL), 
		m_nAddr(-1), m_nTablesPage(-1), 
		m_nTablesAddr(-1, 0, SCHEMA_PAGE, SCHEMA_TABLE_LIST_PAGE), m_nPageSize(MIN_PAGE_SIZE)
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

		IDBTransactionPtr pInnerTran;
		if(!pDBTran.get())
		{
			 pInnerTran =  (IDBTransaction*)m_pDB->startTransaction(eTT_DDL).get();
			 pDBTran = pInnerTran;
		}
	
		if(pInnerTran.get())
		{
			if(!pInnerTran->begin())
			{
				m_pDB->closeTransaction(pInnerTran.get());
				return false;
			}
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
		 
		if(!m_nTablesAddr.push(nPageAddr, pDBTran.get()))
		{
			if(pInnerTran.get())
			{
				pInnerTran->rollback();
				m_pDB->closeTransaction(pInnerTran.get());
			}
			return false;
		}
		if(pInnerTran.get())
		{
			pInnerTran->commit();
			m_pDB->closeTransaction(pInnerTran.get());
		}
		return true;
	}
	
	bool CSchema::LoadSchema()
	{

		FilePagePtr pPage(m_pStorage->getFilePage(m_nAddr, m_nPageSize));
		if(!pPage.get())
			return false;

		CommonLib::FxMemoryReadStream stream;
		stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());
		sFilePageHeader header(stream, pPage->getPageSize());
		if(!header.isValid())
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
		m_nTablesAddr.setFirstPage(m_nTablesPage);
		m_nTablesAddr.setPageSize(nTableListPageSize/*m_pStorage->getPageSize()*/);
		m_nTablesAddr.load(m_pStorage);
		TTablePages::iterator it = m_nTablesAddr.begin();
		while(!it.isNull())
		{
			CTable* pTable = new CTable(m_pDB, it.value());
			if(!pTable->load())
			{
				delete pTable;
				return false;
			}
			m_TablesByName.insert(std::make_pair(pTable->getName(), ITablePtr(pTable)));
			m_TablesByID.insert(std::make_pair(pTable->getAddr(), ITablePtr(pTable)));
			m_vecTables.push_back(ITablePtr(pTable));
			it.next();
		}

		/*ListDBPage<CStorage> listDB(m_pStorage, DB_SCHEMA_TABLE_SYMBOL);
		if(!listDB.load(&stream, m_nTablesPage))
			return false;
		if(!readTablePage(&stream))
			return false;
		while(listDB.next(&stream))
		{
			if(!readTablePage(&stream))
				return false;
		}*/
		return true;
	}
	bool CSchema::readTablePage(CommonLib::IReadStream* pStream)
	{
		uint32 nSize = pStream->readInt32();
		if(nSize == 0)
			return true;
		//int nPageSize = m_pStorage->getPageSize();
		//if(nSize > (nPageSize/sizeof(int64)) - 2)
		//	return false;

		for(uint32 i = 0; i < nSize; ++i)
		{
			int64 nPageAddr = pStream->readInt64();
			
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
		sFilePageHeader header(stream, SCHEMA_PAGE, SCHEMA_ROOT_PAGE, pPage->getPageSize());
		stream.write((int64)-1); //next
		stream.write((int64)-1); //prev
		stream.write(m_nTablesPage); //TablePage
		header.writeCRC32(stream);
		if(pTran)
		{
			pPage->setFlag(eFP_CHANGE, true);
			pTran->saveFilePage(pPage);
			m_nTablesAddr.save(pTran);
		}
		else		
		{
			m_nTablesAddr.save(m_pStorage);
			m_pStorage->saveFilePage(pPage);
		}
		return true;
	}
	bool CSchema::save(IDBTransaction *pTran)
	{
		if(!pTran && !m_pStorage)
			return false;

		if(m_nTablesPage == -1)
		{
			FilePagePtr pPage (pTran ?  pTran->getNewPage(m_nPageSize) : m_pStorage->getNewPage(m_nPageSize));
			if(!pPage.get())
				return false;
			 m_nTablesPage = pPage->getAddr();
			 m_nTablesAddr.setFirstPage(m_nTablesPage);
			 m_nTablesAddr.setPageSize(pPage->getPageSize());
			if(!saveHead(pTran))
				return false;
		}
		if(pTran)
			return m_nTablesAddr.save(pTran);
		else
			return m_nTablesAddr.save(m_pStorage);
	}

	bool CSchema::dropTable(ITable *pTable, ITransaction *pTran)
	{
		assert(pTable);
		if(!pTable)
			return false;

		IDBTransaction* pDBTran = (IDBTransaction*)pTran;

		TTables::iterator it = std::find(m_vecTables.begin(), m_vecTables.end(), ITablePtr(pTable));
		assert(it == m_vecTables.end());
		if(it == m_vecTables.end())
			return false;
	 

	    CTable *pDBTable = (CTable*)pTable;

		pDBTable->lock(); //TO DO add in wait graf
		if(!pDBTable->isCanBeRemoving())
		{
			pDBTable->unlock();
			pDBTran->error(L""); //TO DO Error msg
			return false;
		}
		if(!m_nTablesAddr.remove(pDBTable->getAddr(), pDBTran))
			return false;

		m_TablesByName.erase(pDBTable->getName());
		m_TablesByID.erase(pDBTable->getAddr());
		m_vecTables.erase(it);

 
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
		return m_vecTables.size();
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

 

}