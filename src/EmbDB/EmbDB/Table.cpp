#include "stdafx.h"
#include "Table.h"
#include "DBMagicSymbol.h"
#include "PageVector.h"
#include "storage.h"
#include <vector>
#include "DBFieldInfo.h"
#include "ValueField.h"
#include "Database.h"
#include "SpatialOIDField.h"
#include "Index.h"
#include "MultiIndexBPTree.h"
#include "UniqueIndex.h"
#include "CreateFields.h"

namespace embDB
{
	CTable::CTable(CDatabase* pDB, int64 nPageAddr, CStorage* pTableStorage) : m_pDB(pDB), m_pMainDBStorage(pDB->getMainStorage()), 
		m_nTablePage(nPageAddr), 
		m_nFieldsPage(-1),
		m_nStoragePageID(-1),
		m_pTableStorage(pTableStorage),
		m_nRowIDPage(-1),
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
		m_nRowIDPage(-1),
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

	bool CTable::addField(sFieldInfo& fi, IDBTransactions *pTran, bool bNew)
	{
		TFieldByName::iterator it = m_FieldByName.find(fi.m_sFieldName);
		if(it != m_FieldByName.end())
			return false;

		bool bRet = false;

		switch(fi.m_nFieldDataType)
		{
		case dteSimple:
		case dteIsNotEmpty:
		case dteIsUNIQUE:
			bRet = createValueField(fi, pTran, bNew);
			break;
		}
		return bRet;


	}
	bool CTable::addIndex(sFieldInfo& fi, IDBTransactions *pTran, bool bNew)
	{
		bool bRet = false;
		switch(fi.m_nIndexType)
		{
		case itUnique:
		case itMultiRegular:
			bRet = createIndexField(fi, pTran, bNew);
			break;
		case itSpatial:
			break;
		case itFreeText:
			break;
		case itRouting:
			break;
		}

		return bRet;
	}
	bool CTable::addSpatialField(sFieldInfo& fi, IDBTransactions *pTran, bool bNew)
	{
		return createSpatialIndexField(fi, pTran, bNew);
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

			if(m_nIndexsPage == -1)
			{
				FilePagePtr pPage = pTran->getNewPage();
				if(!pPage.get())
					return false;
				m_nIndexsPage = pPage->getAddr();
				m_nIndexAddr.setFirstPage(m_nIndexsPage);

			}
			stream.write(m_nFieldsPage);
			stream.write(m_nIndexsPage);
			header.writeCRC32(stream);
			pTran->saveFilePage(pFPage);
			return m_nFieldsAddr.save(pTran) && m_nIndexAddr.save(pTran);
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
			 pTran->error(_T("TABLE: Page %I64d Error CRC for field header page"), nAddr);
			  return false;
		 }
		 if(header.m_nObjectPageType != TABLE_PAGE || header.m_nSubObjectPageType != TABLE_FIELD_PAGE )
		 {
			 if(!pTran)
				 return false;
			 pTran->error(_T("TABLE: Page %I64d is not field info"), nAddr);
			  return false;
		 }
	 
