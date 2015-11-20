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
#include "FixedStringField.h"
#include "StringField.h"
#include "BlobField.h"
#include "ShapeField.h"
#include "SpatialIndexHandler.h"
#include "CreateFields.h"
#include "Fields.h"
#include "ReadStreamPage.h"
#include "WriteStreamPage.h"

namespace embDB
{
	CTable::CTable(CDatabase* pDB, int64 nPageAddr) : m_pDB(pDB), 
		m_nTablePage(nPageAddr), 
		m_nFieldsPage(-1),
		m_nIndexsPage(-1),
		m_nFieldsAddr(-1, 0, TABLE_PAGE, TABLE_FIELD_LIST_PAGE),
		m_nIndexAddr(-1, 0, TABLE_PAGE, TABLE_INDEX_LIST_PAGE),
		m_OIDCounter(TABLE_PAGE, TABLE_OID_COUNTER_PAGE, MIN_PAGE_SIZE)
	{
		m_pDBStorage = pDB->getDBStorage();
		m_nFieldsAddr.setPageSize(8192/*m_pDBStorage->getPageSize()*/);
		m_nIndexAddr.setPageSize(8192/*m_pDBStorage->getPageSize()*/);
		
		m_pFields = new CFields();
		//m_pIndexs = new CFields();
	}
	CTable::CTable(CDatabase* pDB, int64 nPageAddr, const CommonLib::CString& sTableName) : 
		m_pDB(pDB),
		m_nTablePage(nPageAddr), 
		m_sTableName(sTableName),
		m_nFieldsPage(-1),
		m_nIndexsPage(-1),
		m_nFieldsAddr(-1, 0, TABLE_PAGE, TABLE_FIELD_LIST_PAGE),
		m_nIndexAddr(-1, 0, TABLE_PAGE, TABLE_INDEX_LIST_PAGE),
		m_OIDCounter(TABLE_PAGE, TABLE_OID_COUNTER_PAGE, MIN_PAGE_SIZE)
	{
		m_pDBStorage = pDB->getDBStorage();
		m_nFieldsAddr.setPageSize(8192/*m_pDBStorage->getPageSize()*/);
		m_nIndexAddr.setPageSize(8192/*m_pDBStorage->getPageSize()*/);
		
		m_pFields = new CFields();
		//m_pIndexs = new CFields();
	}
	CTable::CTable(CDatabase* pDB, CFilePage* pFilePage, const CommonLib::CString& sTableName) :
		m_pDB(pDB), 
		m_nTablePage(pFilePage->getAddr()), 
		m_sTableName(sTableName),
		m_nFieldsPage(-1),
		m_nIndexsPage(-1),
		m_nFieldsAddr(-1, 0, TABLE_PAGE, TABLE_FIELD_LIST_PAGE),
		m_nIndexAddr(-1, 0, TABLE_PAGE, TABLE_INDEX_LIST_PAGE),
		m_OIDCounter(TABLE_PAGE, TABLE_OID_COUNTER_PAGE, MIN_PAGE_SIZE)
	{
		m_pDBStorage = pDB->getDBStorage();
		m_nFieldsAddr.setPageSize(8192/*m_pDBStorage->getPageSize()*/);
		m_nIndexAddr.setPageSize(8192/*m_pDBStorage->getPageSize()*/);
		
		m_pFields = new CFields();
		//m_pIndexs = new CFields();
	}
	CTable::~CTable()
	{

	}

