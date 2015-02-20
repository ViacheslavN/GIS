#include "stdafx.h"
#include "Table.h"
#include "DBMagicSymbol.h"
#include "PageVector.h"
#include "storage.h"
#include <vector>
#include "DBFieldInfo.h"
#include "OIDField.h"
#include "Database.h"
#include "SpatialOIDField.h"
#include "Index.h"
#include "CreateFields.h"
namespace embDB
{
	CTable::CTable(CDatabase* pDB, int64 nPageAddr, CStorage* pTableStorage) : m_pDB(pDB), m_pMainDBStorage(pDB->getMainStorage()), 
		m_nTablePage(nPageAddr), 
		m_nFieldsPage(-1),
		m_nStoragePageID(-1),
		m_pTableStorage(pTableStorage),
		m_nLastRecOID(0),
		m_nIndexsPage(-1),
		m_nFieldsAddr(-1, 0, TABLE_PAGE, TABLE_FIELD_LIST_PAGE),
		m_nIndexAddr(-1, 0, TABLE_PAGE, TABLE_INDEX_LIST_PAGE)
	{
		m_nFieldsAddr.setPageSize(m_pMainDBStorage->getPageSize());
		m_nIndexAddr.setPageSize(m_pMainDBStorage->getPageSize());
	}
	CTable::CTable(CDatabase* pDB, int64 nPageAddr, const CommonLib::str_t& sTableName, CStorage* pTableStorage) : 
		m_pDB(pDB),
		m_pMainDBStorage(pDB->getMainStorage()), 
		m_nTablePage(nPageAddr), 
		m_sTableName(sTableName),
		m_nFieldsPage(-1),
		m_nStoragePageID(-1),
		m_pTableStorage(pTableStorage),
		m_nLastRecOID(0),
		m_nIndexsPage(-1),
		m_nFieldsAddr(-1, 0, TABLE_PAGE, TABLE_FIELD_LIST_PAGE),
		m_nIndexAddr(-1, 0, TABLE_PAGE, TABLE_INDEX_LIST_PAGE)
	{
		m_nFieldsAddr.setPageSize(m_pMainDBStorage->getPageSize());
		m_nIndexAddr.setPageSize(m_pMainDBStorage->getPageSize());
	}
	CTable::CTable(CDatabase* pDB, CFilePage* pFilePage, const CommonLib::str_t& sTableName, CStorage* pTableStorage/*, int64 nTableID*/) :
		m_pDB(pDB), 
		m_pMainDBStorage(pDB->getMainStorage()), 
		m_nTablePage(pFilePage->getAddr()), 
		m_sTableName(sTableName),
		m_nFieldsPage(-1),
		m_nStoragePageID(-1),
		m_pTableStorage(pTableStorage),
		m_nLastRecOID(0),
		m_nIndexsPage(-1),
		m_nFieldsAddr(-1, 0, TABLE_PAGE, TABLE_FIELD_LIST_PAGE),
		m_nIndexAddr(-1, 0, TABLE_PAGE, TABLE_INDEX_LIST_PAGE)
	{
		m_nFieldsAddr.setPageSize(m_pMainDBStorage->getPageSize());
		m_nIndexAddr.setPageSize(m_pMainDBStorage->getPageSize());
	}
	CTable::~CTable()
	{

	}