		if(!fi.Read(&stream))
			return false;
		fi.m_nFIPage = nAddr;
		return addField(fi, pTran, false);
	}

	bool CTable::ReadIndex(int64 nAddr, IDBTransactions *pTran)
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
			pTran->error(_T("TABLE: Page %I64d Error CRC for field header page"), nAddr);
			return false;
		}
		if(header.m_nObjectPageType != TABLE_PAGE || header.m_nSubObjectPageType != TABLE_INDEX_PAGE )
		{
			if(!pTran)
				return false;
			pTran->error(_T("TABLE: Page %I64d is not field info"), nAddr);
			return false;
		}

		if(!fi.Read(&stream))
			return false;
		fi.m_nFIPage = nAddr;
		return addIndex(fi, pTran, false);
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
	bool CTable::createValueField(sFieldInfo& fi, IDBTransactions *pTran, bool bNew)
	{
		
		bool bRet = true;
		int64 nFieldAddr = -1;
		IDBFieldHandler* pField =  CreateValueField(fi, m_pDB);
		if(!pField)
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
		m_FieldByName.insert(std::make_pair(fi.m_sFieldName, pField));
		m_FieldByID.insert(std::make_pair(fi.m_nFIPage, pField));

		if(bNew && fi.m_nFieldDataType == dteIsUNIQUE)
		{
		 
			embDB::SIndexProp indexProp;
			indexProp.indexType = embDB::itUnique;
			addIndex(fi.m_sFieldName, indexProp, false);
		}

		return true;
	}
	bool CTable::createIndexField(sFieldInfo& fi, IDBTransactions *pTran, bool bNew)
	{
		IDBIndexHandler* pIndex = NULL;
		bool bRet = true;
		int64 nFieldAddr = -1;
 
		if(fi.m_nIndexType == itUnique)
			pIndex = CreateUniqueIndex(fi, m_pDB);
		else if(fi.m_nIndexType == itMultiRegular)
			pIndex = CreateMultiIndex(fi, m_pDB);

		if(!pIndex)
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
			sFilePageHeader header (stream, TABLE_PAGE, TABLE_INDEX_PAGE);
			fi.Write(&stream);
			pFieldInfoPage->setFlag(eFP_CHANGE, true);
			header.writeCRC32(stream);
			pTran->saveFilePage(pFieldInfoPage);
			pIndex->setFieldInfoType(fi);
			bRet = pIndex->save(pFieldPage->getAddr(), pTran);
			if(!m_nIndexAddr.push(fi.m_nFIPage, pTran))
				return false;
		}
		else
		{
			pIndex->setFieldInfoType(fi);
			bRet = pIndex->load(fi.m_nFieldPage, m_pTableStorage ? m_pTableStorage : m_pMainDBStorage);

			if(!bRet)
				delete pIndex;
		}
		m_IndexByName.insert(std::make_pair(fi.m_sFieldName, pIndex));
		m_IndexByID.insert(std::make_pair(fi.m_nFIPage, pIndex));

		TFieldByName::iterator it = m_FieldByName.find(fi.m_sFieldName);
		if(it != m_FieldByName.end())
			it->second->setIndexHandler(pIndex);
		else
			assert(false);

		return true;
	}
	bool  CTable::createSpatialIndexField(sFieldInfo& fi, IDBTransactions *pTran, bool bNew)
	{

		IDBFieldHandler *pSpatialField = NULL;

		switch(fi.m_nFieldDataType)
		{
			case dtPoint16:
				pSpatialField = new TPoint16Field(m_pDB->getBTreeAlloc());
				break;
			case dtPoint32:
				pSpatialField = new TPoint32Field(m_pDB->getBTreeAlloc());
				break;
			case dtPoint64:
				pSpatialField = new TPoint64Field(m_pDB->getBTreeAlloc());
				break;
			case dtShape16:
			case dtRect16:
				pSpatialField = new TRect16Field(m_pDB->getBTreeAlloc());
					break;
			case dtShape32:
			case dtRect32:
					pSpatialField = new TRect32Field(m_pDB->getBTreeAlloc());
					break;
			case dtShape64:
			case dtRect64:
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
		m_FieldByName.insert(std::make_pair(fi.m_sFieldName, pSpatialField));
		m_FieldByID.insert(std::make_pair(fi.m_nFIPage, pSpatialField));
		return true;
	 
	}
	

	IDBFieldHandler* CTable::getFieldHandler(const CommonLib::str_t&  sName)
	{
		TFieldByName::iterator it = m_FieldByName.find(sName);
		if(it == m_FieldByName.end())
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
		sFilePageHeader header(stream);
		if(!header.isValid())
			return false;//TO DO DB LOg
		if(header.m_nObjectPageType != TABLE_PAGE || header.m_nSubObjectPageType != TABLE_STORAGE_PAGE)
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
		TFieldByName::iterator it = m_FieldByName.begin();
		while(it != m_FieldByName.end())
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
			pTran->error(L""); //TO DO add  message
			return false;
		}
		if(!m_nFieldsAddr.remove(pField->getFieldInfoType()->m_nFIPage, pTran))
		{
			pField->unlock();
			pTran->error(L""); //TO DO add  message
			return false;
		}

		m_FieldByName.erase(pField->getFieldInfoType()->m_sFieldName);
		m_FieldByID.erase(pField->getFieldInfoType()->m_nFIPage);

		delete pField;

		return false;
	}


	bool CTable::delField(const CommonLib::str_t& sFieldName, IDBTransactions *pTran )
	{

		TFieldByName::iterator it = m_FieldByName.find(sFieldName);
		if(it != m_FieldByName.end())
		{
			pTran->error(L"");//TO DO error msg
			return false;
		}
		return delField(it->second, pTran);
	 
	}
	bool CTable::delField(int64 nID, IDBTransactions *pTran )
	{

		TFieldByID::iterator it = m_FieldByID.find(nID);
		if(it == m_FieldByID.end())
		{
			pTran->error(L"");//TO DO error msg
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
		return m_FieldByID.size();
	}
	IField* CTable::getField(size_t nIdx) const
	{
		return NULL;
	}
	IField* CTable::createField(SFieldProp& sFP)
	{

		sFieldInfo fi;

		fi.m_nFieldType  = sFP.dataType;
		fi.m_nFieldDataType = sFP.dateTypeExt;
		fi.m_sFieldName = sFP.sFieldName;
		fi.m_sFieldAlias = sFP.sFieldAlias;
		IDBTransactions* pTran =  (IDBTransactions*)m_pDB->startTransaction(eTT_DDL);
		pTran->begin();

		if(!addField(fi, pTran, true))
		{	
			pTran->rollback();
			return NULL;
		}

		pTran->commit();
		return getField(sFP.sFieldName);
	}
	bool CTable::deleteField(IField* pField)
	{
		return true;
	}


 

	bool CTable::createIndex(const CommonLib::str_t& sFieldName, SIndexProp& ip)
	{
	
		return addIndex(sFieldName, ip, true);

	}
	bool  CTable::addIndex(const CommonLib::str_t& sFieldName, SIndexProp& ip, bool bNew)
	{
		TFieldByName::iterator it = m_FieldByName.find(sFieldName);
		if(it == m_FieldByName.end())
			return false;

		IDBFieldHandler* pFieldHandler = it->second;
		assert(pFieldHandler);

		IDBTransactions* pTran =  (IDBTransactions*)m_pDB->startTransaction(eTT_DDL);
		pTran->begin();

		sFieldInfo  filedFi = *pFieldHandler->getFieldInfoType();
		sFieldInfo indexFi;
		indexFi.m_nIndexType = ip.indexType;
		indexFi.m_nFieldType = filedFi.m_nFieldType;
		indexFi.m_nFieldDataType = filedFi.m_nFieldDataType;
		indexFi.m_sFieldName = filedFi.m_sFieldName;
		indexFi.m_sFieldAlias = filedFi.m_sFieldAlias;

		if(!addIndex(indexFi, pTran, true))
		{
			pTran->rollback();
			return false;
		}

		TIndexByName::iterator idx_it = m_IndexByName.find(sFieldName);
		assert(idx_it != m_IndexByName.end());

		IDBIndexHandler *pHandlerIndex = idx_it->second;


		BuildIndex(pHandlerIndex, pFieldHandler, pTran);
		pTran->commit();
		return true;
	}
	bool CTable::createCompositeIndex(std::vector<CommonLib::str_t>& vecFields, SIndexProp& ip)
	{
		return true;
	}



	bool CTable::BuildIndex(IDBIndexHandler* pIndexHandler, IDBFieldHandler *pFieldHandler, IDBTransactions* pTran)
	{
		IValueFiled *pField = pFieldHandler->getOIDField(pTran, m_pTableStorage ? m_pTableStorage : m_pMainDBStorage);
		IndexFiled* pIndex =	pIndexHandler->getIndex(pTran, m_pTableStorage ? m_pTableStorage : m_pMainDBStorage);

		 if(!pField || !pIndex)
			 return false;

		FieldIteratorPtr pFieldIterator =  pField->begin();
		IVariant val;
		while (!pFieldIterator->isNull())
		{
			if(!pFieldIterator->getVal(&val))
				return false;

			if(!pIndex->insert(&val, pFieldIterator->getRowID()))
				return false;
		}

		pIndex->commit();
		return true;
	}

}