	bool CTable::addField(sFieldInfo* fi, IDBTransaction *pTran, bool bNew)
	{

		if(m_pFields->FieldExists(fi->m_sFieldName))
			return false;
		/*TFieldByName::iterator it = m_FieldByName.find(fi->m_sFieldName);
		if(it != m_FieldByName.end())
			return false;*/

		bool bRet = false;

		if(fi->m_nFieldDataType & (dteSimple|dteIsNotEmpty|dteIsUNIQUE))
		{
			bRet = createValueField(fi, pTran, bNew);
		}
		return bRet;


	}
	bool CTable::addIndex(sFieldInfo* fi, IDBTransaction *pTran, bool bNew)
	{
		bool bRet = false;
		switch(fi->m_nIndexType)
		{
		case itUnique:
		case itMultiRegular:
			bRet = createIndexField(fi, pTran, bNew);
			break;
		case itSpatial:
			bRet =  createSpatialIndexField(fi, pTran, bNew);
			break;
		case itFreeText:
			break;
		case itRouting:
			break;
		}

		return bRet;
	}
	 
	bool CTable::load()
	{
		CommonLib::FxMemoryReadStream stream;
		FilePagePtr pPage = m_pDBStorage->getFilePage(m_nTablePage, nTableHeaderPageSize);
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
		m_sTableName = CommonLib::CString(&buf[0]);

		m_nFieldsPage = stream.readInt64();
		m_nIndexsPage = stream.readInt64();
		int64 nCounterOIDPage = stream.readInt64();


		m_OIDCounter.SetPage(nCounterOIDPage);
		if(!m_OIDCounter.load<IDBStorage>(m_pDBStorage.get()))
			return false;
 

		if(m_nFieldsPage == -1)
			return true;
		{
			m_nFieldsAddr.setFirstPage(m_nFieldsPage);
			m_nFieldsAddr.setPageSize(/*m_pDBStorage->getPageSize()*/COMMON_PAGE_SIZE);
			m_nFieldsAddr.load(m_pDBStorage.get());
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
		m_nIndexAddr.setPageSize(/*m_pDBStorage->getPageSize()*/COMMON_PAGE_SIZE);
		m_nIndexAddr.load(m_pDBStorage.get());
		TFieldPages::iterator it = m_nIndexAddr.begin();
		while(!it.isNull())
		{
			if(!ReadIndex(it.value(), NULL))
				return false;
			it.next();
		}
		
		return true;
	}
	bool CTable::save(IDBTransaction *pTran)
	{
		FilePagePtr pFPage = pTran->getFilePage(m_nTablePage, nTableHeaderPageSize);
		if(!pFPage.get())
			return false;
		CommonLib::FxMemoryWriteStream stream;
		stream.attach(pFPage->getRowData(), pFPage->getPageSize());
		sFilePageHeader header(stream, TABLE_PAGE, TABLE_HEADER_PAGE);
		stream.write((int64)-1);	
		stream.write((int64)-1);	
	 
		stream.write((uint32)m_sTableName.length());
		stream.write((byte*)m_sTableName.cwstr(), m_sTableName.length() * 2);
		pFPage->setFlag(eFP_CHANGE, true);
		FilePagePtr pOIDFPage = pTran->getNewPage(MIN_PAGE_SIZE);

		m_OIDCounter.SetPage(pOIDFPage->getAddr());
		m_OIDCounter.SetValue(1);
		if(!m_OIDCounter.save<IDBTransaction>(pTran))
			return false;


		if(m_nFieldsPage == -1)
		{
			FilePagePtr pPage = pTran->getNewPage(nTableFieldsPageSize);
			if(!pPage.get())
				return false;
			m_nFieldsPage = pPage->getAddr();
			m_nFieldsAddr.setFirstPage(m_nFieldsPage);

		}

		if(m_nIndexsPage == -1)
		{
			FilePagePtr pPage = pTran->getNewPage(nTableIndexPageSize);
			if(!pPage.get())
				return false;
			m_nIndexsPage = pPage->getAddr();
			m_nIndexAddr.setFirstPage(m_nIndexsPage);

		}
		stream.write(m_nFieldsPage);
		stream.write(m_nIndexsPage);
		stream.write(m_OIDCounter.GetPage());
		header.writeCRC32(stream);
		pTran->saveFilePage(pFPage);
		return m_nFieldsAddr.save(pTran) && m_nIndexAddr.save(pTran);
		
	}
	int64 CTable::getAddr()
	{
		return m_nTablePage;
	}
	 
	bool CTable::ReadField(int64 nAddr, IDBTransaction *pTran)
	{
		CommonLib::FxMemoryReadStream stream;
		FilePagePtr pPage = m_pDBStorage->getFilePage(nAddr, nFieldInfoPageSize);
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

		if(fi.m_nFieldType == dtGeometry)
		{
			stream.seek(0, CommonLib::soFromBegin);
			sFilePageHeader spheader (stream); //TO DO temporary
			sSpatialFieldInfo sfi;

			if(!sfi.Read(&stream))
				return false;
			sfi.m_nFIPage = nAddr;
			return addField(&sfi, pTran, false);
		}
		else
		{
			fi.m_nFIPage = nAddr;
			return addField(&fi, pTran, false);
		}
		
	}

	bool CTable::ReadIndex(int64 nAddr, IDBTransaction *pTran)
	{
		CommonLib::FxMemoryReadStream stream;
		FilePagePtr pPage = m_pDBStorage->getFilePage(nAddr, nFieldInfoPageSize);
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

		if(fi.m_nFieldType == dtGeometry)
		{
			stream.seek(0, CommonLib::soFromBegin);
			sFilePageHeader spheader (stream); //TO DO temporary
			sSpatialFieldInfo sfi;

			if(!sfi.Read(&stream))
				return false;
			sfi.m_nFIPage = nAddr;
			return addIndex(&sfi, pTran, false);
		}
		else
		{
			
			fi.m_nFIPage = nAddr;
			return addIndex(&fi, pTran, false);
		}
	}
	bool CTable::readHeader(CommonLib::FxMemoryReadStream& stream)
	{
		size_t nlenStr = stream.readInt32();
		if(nlenStr <= 0 || nlenStr > size_t(stream.size() - stream.pos()))
		 return false;
		std::vector<wchar_t> buf(nlenStr + 1, L'\0');
		stream.read((byte*)&buf[0], nlenStr * 2);
		m_sTableName = CommonLib::CString(&buf[0]);
		return true;

	}
	bool CTable::createValueField(sFieldInfo* fi, IDBTransaction *pTran, bool bNew)
	{
		
		/*bool bRet = true;
		int64 nFieldAddr = -1;
		IDBFieldHandler* pField =  CreateValueField(fi, m_pDB, pTran);
		if(!pField)
			return false;
		if(fi->m_nFieldPage == -1)
		{
			assert(pTran);
			FilePagePtr pFieldInfoPage = pTran->getNewPage(nFieldInfoPageSize);
			if(!pFieldInfoPage.get())
				return false;
 
			WriteStreamPage stream(pTran, nFieldInfoPageSize, TABLE_PAGE, TABLE_FIELD_PAGE);
			stream.open(pFieldInfoPage->getAddr(), 0);
			stream.write(fi->m_nFieldType);
			stream.write(fi->m_nFieldDataType);
			stream.write(fi->m_sFieldName); //TO DO write utf8
			stream.write(fi->m_sFieldAlias);
			pField->setFieldInfoType(fi);
			bRet = pField->save(&stream, pTran);
			if(!m_nFieldsAddr.push(fi->m_nFIPage, pTran))
				return false;

			stream.Save();
		}
		else
		{
			pField->setFieldInfoType(fi);
			bRet = pField->load(fi->m_nFieldPage, m_pDBStorage.get());
			
			if(!bRet)
				delete pField;
		}
		//m_FieldByName.insert(std::make_pair(fi->m_sFieldName, pField));
		//m_FieldByID.insert(std::make_pair(fi->m_nFIPage, pField));

		m_pFields->AddField(pField);

		if(bNew && fi->m_nFieldDataType & dteIsUNIQUE)
		{
		 
			embDB::SIndexProp indexProp;
			indexProp.indexType = embDB::itUnique;
			addIndex(fi->m_sFieldName, indexProp, false);
		}*/

		return true;
	}
	bool CTable::createIndexField(sFieldInfo* fi, IDBTransaction *pTran, bool bNew)
	{
		IDBIndexHandler* pIndex = NULL;
		bool bRet = true;
		int64 nFieldAddr = -1;
 
		if(fi->m_nIndexType == itUnique)
			pIndex = CreateUniqueIndex(fi, m_pDB);
		else if(fi->m_nIndexType == itMultiRegular)
			pIndex = CreateMultiIndex(fi, m_pDB);

		if(!pIndex)
			return false;
		//return pField != NULL;
		if(fi->m_nFieldPage == -1)
		{
			assert(pTran);
			FilePagePtr pFieldInfoPage = pTran->getNewPage(nFieldInfoPageSize);
			if(!pFieldInfoPage.get())
				return false;
			FilePagePtr pFieldPage = pTran->getNewPage(nFieldInfoPageSize);
			if(!pFieldPage.get())
				return false;
			fi->m_nFieldPage = pFieldPage->getAddr();
			fi->m_nFIPage = pFieldInfoPage->getAddr();
			CommonLib::FxMemoryWriteStream stream;
			stream.attach(pFieldInfoPage->getRowData(), pFieldInfoPage->getPageSize());
			sFilePageHeader header (stream, TABLE_PAGE, TABLE_INDEX_PAGE);
			fi->Write(&stream);
			pFieldInfoPage->setFlag(eFP_CHANGE, true);
			header.writeCRC32(stream);
			pTran->saveFilePage(pFieldInfoPage);
//			pIndex->setFieldInfoType(fi);
			bRet = pIndex->save(pFieldPage->getAddr(), pTran);
			if(!m_nIndexAddr.push(fi->m_nFIPage, pTran))
				return false;
		}
		else
		{
		//	pIndex->setFieldInfoType(fi);
			bRet = pIndex->load(fi->m_nFieldPage, m_pDBStorage.get());

			if(!bRet)
				delete pIndex;
		}
	/*	m_IndexByName.insert(std::make_pair(fi->m_sFieldName, pIndex));
		m_IndexByID.insert(std::make_pair(fi->m_nFIPage, pIndex));

		TFieldByName::iterator it = m_FieldByName.find(fi->m_sFieldName);
		if(it != m_FieldByName.end())
			it->second->setIndexHandler(pIndex);
		else
			assert(false);*/

		//m_pIndexs->AddField(pIndex);
		m_IndexByName.insert(std::make_pair(fi->m_sFieldName, pIndex));
		IFieldPtr pField = m_pFields->GetField(fi->m_sFieldName);
		assert(pField.get());

		IDBFieldHandler *pBFieldHandler = (IDBFieldHandler*)pField.get();
		pBFieldHandler->setIndexHandler(pIndex);

		return true;
	}
	bool  CTable::createSpatialIndexField(sFieldInfo* fi, IDBTransaction *pTran, bool bNew)
	{
		TIndexByName::iterator it = m_IndexByName.find(fi->m_sFieldName);
		if(it == m_IndexByName.end())
			return false;
		//if(m_pIndexs->FieldExists(fi->m_sFieldName))
		//	return false;

		IFieldPtr pField = m_pFields->GetField(fi->m_sFieldName.cwstr());
		if(!pField.get())
			return false;

		IDBFieldHandler* pFieldHandler = (IDBFieldHandler*)pField.get();
		sFieldInfo* pFieldInfo = NULL;//pFieldHandler->getFieldInfoType();
		sSpatialFieldInfo *pSPFi = dynamic_cast<sSpatialFieldInfo *>(pFieldInfo);
		if(!pSPFi)
			return false;

		sSpatialFieldInfo spIndex =  *pSPFi;
		spIndex.m_nFieldPage = fi->m_nFieldPage;
		spIndex.m_nFIPage = fi->m_nFIPage;
		spIndex.m_nIndexType = itSpatial;
		
		IDBIndexHandler *pSpatialIndex = NULL;

		switch(spIndex.m_nSpatialType)
		{
			case stPoint16:
				pSpatialIndex = new THandlerIndexPoint16(m_pDB->getBTreeAlloc());
				break;
			case stPoint32:
				pSpatialIndex = new THandlerIndexPoint32(m_pDB->getBTreeAlloc());
				break;
			case stPoint64:
				pSpatialIndex = new THandlerIndexPoint64(m_pDB->getBTreeAlloc());
				break;
			case stRect16:
				pSpatialIndex = new THandlerIndexRect16(m_pDB->getBTreeAlloc());
				break;
			case stRect32:
				pSpatialIndex = new THandlerIndexRect32(m_pDB->getBTreeAlloc());
				break;
			case stRect64:
				pSpatialIndex = new THandlerIndexRect64(m_pDB->getBTreeAlloc());
					break;
		
		}

		if(!pSpatialIndex)
			return false;

		if(spIndex.m_nFieldPage == -1)
		{
			assert(pTran);
			FilePagePtr pFieldInfoPage = pTran->getNewPage(nFieldInfoPageSize);
			if(!pFieldInfoPage.get())
				return false;
			FilePagePtr pFieldPage = pTran->getNewPage(nFieldInfoPageSize);
			if(!pFieldPage.get())
				return false;
			spIndex.m_nFieldPage = pFieldPage->getAddr();
			spIndex.m_nFIPage = pFieldInfoPage->getAddr();
			CommonLib::FxMemoryWriteStream stream;
			stream.attach(pFieldInfoPage->getRowData(), pFieldInfoPage->getPageSize());
			//stream.write((int64)DB_FIELD_INFO_SYMBOL);
			sFilePageHeader header (stream, TABLE_PAGE, TABLE_INDEX_PAGE);
			spIndex.Write(&stream);
			pFieldInfoPage->setFlag(eFP_CHANGE, true);
			header.writeCRC32(stream);
			pTran->saveFilePage(pFieldInfoPage);
			//pSpatialIndex->setFieldInfoType(&spIndex);
			pSpatialIndex->save(pFieldPage->getAddr(), pTran);
			if(!m_nIndexAddr.push(spIndex.m_nFIPage, pTran))
				return false;
		}
		else
		{
			//pSpatialIndex->setFieldInfoType(&spIndex);
			bool bRet = pSpatialIndex->load(spIndex.m_nFieldPage, m_pDBStorage.get());

			if(!bRet)
				delete pSpatialIndex;
		}
		//m_IndexByName.insert(std::make_pair(fi->m_sFieldName, pSpatialIndex));
		//m_IndexByID.insert(std::make_pair(fi->m_nFIPage, pSpatialIndex));
		//m_pIndexs->AddField(pSpatialIndex);

		m_IndexByName.insert(std::make_pair(fi->m_sFieldName, pSpatialIndex));
		pFieldHandler->setIndexHandler(pSpatialIndex);
		return true;
	 
	}
	

	/*IDBFieldHandlerPtr CTable::getFieldHandler(const CommonLib::CString&  sName)
	{
		TFieldByName::iterator it = m_FieldByName.find(sName);
		if(it == m_FieldByName.end())
			return IDBFieldHandlerPtr();
		return it->second;
	}*/

	/*bool CTable::loadTableStorage(int64 nAddr)
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
		CommonLib::CString sStorageName = CommonLib::CString(&buf[0]);
		m_pTableStorage = m_pDB->getTableStorage(sStorageName, false);
		if(!m_pTableStorage)
			return false;
		return true;


	}*/
	
	bool CTable::lock()
	{
		/*TFieldByName::iterator it = m_FieldByName.begin();
		while(it != m_FieldByName.end())
		{
			it->second->lock();
		}*/
		return true;
	}
	bool CTable::isCanBeRemoving()
	{
		return true;
	}
	bool CTable::delField( IField *pField, IDBTransaction *pTran )
	{
		assert(pField);
		if(!pField)
			return false;

		int nIndex = m_pFields->FindField(pField->getName());
		if(nIndex == -1)
			return false;

		IDBFieldHandler *pDBField = dynamic_cast<IDBFieldHandler *>(pField);
		if(!pDBField)
			return false; //TO DO add  message


		pDBField->lock();
		if(!pDBField->isCanBeRemoving())
		{
			pDBField->unlock();
			pTran->error(L""); //TO DO add  message
			return false;
		}
		if(!m_nFieldsAddr.remove(pDBField->GetPageAddr(), pTran))
		{
			pDBField->unlock();
			pTran->error(L""); //TO DO add  message
			return false;
		}

		IDBIndexHandlerPtr pIndexHandler = pDBField->getIndexIndexHandler();
	/*	if(pIndexHandler.get())
		{

			if(!m_nIndexAddr.remove(pIndexHandler->getFieldInfoType()->m_nFIPage, pTran))
			{
				pDBField->unlock();
				pTran->error(L""); //TO DO add  message
				return false;
			}
			int nIdx = m_pIndexs->FindField(pIndexHandler->getName());
			if(nIdx != -1)
			{
				m_pIndexs->RemoveField(nIdx);
			}
		}*/
		m_pFields->RemoveField(nIndex);
		//m_FieldByName.erase(pField->getFieldInfoType()->m_sFieldName);
		//m_FieldByID.erase(pField->getFieldInfoType()->m_nFIPage);


		//delete pField;
		

		return false;
	}


	bool CTable::delField(const CommonLib::CString& sFieldName, IDBTransaction *pTran )
	{

		int nIndex = m_pFields->FindField(sFieldName.cwstr());
		if(nIndex == -1)
		{
			pTran->error(L"");//TO DO error msg
			return false;
		}
		return delField(m_pFields->GetField(nIndex).get(), pTran);
	 
	}
	
	bool CTable::commit(IDBTransaction *pTran)
	{
		m_OIDCounter.save(pTran);
		return true;
	}
	 int64 CTable::GetNextOID()
	{
		return m_OIDCounter.GetNext();
	}


	bool CTable::getOIDFieldName(CommonLib::CString& sOIDName)
	{
		return true;
	}
	bool CTable::setOIDFieldName(const CommonLib::CString& sOIDName)
	{
		return true;
	}
	const CommonLib::CString& CTable::getName() const 
	{
		return m_sTableName;
	}
	IFieldPtr CTable::createField(SFieldProp& sFP, ITransaction *pTran )
	{

		if(sFP.m_sFieldName.length() > nMaxFieldNameLen || sFP.m_sFieldAlias.length() > nMaxFieldAliasLen)
			return IFieldPtr(); // TO DO Error

		if(m_pFields->FieldExists(sFP.m_sFieldName))
			return IFieldPtr(); // TO DO Error

		IDBTransactionPtr pDBTran;
		if(pTran)
		{
			if(pTran->getType() != eTT_DDL)
				return IFieldPtr(); // TO DO Error

			pDBTran =  (IDBTransaction*)pTran;

		}
		else
		{

			pDBTran =  ((IDBTransaction*)m_pDB->startTransaction(eTT_DDL).get());
			pDBTran->begin();
		}

		FilePagePtr pFieldInfoPage = pDBTran->getNewPage(nFieldInfoPageSize);
		if(!pFieldInfoPage.get())
			return IFieldPtr(); // TO DO Error

		WriteStreamPage stream(pDBTran.get(), nFieldInfoPageSize, TABLE_PAGE, TABLE_FIELD_PAGE);
		stream.open(pFieldInfoPage->getAddr(), 0);
		stream.write(sFP.m_dataType);


		IDBFieldHandler* pField =  CreateValueField(&sFP, m_pDB, pDBTran.get(), pFieldInfoPage->getAddr());
		if(!pField)
			return IFieldPtr(); // TO DO Error


		if(pField->save(&stream, pDBTran.get()))
		{
			return IFieldPtr(); // TO DO Error
		}

		if(!m_nFieldsAddr.push(pFieldInfoPage->getAddr(), pDBTran.get()))
			return IFieldPtr(); // TO DO Error

		stream.Save();

		/*if(!addField(&fi, pTran.get(), true))
		{	
			pTran->rollback();
			m_pDB->closeTransaction(pTran.get());
			return IFieldPtr();
		}*/

		if(!pTran)
		{
			pDBTran->commit();
			m_pDB->closeTransaction(pDBTran.get());
		}

		return getField(sFP.m_sFieldName);
	}
	bool CTable::deleteField(IField* pField)
	{
		return delField(pField);
	}

	IFieldPtr CTable::getField(const CommonLib::CString& sName) const
	{
		return m_pFields->GetField(sName);
	}
	size_t  CTable::getFieldCnt() const
	{
		return m_pFields->GetFieldCount();
	}
	IFieldPtr CTable::getField(size_t nIdx) const
	{
		return m_pFields->GetField(nIdx);
	}
 

	bool CTable::createIndex(const CommonLib::CString& sFieldName, SIndexProp& ip)
	{
	
		return addIndex(sFieldName, ip, true);

	}
	bool  CTable::addIndex(const CommonLib::CString& sFieldName, SIndexProp& ip, bool bNew)
	{
		TIndexByName::iterator it = m_IndexByName.find(sFieldName);
		if(it == m_IndexByName.end())
			return false;
		//if(m_pIndexs->FieldExists(sFieldName))
		//	return false;

	/*	IFieldPtr pField  = m_pFields->GetField(sFieldName);
		if(!pField.get())
			return false;

		IDBFieldHandler* pFieldHandler = (IDBFieldHandler*)pField.get();
	 

		IDBTransaction* pTran =  (IDBTransaction*)m_pDB->startTransaction(eTT_DDL).get();
		pTran->begin();

		sFieldInfo*  filedFi = pFieldHandler->getFieldInfoType();
		sFieldInfo indexFi;
		indexFi.m_nIndexType = ip.indexType;
		indexFi.m_nFieldType = filedFi->m_nFieldType;
		indexFi.m_nFieldDataType = filedFi->m_nFieldDataType;
		indexFi.m_sFieldName = filedFi->m_sFieldName;
		indexFi.m_sFieldAlias = filedFi->m_sFieldAlias;*/

	/*	if(!addIndex(&indexFi, pTran, true))
		{
			pTran->rollback();
			return false;
		}

		TIndexByName::iterator idx_it = m_IndexByName.find(sFieldName);
		assert(idx_it != m_IndexByName.end());

		IDBIndexHandlerPtr pIndex = idx_it->second;

		BuildIndex(pIndex.get(), pFieldHandler, pTran);
		pTran->commit();*/
		return true;
	}
	bool CTable::createCompositeIndex(std::vector<CommonLib::CString>& vecFields, SIndexProp& ip)
	{
		return true;
	}



	bool CTable::BuildIndex(IDBIndexHandler* pIndexHandler, IDBFieldHandler *pFieldHandler, IDBTransaction* pTran)
	{
		IValueFieldPtr pField = pFieldHandler->getValueField(pTran,  m_pDBStorage.get());
		IndexFiledPtr pIndex =	pIndexHandler->getIndex(pTran, m_pDBStorage.get());

		 if(!pField.get() || !pIndex.get())
			 return false;

		IFieldIteratorPtr pFieldIterator =  pField->begin();
		/*IVariant*/CommonLib::CVariant val;
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

	eSpatialType CTable::GetSpatialType(uint64 nMaxVal, bool isPoint)
	{
		if(nMaxVal < 0xFFFF)
			return  isPoint ? stPoint16 : stRect16;
		else if(nMaxVal < 0xFFFFFFFF)
			return  isPoint ? stPoint32 : stRect32;

		return  isPoint ? stPoint64 : stRect64;
	}

	IFieldPtr CTable::createShapeField(const wchar_t *pszFieldName, const wchar_t* pszAlias,  CommonLib::eShapeType shapeType,
		const CommonLib::bbox& extent, eSpatialCoordinatesUnits CoordUnits, bool bCreateIndex)
	{

		/*TFieldByName::iterator it = m_FieldByName.find(pszFieldName);
		if(it != m_FieldByName.end())
			return IFieldPtr();*/
		if(m_pFields->FieldExists(pszFieldName))
			return IFieldPtr();

		if(shapeType == CommonLib::shape_type_null || extent.type == CommonLib::bbox_type_invalid)
			return IFieldPtr(); //TO DO Log

		double dOffsetX = 0., dOffsetY = 0., dScaleX = 1., dScaleY = 1.;
		if(extent.xMin < 0)
			dOffsetX = fabs(extent.xMin);
		else
			dOffsetX = -1 *extent.xMin;
	
		if(extent.yMin < 0)
			dOffsetY = fabs(extent.yMin);
		else
			dOffsetY = -1 *extent.yMin;

		
		eSpatialType SpatialDataType = stUnknown; 			 

		bool isPoint = false;
		if(shapeType == CommonLib::shape_type_point || shapeType == CommonLib::shape_type_point_m || shapeType == CommonLib::shape_type_point_zm /* || 
			shapeType == CommonLib::shape_type_multipoint || shapeType == CommonLib::shape_type_multipoint_m || shapeType == CommonLib::shape_type_multipoint_zm || shapeType == CommonLib::shape_type_multipoint_z*/)
		{
			isPoint = true;
		}
		double dMaxX = fabs(extent.xMax + dOffsetX);
		double dMaxY = fabs(extent.yMax + dOffsetY);
		double dMaxCoord = max(dMaxX, dMaxY);
		switch(CoordUnits)
		{
			case scuDecimalDegrees:
				dScaleX = 0.0000001;
				dScaleY = 0.0000001;
				break;
			case scuKilometers:
			case scuMiles:
				dScaleX = 0.001;
				dScaleY = 0.001;
				break;
			case scuMeters:
			case scuYards:
			case scuFeet:
			case scuDecimeters:
			case scuInches:
				dScaleX = 0.01;
				dScaleY = 0.01;
				break;
			case scuMillimeters:
				dScaleX = 1;
				dScaleY = 1;
				break;
			default:
				dScaleX = 0.0001;
				dScaleY = 0.0001;
				break;
		}
		
		int64 nMaxVal = int64(dMaxCoord/dScaleX);
		SpatialDataType = GetSpatialType(nMaxVal, isPoint);
	
		sSpatialFieldInfo fi;
		fi.m_extent = extent;
		fi.m_bCheckCRC32 = true;
		fi.m_ShapeType = shapeType;
		fi.m_sFieldName = pszFieldName;
		fi.m_sFieldAlias = pszAlias;
		fi.m_dOffsetX = dOffsetX;
		fi.m_dOffsetY = dOffsetY;
		fi.m_dScaleX = dScaleX;
		fi.m_dScaleY = dScaleY;
		fi.m_nFieldType = dtGeometry;
		fi.m_nCoordType = CoordUnits;
		fi.m_nSpatialType = SpatialDataType;


		IDBTransaction* pTran =  (IDBTransaction*)m_pDB->startTransaction(eTT_DDL).get();
		pTran->begin();
		if(!createValueField(&fi, pTran, true))
		{	
			pTran->rollback();
			m_pDB->closeTransaction(pTran);
			return IFieldPtr();
		}
		pTran->commit();
		m_pDB->closeTransaction(pTran);		

		if(bCreateIndex)
		{

			//addIndex(const CommonLib::CString& sFieldName, SIndexProp& ip, bool bNew)
			
			SIndexProp ip;
			ip.indexType = itSpatial;
			addIndex(pszFieldName, ip, true);
		}
		return getField(fi.m_sFieldName);

	}
}