	bool CTable::addField(sFieldInfo& fi, IDBTransactions *pTran)
	{
		return CreateField(fi, pTran);
	}
	bool CTable::CreateField(sFieldInfo& fi, IDBTransactions *pTran)
	{
		TFieldByName::iterator it = m_OIDFieldByName.find(fi.m_sFieldName);
		if(it != m_OIDFieldByName.end())
			return false;
		bool bRet = false;
		switch(fi.m_nFieldType)
		{
		case FT_VALUE_FIELD:
			bRet = createValueField(fi, pTran);
			break;
		case FT_INDEX_VALUE_FIELD:
			bRet = createIndexField(fi, pTran);
			break;
		case FT_COUNTER_VALUE_FIELD:
			break;
		case FT_MULTI_INDEX_VALUE_FIELD:
			break;
		case FT_SPATIAL_INDEX_VALUE_FIELD:
		case FT_MULTI_SPATIAL_INDEX_VALUE_FIELD:
			bRet = createSpatialIndexField(fi, pTran);
			break;
		}
		return bRet;
	}
	bool  CTable::addSpatialField(sFieldInfo& fi, IDBTransactions *pTran)
	{
		return createSpatialIndexField(fi, pTran);
	}
	bool CTable::load()
	{
		CommonLib::FxMemoryReadStream stream;
		FilePagePtr pPage = m_pMainDBStorage->getFilePage(m_nTablePage);
		if(!pPage.get())
			return false;
		stream.attach(pPage->getRowData(), pPage->getPageSize());
		sFilePageHeader header(stream);
		if(!header.isValid())
			return false;//TO DO DB LOg
		if(header.m_nObjectPageType != TABLE_PAGE || header.m_nSubObjectPageType != TABLE_HEADER_PAGE)
			return false;
		int64 nPrev = stream.readInt64();
		int64  nNext = stream.readInt64();
		//m_nTableID = stream.readInt64();
		size_t nlenStr = stream.readInt32();
		if(nlenStr <= 0 || nlenStr > size_t(stream.size() - stream.pos()))
			return false;
		std::vector<wchar_t> buf(nlenStr + 1, L'\0');
		stream.read((byte*)&buf[0], nlenStr * 2);
		m_sTableName = CommonLib::str_t(&buf[0]);
		m_nStoragePageID = stream.readInt64();
		m_nFieldsPage = 0;

		if(m_nStoragePageID != -1)
		{
			if(!loadTableStorage(m_nStoragePageID))
			{
				return false;
			}
		}
		else
		{
			m_nFieldsPage = stream.readInt64();
			m_nIndexsPage = stream.readInt64();
		}

		if(m_nFieldsPage == -1)
			return true;
		{
			m_nFieldsAddr.setFirstPage(m_nFieldsPage);
			m_nFieldsAddr.setPageSize(m_pMainDBStorage->getPageSize());
			m_nFieldsAddr.load(m_pMainDBStorage);
			TFieldPages::iterator it = m_nFieldsAddr.begin();
			while(!it.isNull())
			{
				if(!ReadField(it.value(), NULL))
					return false;
				it.next();
			}

		}
		
		if(m_nIndexsPage == -1)
			return true;


		m_nIndexAddr.setFirstPage(m_nIndexsPage);
		m_nIndexAddr.setPageSize(m_pMainDBStorage->getPageSize());
		m_nIndexAddr.load(m_pMainDBStorage);
		TFieldPages::iterator it = m_nIndexAddr.begin();
		while(!it.isNull())
		{
			if(!ReadIndex(it.value(), NULL))
				return false;
			it.next();
		}
		
		return true;
	}
	bool CTable::save(IDBTransactions *pTran)
	{
		FilePagePtr pFPage = pTran->getFilePage(m_nTablePage);
		if(!pFPage.get())
			return false;
		CommonLib::FxMemoryWriteStream stream;
		stream.attach(pFPage->getRowData(), pFPage->getPageSize());
		sFilePageHeader header(stream, TABLE_PAGE, TABLE_HEADER_PAGE);
		stream.write((int64)-1);	
		stream.write((int64)-1);	
		//stream.write(m_nTableID);
		stream.write((uint32)m_sTableName.length());
		stream.write((byte*)m_sTableName.cwstr(), m_sTableName.length() * 2);
		stream.write(m_nStoragePageID);
		pFPage->setFlag(eFP_CHANGE, true);
		if(m_nStoragePageID == -1)
		{
			if(m_nFieldsPage == -1)
			{
				FilePagePtr pPage = pTran->getNewPage();
				if(!pPage.get())
					return false;
				m_nFieldsPage = pPage->getAddr();
				m_nFieldsAddr.setFirstPage(m_nFieldsPage);

			}
			stream.write(m_nFieldsPage);
			stream.write(m_nIndexsPage);
			header.writeCRC32(stream);
			pTran->saveFilePage(pFPage);
			return m_nFieldsAddr.save(pTran);
		}
		else
		{
			if(!m_pTableStorage)
				loadTableStorage(m_nStoragePageID);
			if(!m_pTableStorage)
				return false; //TO DO Log
			IDBTransactions* pInnerTran =  (IDBTransactions*)m_pDB->startTransaction(eTT_DDL);
			pInnerTran->setDBStorage(m_pTableStorage);
	
			if(!pInnerTran->begin())
			{
				m_pDB->closeTransaction(pInnerTran);
				return false;
			}
			if(!m_nFieldsAddr.save(pInnerTran))
			{
				pInnerTran->rollback();
				m_pDB->closeTransaction(pInnerTran);
				return false;
			}
			pTran->addInnerTransactions(pInnerTran);
			//bool bRet = pInnerTran->commit();
			//m_pDB->closeTransaction(pInnerTran);
			return true;
		}

		
	}
	int64 CTable::getAddr()
	{
		return m_nTablePage;
	}
	bool CTable::readFields(CommonLib::FxMemoryReadStream& stream, IDBTransactions *pTran)
	{
		size_t nCount = stream.readInt32();
		if(nCount * sizeof(int64) > size_t(stream.size() - stream.pos()))
			return false;
		for(size_t i = 0; i < nCount; ++i)
		{
			int64 nFIPage = stream.readInt64();
			if(!ReadField(nFIPage, pTran))
				return false;

		}
		return true; 
 	}
	bool CTable::ReadIndex(int64 nAddr, IDBTransactions *pTran)
	{
		return true;
	}
	bool CTable::ReadField(int64 nAddr, IDBTransactions *pTran)
	{
		CommonLib::FxMemoryReadStream stream;
		FilePagePtr pPage = m_pTableStorage ? m_pTableStorage->getFilePage(nAddr) : m_pMainDBStorage->getFilePage(nAddr);
		if(!pPage.get())
			return false;
		stream.attach(pPage->getRowData(), pPage->getPageSize());
		sFieldInfo fi;
		sFilePageHeader header (stream);
		 if(!header.isValid())
		 {
			 if(!pTran)
				 return false;
			 CommonLib::str_t sMsg;
			 sMsg.format(_T("TABLE: Page %I64d Error CRC for field header page"), nAddr);
			 pTran->error(sMsg);
			  return false;
		 }
		 if(header.m_nObjectPageType != TABLE_PAGE || header.m_nSubObjectPageType != TABLE_FIELD_PAGE )
		 {
			 if(!pTran)
				 return false;
			 CommonLib::str_t sMsg;
			 sMsg.format(_T("TABLE: Page %I64d is not field info"), nAddr);
			 pTran->error(sMsg);
			  return false;
		 }
	 
		if(!fi.Read(&stream))
			return false;
		fi.m_nFIPage = nAddr;
		return CreateField(fi, pTran);
	}
	bool CTable::readHeader(CommonLib::FxMemoryReadStream& stream)
	{
		size_t nlenStr = stream.readInt32();
		if(nlenStr <= 0 || nlenStr > size_t(stream.size() - stream.pos()))
		 return false;
		std::vector<wchar_t> buf(nlenStr + 1, L'\0');
		stream.read((byte*)&buf[0], nlenStr * 2);
		m_sTableName = CommonLib::str_t(&buf[0]);
		return true;

	}
	bool CTable::createValueField(sFieldInfo& fi, IDBTransactions *pTran)
	{
		
		bool bRet = true;
		int64 nFieldAddr = -1;
		IDBFieldHandler* pField =  CreateValueField(fi, m_pDB);
		if(!pField)
			return false;
		//return pField != NULL;
		if(fi.m_nFieldPage == -1)
		{
			assert(pTran);
			FilePagePtr pFieldInfoPage = pTran->getNewPage();
			if(!pFieldInfoPage.get())
				return false;
			FilePagePtr pFieldPage = pTran->getNewPage();
			if(!pFieldPage.get())
				return false;
			fi.m_nFieldPage = pFieldPage->getAddr();
			fi.m_nFIPage = pFieldInfoPage->getAddr();
			CommonLib::FxMemoryWriteStream stream;
			stream.attach(pFieldInfoPage->getRowData(), pFieldInfoPage->getPageSize());
			//stream.write((int64)DB_FIELD_INFO_SYMBOL);
			sFilePageHeader header (stream, TABLE_PAGE, TABLE_FIELD_PAGE);
			fi.Write(&stream);
			pFieldInfoPage->setFlag(eFP_CHANGE, true);
			header.writeCRC32(stream);
			pTran->saveFilePage(pFieldInfoPage);
			pField->setFieldInfoType(fi);
			bRet = pField->save(pFieldPage->getAddr(), pTran);
			if(!m_nFieldsAddr.push(fi.m_nFIPage, pTran))
				return false;
		}
		else
		{
			pField->setFieldInfoType(fi);
			bRet = pField->load(fi.m_nFieldPage, m_pTableStorage ? m_pTableStorage : m_pMainDBStorage);
			
			if(!bRet)
				delete pField;
		}
		m_OIDFieldByName.insert(std::make_pair(fi.m_sFieldName, pField));
		m_OIDFieldByID.insert(std::make_pair(fi.m_nFIPage, pField));
		return true;
	}
	bool CTable::createIndexField(sFieldInfo& fi, IDBTransactions *pTran)
	{
		IDBFieldHandler* pField = NULL;
		bool bRet = true;
		int64 nFieldAddr = -1;
		switch(fi.m_nFieldDataType)
		{
			case ftInteger32:
				pField = new TOIDFieldINT32(m_pDB->getBTreeAlloc());
				break;

		}
		if(!pField)
			return false;
		//return pField != NULL;
		if(fi.m_nFieldPage == -1)
		{
			assert(pTran);
			FilePagePtr pFieldInfoPage = pTran->getNewPage();
			if(!pFieldInfoPage.get())
				return false;
			FilePagePtr pFieldPage = pTran->getNewPage();
			if(!pFieldPage.get())
				return false;
			fi.m_nFieldPage = pFieldPage->getAddr();
			fi.m_nFIPage = pFieldInfoPage->getAddr();
			CommonLib::FxMemoryWriteStream stream;
			stream.attach(pFieldInfoPage->getRowData(), pFieldInfoPage->getPageSize());
			//stream.write((int64)DB_FIELD_INFO_SYMBOL);
			sFilePageHeader header (stream, TABLE_PAGE, TABLE_FIELD_PAGE);
			fi.Write(&stream);
			pFieldInfoPage->setFlag(eFP_CHANGE, true);
			header.writeCRC32(stream);
			pTran->saveFilePage(pFieldInfoPage);
			pField->setFieldInfoType(fi);
			bRet = pField->save(pFieldPage->getAddr(), pTran);
			if(!m_nFieldsAddr.push(fi.m_nFIPage, pTran))
				return false;
		}
		else
		{
			pField->setFieldInfoType(fi);
			bRet = pField->load(fi.m_nFieldPage, m_pTableStorage ? m_pTableStorage : m_pMainDBStorage);

			if(!bRet)
				delete pField;
		}
		m_OIDFieldByName.insert(std::make_pair(fi.m_sFieldName, pField));
		m_OIDFieldByID.insert(std::make_pair(fi.m_nFIPage, pField));
		return true;
	}
	bool  CTable::createSpatialIndexField(sFieldInfo& fi, IDBTransactions *pTran)
	{

		IDBFieldHandler *pSpatialField = NULL;

		switch(fi.m_nFieldDataType)
		{
			case ftPoint16:
				pSpatialField = new TPoint16Field(m_pDB->getBTreeAlloc());
				break;
			case ftPoint32:
				pSpatialField = new TPoint32Field(m_pDB->getBTreeAlloc());
				break;
			case ftPoint64:
				pSpatialField = new TPoint64Field(m_pDB->getBTreeAlloc());
				break;
			case ftShape16:
			case ftRect16:
				pSpatialField = new TRect16Field(m_pDB->getBTreeAlloc());
					break;
			case ftShape32:
			case ftRect32:
					pSpatialField = new TRect32Field(m_pDB->getBTreeAlloc());
					break;
			case ftShape64:
			case ftRect64:
				pSpatialField = new TRect64Field(m_pDB->getBTreeAlloc());
					break;
		
		}

		if(!pSpatialField)
			return false;

		if(fi.m_nFieldPage == -1)
		{
			assert(pTran);
			FilePagePtr pFieldInfoPage = pTran->getNewPage();
			if(!pFieldInfoPage.get())
				return false;
			FilePagePtr pFieldPage = pTran->getNewPage();
			if(!pFieldPage.get())
				return false;
			fi.m_nFieldPage = pFieldPage->getAddr();
			fi.m_nFIPage = pFieldInfoPage->getAddr();
			CommonLib::FxMemoryWriteStream stream;
			stream.attach(pFieldInfoPage->getRowData(), pFieldInfoPage->getPageSize());
			//stream.write((int64)DB_FIELD_INFO_SYMBOL);
			sFilePageHeader header (stream, TABLE_PAGE, TABLE_FIELD_PAGE);
			fi.Write(&stream);
			pFieldInfoPage->setFlag(eFP_CHANGE, true);
			header.writeCRC32(stream);
			pTran->saveFilePage(pFieldInfoPage);
			pSpatialField->setFieldInfoType(fi);
			pSpatialField->save(pFieldPage->getAddr(), pTran);
			if(!m_nFieldsAddr.push(fi.m_nFIPage, pTran))
				return false;
		}
		else
		{
			pSpatialField->setFieldInfoType(fi);
			bool bRet = pSpatialField->load(fi.m_nFieldPage, m_pTableStorage ? m_pTableStorage : m_pMainDBStorage);

			if(!bRet)
				delete pSpatialField;
		}
		m_OIDFieldByName.insert(std::make_pair(fi.m_sFieldName, pSpatialField));
		m_OIDFieldByID.insert(std::make_pair(fi.m_nFIPage, pSpatialField));
		return true;
	 
	}
	bool CTable::saveFields(IDBTransactions *pTran)
	{
		if(!m_OIDFieldByName.size())
			return true;
		
		CommonLib::FxMemoryWriteStream stream;
		FilePagePtr pFPage = pTran->getFilePage(m_nFieldsPage);
		FilePagePtr pNextPage(NULL);
		if(!pFPage.get())
			return false;
	 
		stream.attach(pFPage->getRowData(), pFPage->getPageSize());
		TFieldByID::iterator it =  m_OIDFieldByID.begin();
		CommonLib::FxMemoryWriteStream streamNext;
		CommonLib::FxMemoryWriteStream streamSize;
		int64 nAddr = m_nFieldsPage;
		int64 nPrevAdd = -1;

		while(it != m_OIDFieldByID.end())
		{
			if(!pFPage.get())
				return false;

			stream.attach(pFPage->getRowData(), pFPage->getPageSize());
			stream.write((int64)DB_FIELDS_PAGE_SYMBOL);
			stream.write(nPrevAdd);
			size_t nPos = stream.pos();
			streamNext.attach(pFPage->getRowData() + nPos, sizeof(int64));
			stream.seek(nPos + sizeof(int64), CommonLib::soFromBegin);
			streamSize.attach(pFPage->getRowData() + stream.pos() , sizeof(size_t));
			stream.seek(stream.pos() + sizeof(size_t), CommonLib::soFromBegin);
			size_t nSize = 0;
			size_t nSizeCnt = 0;
			size_t nCap = stream.size() - stream.pos();
			nSizeCnt = nCap/sizeof(int64);


			while(it != m_OIDFieldByID.end() && nSize < nSizeCnt)
			{
				sFieldInfo* fi = it->second->getFieldInfoType();
				stream.write(fi->m_nFIPage);	
				++nSize;
				++it;
			}

			streamSize.write((uint32)nSize);
			if(it != m_OIDFieldByID.end())
			{
				pNextPage = nAddr == -1 ? pTran->getNewPage() : pTran->getFilePage(nAddr);
				if(!pNextPage.get())
				{
					return false;
				}
				streamNext.write(pNextPage->getAddr());
			}
			else
			{
				streamNext.write((int64)-1);
			}
			nPrevAdd = pFPage->getAddr();
			pFPage->setFlag(eFP_CHANGE, true);
			pTran->saveFilePage(pFPage);
			pFPage = pNextPage;
		}
		return true;
	}

