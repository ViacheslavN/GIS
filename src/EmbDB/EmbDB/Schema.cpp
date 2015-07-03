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
		m_nTablesAddr(-1, 0, SCHEMA_PAGE, SCHEMA_TABLE_LIST_PAGE)
	{

	}
	CSchema::~CSchema()
	{

	}
	bool CSchema::open(CStorage* pStorage, __int64 nFileAddr, bool bNew)
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
	bool CSchema::addTable(const CommonLib::str_t& sTableName, const CommonLib::str_t& sStorageName, IDBTransactions *pTran )
	{
		TTablesByName::iterator it = m_TablesByName.find(sTableName);
		if(!it.isNull())
			return false;

		IDBTransactions *pInnerTran = 0;
		if(!pTran)
		{
			 pInnerTran =  (IDBTransactions*)m_pDB->startTransaction(eTT_DDL);
			 pTran = pInnerTran;
		}
	
		CStorage *pTradeStorage = NULL;
		if(!sStorageName.isEmpty())
		{
			pTradeStorage = m_pDB->getTableStorage(sStorageName, true);
			if(!pTradeStorage)
			{
				//TO DO Loggin
				return false;
			}
			//pTran->setDBStorage(pTradeStorage);
		}
		if(pInnerTran)
		{
			if(!pInnerTran->begin())
			{
				m_pDB->closeTransaction(pInnerTran);
				return false;
			}
		}
		
		FilePagePtr pPage = pTran->getNewPage();
		if(!pPage.get())
			return false;
		CTable* pTable = new CTable(m_pDB, pPage.get(), sTableName, pTradeStorage/*, m_nLastTableID++*/);
		if(!pTable->save(pTran))
			return false;
		m_TablesByName.insert(sTableName, pTable);
		bool bRet = !m_TablesByID.isNull( m_TablesByID.insert(pTable->getAddr(), pTable));
		assert(bRet);
		if(!bRet)
		{
			return false;
		}
		if(!m_nTablesAddr.push(pPage->getAddr(), pTran))
		{
			if(pInnerTran)
			{
				pInnerTran->rollback();
				m_pDB->closeTransaction(pInnerTran);
			}
			return false;
		}
		if(pInnerTran)
		{
			pInnerTran->commit();
			m_pDB->closeTransaction(pInnerTran);
		}
		return true;
	}
	CTable* CSchema::getTable(const CommonLib::str_t& sTableName)
	{
		TTablesByName::iterator it = m_TablesByName.find(sTableName);
		if(!it.isNull())
			return it.value();
		return NULL;
	}
	CTable* CSchema::getTable(int64 nID)
	{
		TTablesByID::iterator it = m_TablesByID.find(nID);
		if(!it.isNull())
			return it.value();
		return NULL;
	}
	bool CSchema::LoadSchema()
	{

		FilePagePtr pPage(m_pStorage->getFilePage(m_nAddr));
		if(!pPage.get())
			return false;

		CommonLib::FxMemoryReadStream stream;
		stream.attach(pPage->getRowData(), pPage->getPageSize());
		sFilePageHeader header(stream);
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
		m_nTablesAddr.setPageSize(m_pStorage->getPageSize());
		m_nTablesAddr.load(m_pStorage);
		TTablePages::iterator it = m_nTablesAddr.begin();
		while(!it.isNull())
		{
			CTable* pTable = new CTable(m_pDB, it.value(), NULL);
			if(!pTable->load())
			{
				delete pTable;
				return false;
			}
			m_TablesByName.insert(pTable->getName(), pTable);
			bool bRet = !m_TablesByID.isNull(m_TablesByID.insert(pTable->getAddr(), pTable));
			assert(bRet);
			if(!bRet)
			{
				return false;
			}
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
		size_t nSize = pStream->readInt32();
		if(nSize == 0)
			return true;
		int nPageSize = m_pStorage->getPageSize();
		if(nSize > (nPageSize/sizeof(int64)) - 2)
			return false;

		for(size_t i = 0; i < nSize; ++i)
		{
			int64 nPageAddr = pStream->readInt64();
			
		}
		return true;
	}
	bool CSchema::saveHead(IDBTransactions *pTran)
	{
		FilePagePtr pPage(pTran ? pTran->getFilePage(m_nAddr) : m_pStorage->getFilePage(m_nAddr));
		if(!pPage.get())
			return false;

 
		CommonLib::FxMemoryWriteStream stream;
		stream.attach(pPage->getRowData(), pPage->getPageSize());
		sFilePageHeader header(stream, SCHEMA_PAGE, SCHEMA_ROOT_PAGE);
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
	bool CSchema::save(IDBTransactions *pTran)
	{
		if(!pTran && !m_pStorage)
			return false;

		if(m_nTablesPage == -1)
		{
			FilePagePtr pPage (pTran ?  pTran->getNewPage() : m_pStorage->getNewPage());
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

	bool CSchema::dropTable(CTable *pTable, IDBTransactions *pTran)
	{
		assert(pTable);
		if(!pTable)
			return false;

		pTable->lock(); //TO DO add in wait graf
		if(!pTable->isCanBeRemoving())
		{
			pTable->unlock();
			pTran->error(L""); //TO DO Error msg
			return false;
		}
		if(!m_nTablesAddr.remove(pTable->getAddr(), pTran))
			return false;

		m_TablesByName.remove(pTable->getName());
		m_TablesByID.remove(pTable->getAddr());
		//TO DO add free pages from tables and fields
		delete pTable;
		return true;
	}

	bool CSchema::dropTable(const CommonLib::str_t& sTableName, IDBTransactions *Tran)
	{
		TTablesByName::iterator it = m_TablesByName.find(sTableName);
		if(it.isNull())
			return false;
		return dropTable(it.value(), Tran);
	}
	bool CSchema::dropTable(int64 nID, IDBTransactions *Tran)
	{
		TTablesByID::iterator it = m_TablesByID.find(nID);
		if(it.isNull())
			return false;
		return dropTable(it.value(), Tran);
	}
}