	IDBFieldHandler* CTable::getField(const CommonLib::str_t&  sName)
	{
		TFieldByName::iterator it = m_OIDFieldByName.find(sName);
		if(it == m_OIDFieldByName.end())
			return NULL;
		return it->second;
	}

	bool CTable::loadTableStorage(int64 nAddr)
	{
		FilePagePtr pPage(m_pMainDBStorage->getFilePage(nAddr));
		if(!pPage.get())
			return false;
		CommonLib::FxMemoryReadStream stream;
		stream.attach(pPage->getRowData(), pPage->getPageSize());
		if(stream.readInt64() != DB_TABLE_STORAGE_INFO_PAGE)
			return false;
		size_t nlenStr = stream.readInt32();
		if(nlenStr <= 0 || nlenStr > size_t(stream.size() - stream.pos()))
			return false;
		std::vector<wchar_t> buf(nlenStr + 1, L'\0');
		stream.read((byte*)&buf[0], nlenStr * 2);
		CommonLib::str_t sStorageName = CommonLib::str_t(&buf[0]);
		m_pTableStorage = m_pDB->getTableStorage(sStorageName, false);
		if(!m_pTableStorage)
			return false;
		return true;


	}
	
	bool CTable::lock()
	{
		TFieldByName::iterator it = m_OIDFieldByName.begin();
		while(it != m_OIDFieldByName.end())
		{
			it->second->lock();
		}
		return true;
	}
	bool CTable::isCanBeRemoving()
	{
		return true;
	}
	bool CTable::delField(IDBFieldHandler *pField, IDBTransactions *pTran )
	{
		assert(pField);
		if(!pField)
			return false;

		pField->lock();
		if(!pField->isCanBeRemoving())
		{
			pField->unlock();
			pTran->error(""); //TO DO add  message
			return false;
		}
		if(!m_nFieldsAddr.remove(pField->getFieldInfoType()->m_nFIPage, pTran))
		{
			pField->unlock();
			pTran->error(""); //TO DO add  message
			return false;
		}

		m_OIDFieldByName.erase(pField->getFieldInfoType()->m_sFieldName);
		m_OIDFieldByID.erase(pField->getFieldInfoType()->m_nFIPage);

		delete pField;

		return false;
	}


	bool CTable::delField(const CommonLib::str_t& sFieldName, IDBTransactions *pTran )
	{

		TFieldByName::iterator it = m_OIDFieldByName.find(sFieldName);
		if(it != m_OIDFieldByName.end())
		{
			pTran->error("");//TO DO error msg
			return false;
		}
		return delField(it->second, pTran);
	 
	}
	bool CTable::delField(int64 nID, IDBTransactions *pTran )
	{

		TFieldByID::iterator it = m_OIDFieldByID.find(nID);
		if(it == m_OIDFieldByID.end())
		{
			pTran->error("");//TO DO error msg
			return false;
		}
		return delField(it->second, pTran);
	}
	bool CTable::commit()
	{
		return true;
	}



	bool CTable::getOIDFieldName(CommonLib::str_t& sOIDName)
	{
		return true;
	}
	bool CTable::setOIDFieldName(const CommonLib::str_t& sOIDName)
	{
		return true;
	}
	const CommonLib::str_t& CTable::getName() const 
	{
		return m_sTableName;
	}
	IField* CTable::getField(const CommonLib::str_t& sName) const 
	{
		return NULL;
	}
	size_t CTable::getFieldCnt() const
	{
		return m_OIDFieldByID.size();
	}
	IField* CTable::getField(size_t nIdx) const
	{
		return NULL;
	}
	IField* CTable::createField(SFieldProp& sFP)
	{
		return NULL;
	}
	bool CTable::deleteField(IField* pField)
	{
		return true;
	}
	bool CTable::createIndex(const CommonLib::str_t& sFieldName, SIndexProp& ip)
	{
		TFieldByName::iterator it = m_OIDFieldByName.find(sFieldName);
		if(it == m_OIDFieldByName.end())
			return false;

		IDBFieldHandler* pFieldHandler = it->second;
		assert(pFieldHandler);
		//pFieldHandler->getType();

		return true;
	}
	bool CTable::createCompositeIndex(std::vector<CommonLib::str_t>& vecFields, SIndexProp& ip)
	{
		return true;
	}


	bool CTable::insert(IRecordset *pRecordSet, IDBTransactions *Tran)
	{
		return true;
	}
	bool CTable::insert(INameRecordset *pRecordSet, IDBTransactions *Tran)
	{
		return true;
	}

}