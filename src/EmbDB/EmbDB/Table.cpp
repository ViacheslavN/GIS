#include "stdafx.h"
#include "Table.h"
#include "DBMagicSymbol.h"
#include "PageVector.h"
#include "storage.h"
#include <vector>
#include "DBFieldInfo.h"
#include "ValueField.h"
#include "Database.h"
//#include "SpatialOIDField.h"
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
		m_nFieldsAddr(-1, 0, TABLE_PAGE, TABLE_FIELD_LIST_PAGE, pDB->getCheckCRC()),
		m_nIndexAddr(-1, 0, TABLE_PAGE, TABLE_INDEX_LIST_PAGE, pDB->getCheckCRC()),
		m_OIDCounter(TABLE_PAGE, TABLE_OID_COUNTER_PAGE, MIN_PAGE_SIZE, pDB->getCheckCRC())
	{
		m_pDBStorage = pDB->getDBStorage();
		m_nFieldsAddr.setPageSize(MIN_PAGE_SIZE);
		m_nIndexAddr.setPageSize(MIN_PAGE_SIZE);
		
		m_pFields = new CFields();
		//m_pIndexs = new CFields();
	}
	CTable::CTable(CDatabase* pDB, int64 nPageAddr, const CommonLib::CString& sTableName) : 
		m_pDB(pDB),
		m_nTablePage(nPageAddr), 
		m_sTableName(sTableName),
		m_nFieldsPage(-1),
		m_nIndexsPage(-1),
		m_nFieldsAddr(-1, 0, TABLE_PAGE, TABLE_FIELD_LIST_PAGE, pDB->getCheckCRC()),
		m_nIndexAddr(-1, 0, TABLE_PAGE, TABLE_INDEX_LIST_PAGE, pDB->getCheckCRC()),
		m_OIDCounter(TABLE_PAGE, TABLE_OID_COUNTER_PAGE, MIN_PAGE_SIZE, pDB->getCheckCRC())
	{
		m_pDBStorage = pDB->getDBStorage();
		m_nFieldsAddr.setPageSize(MIN_PAGE_SIZE);
		m_nIndexAddr.setPageSize(MIN_PAGE_SIZE);
		
		m_pFields = new CFields();
		//m_pIndexs = new CFields();
	}
	CTable::CTable(CDatabase* pDB, CFilePage* pFilePage, const CommonLib::CString& sTableName) :
		m_pDB(pDB), 
		m_nTablePage(pFilePage->getAddr()), 
		m_sTableName(sTableName),
		m_nFieldsPage(-1),
		m_nIndexsPage(-1),
		m_nFieldsAddr(-1, 0, TABLE_PAGE, TABLE_FIELD_LIST_PAGE, pDB->getCheckCRC()),
		m_nIndexAddr(-1, 0, TABLE_PAGE, TABLE_INDEX_LIST_PAGE, pDB->getCheckCRC()),
		m_OIDCounter(TABLE_PAGE, TABLE_OID_COUNTER_PAGE, MIN_PAGE_SIZE, pDB->getCheckCRC())
	{
		m_pDBStorage = pDB->getDBStorage();
		m_nFieldsAddr.setPageSize(MIN_PAGE_SIZE);
		m_nIndexAddr.setPageSize(MIN_PAGE_SIZE);
		
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
 
	 
	bool CTable::load()
	{
		CommonLib::FxMemoryReadStream stream;
		FilePagePtr pPage = m_pDBStorage->getFilePage(m_nTablePage, nTableHeaderPageSize);
		if(!pPage.get())
			return false;
		stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());
		sFilePageHeader header(stream, pPage->getPageSize(), m_pDB->getCheckCRC());
		if(m_pDB->getCheckCRC() && !header.isValid())
			return false;//TO DO DB LOg
		if(header.m_nObjectPageType != TABLE_PAGE || header.m_nSubObjectPageType != TABLE_HEADER_PAGE)
			return false;
		int64 nPrev = stream.readInt64();
		int64  nNext = stream.readInt64();
		//m_nTableID = stream.readInt64();
		uint32 nlenStr = stream.readInt32();
		if(nlenStr <= 0 || nlenStr > uint32(stream.size() - stream.pos()))
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
			m_nFieldsAddr.setPageSize(MIN_PAGE_SIZE);
			m_nFieldsAddr.load(m_pDBStorage.get());
			TFieldPages::iterator it = m_nFieldsAddr.begin();
			while(!it.isNull())
			{
				if(!ReadField(it.value()))
					return false;
				it.next();
			}

		}
		
		if(m_nIndexsPage == -1)
			return true;


		m_nIndexAddr.setFirstPage(m_nIndexsPage);
		m_nIndexAddr.setPageSize(MIN_PAGE_SIZE);
		m_nIndexAddr.load(m_pDBStorage.get());
		TFieldPages::iterator it = m_nIndexAddr.begin();
		while(!it.isNull())
		{
			if(!ReadIndex(it.value()))
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
		stream.attachBuffer(pFPage->getRowData(), pFPage->getPageSize());
		sFilePageHeader header(stream, TABLE_PAGE, TABLE_HEADER_PAGE, pFPage->getPageSize(), m_pDB->getCheckCRC());
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
	 
	

	
	bool CTable::readHeader(CommonLib::FxMemoryReadStream& stream)
	{
		uint32 nlenStr = stream.readInt32();
		if(nlenStr <= 0 || nlenStr > uint32(stream.size() - stream.pos()))
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
	IDBIndexHandlerPtr CTable::createIndexHandler(IDBFieldHandler *pField, const SIndexProp& ip, int64 nPageAddr)
	{
		IDBIndexHandlerPtr pIndex;
 
		if(ip.m_indexType == itUnique)
			pIndex = CreateUniqueIndex(pField, m_pDB, nPageAddr,  ip);
		else if(ip.m_indexType  == itMultiRegular)
			pIndex = CreateMultiIndex(pField, m_pDB, nPageAddr,  ip);
		else if(ip.m_indexType == itSpatial)
		{
			IDBShapeFieldHandler* pShapeField = dynamic_cast<IDBShapeFieldHandler*>(pField);
			if(!pShapeField)
			{
				//TO DO Log
				return pIndex;
			}
			pIndex = createSpatialIndexField(pShapeField, nPageAddr,  ip);
		}
		return pIndex;
	}
	IDBIndexHandlerPtr  CTable::createSpatialIndexField(IDBShapeFieldHandler* pField,int64 nPageAddr, const SIndexProp& ip)
	{
		IDBIndexHandlerPtr pSpatialIndex;
		switch(pField->GetPointType())
		{
			case stPoint16:
				pSpatialIndex = new THandlerIndexPoint16(pField, m_pDB->getBTreeAlloc(), nPageAddr, ip);
				break;
			case stPoint32:
				pSpatialIndex = new THandlerIndexPoint32(pField, m_pDB->getBTreeAlloc(), nPageAddr, ip);
				break;
			case stPoint64:
				pSpatialIndex = new THandlerIndexPoint64(pField, m_pDB->getBTreeAlloc(), nPageAddr, ip);
				break;
			case stRect16:
				pSpatialIndex = new THandlerIndexRect16(pField, m_pDB->getBTreeAlloc(), nPageAddr, ip);
				break;
			case stRect32:
				pSpatialIndex = new THandlerIndexRect32(pField, m_pDB->getBTreeAlloc(), nPageAddr, ip);
				break;
			case stRect64:
				pSpatialIndex = new THandlerIndexRect64(pField, m_pDB->getBTreeAlloc(), nPageAddr, ip);
					break;
		
		}

		return pSpatialIndex;
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
		stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());
		sFilePageHeader header(stream);
		if(!header.isValid())
			return false;//TO DO DB LOg
		if(header.m_nObjectPageType != TABLE_PAGE || header.m_nSubObjectPageType != TABLE_STORAGE_PAGE)
			return false;
		uint32 nlenStr = stream.readInt32();
		if(nlenStr <= 0 || nlenStr > uint32(stream.size() - stream.pos()))
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
	IFieldPtr CTable::createField(const  SFieldProp& sFP, ITransaction *pTran )
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
		{

			FilePagePtr pFieldInfoPage = pDBTran->getNewPage(nFieldInfoPageSize);
			if(!pFieldInfoPage.get())
				return IFieldPtr(); // TO DO Error

			WriteStreamPage stream(pDBTran.get(), nFieldInfoPageSize, TABLE_PAGE, TABLE_FIELD_PAGE);
			stream.open(pFieldInfoPage);
			stream.write((uint32)sFP.m_dataType);
			stream.write(sFP.m_nPageSize);
			stream.write(sFP.m_nLenField);

			IDBFieldHandlerPtr pField =  CreateValueField(&sFP, m_pDB, pFieldInfoPage->getAddr());
			if(!pField.get())
				return IFieldPtr(); // TO DO Error


			if(!pField->save(&stream, pDBTran.get()))
			{
				return IFieldPtr(); // TO DO Error
			}

			if(!m_nFieldsAddr.push(pFieldInfoPage->getAddr(), pDBTran.get()))
				return IFieldPtr(); // TO DO Error

			stream.Save();
			m_pFields->AddField(pField.get());
		}
		
		if(!pTran)
		{
			pDBTran->commit();
			m_pDB->closeTransaction(pDBTran.get());
		}
		return getField(sFP.m_sFieldName);
	}
		IFieldPtr CTable::createShapeField(const wchar_t *pszFieldName, const wchar_t* pszAlias,  CommonLib::eShapeType shapeType,
		const CommonLib::bbox& extent, eSpatialCoordinatesUnits CoordUnits, bool bCreateIndex, uint32 nPageSize, ITransaction *pTran)
	{

		
		if(shapeType == CommonLib::shape_type_null || extent.type == CommonLib::bbox_type_invalid)
			return IFieldPtr(); //TO DO Log


		SFieldProp fp;
		fp.m_sFieldName = pszFieldName;
		fp.m_sFieldAlias = pszAlias;
		fp.m_nPageSize = nPageSize;
		fp.m_dataType = dtGeometry;
		
		if( fp.m_sFieldName.length() > nMaxFieldNameLen || fp.m_sFieldAlias.length() > nMaxFieldAliasLen)
			return IFieldPtr(); // TO DO Error

		if(m_pFields->FieldExists(fp.m_sFieldName))
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

	

		double dOffsetX = 0., dOffsetY = 0.;
		byte nScaleX = 1, nScaleY = 1;
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
				nScaleX = 7;//0.0000001;
				nScaleY = 7;
				break;
			case scuKilometers:
			case scuMiles:
				nScaleX = 3;//0.001;
				nScaleY = 3;//0.001;
				break;
			case scuMeters:
			case scuYards:
			case scuFeet:
			case scuDecimeters:
			case scuInches:
				nScaleX = 2;//0.01;
				nScaleY = 2;//0.01;
				break;
			case scuMillimeters:
				nScaleX = 1;
				nScaleY = 1;
				break;
			default:
				nScaleX = 4;//0.0001;
				nScaleY = 4;//0.0001;
				break;
		}
		double dScale = 1/pow(10., nScaleX);
		int64 nMaxVal = int64(dMaxCoord/dScale);
		SpatialDataType = GetSpatialType(nMaxVal, isPoint);
		IDBShapeFieldHandlerPtr pShapeFieldHandler; 
		{

			FilePagePtr pFieldInfoPage = pDBTran->getNewPage(nFieldInfoPageSize);
			if(!pFieldInfoPage.get())
				return IFieldPtr(); // TO DO Error

			WriteStreamPage stream(pDBTran.get(), nFieldInfoPageSize, TABLE_PAGE, TABLE_FIELD_PAGE);
			stream.open(pFieldInfoPage);
			stream.write((uint32)fp.m_dataType);
			stream.write(fp.m_nPageSize);
			stream.write(fp.m_nLenField);

			ShapeValueFieldHandler *pShapeField = new  ShapeValueFieldHandler(m_pDB->getBTreeAlloc(), &fp, pFieldInfoPage->getAddr());
			pShapeFieldHandler = pShapeField;
			
			pShapeField->SetBoundingBox(extent);
			pShapeField->SetPointType(SpatialDataType);
			pShapeField->SetShapeType(shapeType);
			pShapeField->SetUnits(CoordUnits);
			pShapeField->SetOffsetX(dOffsetX);
			pShapeField->SetOffsetY(dOffsetY);
			pShapeField->SetScaleX(nScaleX);
			pShapeField->SetScaleY(nScaleY);

			if(!pShapeField->save(&stream, pDBTran.get()))
			{
				return IFieldPtr(); // TO DO Error
			}

			if(!m_nFieldsAddr.push(pFieldInfoPage->getAddr(), pDBTran.get()))
				return IFieldPtr(); // TO DO Error

			stream.Save();
	
		}
		m_pFields->AddField((IField*)pShapeFieldHandler.get());
		if(bCreateIndex)
		{
	
			SIndexProp ip;
			ip.m_indexType = itSpatial;
			ip.m_nNodePageSize = nPageSize;
			createIndex(pszFieldName, ip, pDBTran.get());
		}
		if(!pTran)
		{
			pDBTran->commit();
			m_pDB->closeTransaction(pDBTran.get());
		}
		return getField(fp.m_sFieldName);

	}

	
	bool  CTable::createIndex(const CommonLib::CString& sFieldName, SIndexProp& ip, ITransaction *pTran)
	{
		TIndexByName::iterator it = m_IndexByName.find(sFieldName);
		if(it != m_IndexByName.end())
			return false;
	
		IFieldPtr pField  = m_pFields->GetField(sFieldName);
		if(!pField.get())
			return false;

		IDBFieldHandler* pFieldHandler = (IDBFieldHandler*)pField.get();


	
		IDBTransactionPtr pDBTran;
		if(pTran)
		{
			if(pTran->getType() != eTT_DDL)
				return false; // TO DO Error

			pDBTran =  (IDBTransaction*)pTran;

		}
		else
		{

			pDBTran =  ((IDBTransaction*)m_pDB->startTransaction(eTT_DDL).get());
			pDBTran->begin();
		}
		{

			FilePagePtr pFieldInfoPage = pDBTran->getNewPage(nFieldInfoPageSize);
			if(!pFieldInfoPage.get())
			{
				if(!pTran)
				{
					pDBTran->commit();
					m_pDB->closeTransaction(pDBTran.get());
				}
				return false; // TO DO Error
			}

			IDBIndexHandlerPtr pIndex = createIndexHandler(pFieldHandler, ip, pFieldInfoPage->getAddr());
			if(!pIndex.get())
			{
				if(!pTran)
				{
					pDBTran->commit();
					m_pDB->closeTransaction(pDBTran.get());
				}
				return false; // TO DO Error
			}

			WriteStreamPage stream(pDBTran.get(), nFieldInfoPageSize, FIELD_PAGE, TABLE_INDEX_PAGE);
			stream.open(pFieldInfoPage);
			stream.write((uint32)ip.m_indexType);
			stream.write(sFieldName);
			stream.write(ip.m_nNodePageSize);
			pIndex->save(&stream, pDBTran.get());

			stream.Save();

			pFieldHandler->setIndexHandler(pIndex.get());
			m_IndexByName.insert(std::make_pair(sFieldName, pIndex));
			m_nIndexAddr.push(pFieldInfoPage->getAddr(), pDBTran.get());
		}


		TIndexByName::iterator idx_it = m_IndexByName.find(sFieldName);
		assert(idx_it != m_IndexByName.end());

		IDBIndexHandlerPtr pIndex = idx_it->second;

		BuildIndex(pIndex.get(), pFieldHandler, pDBTran.get());

		if(!pTran)
		{
			pDBTran->commit();
			m_pDB->closeTransaction(pDBTran.get());
		}

		return true;
	}
	bool CTable::ReadIndex(int64 nAddr)
	{
		CommonLib::FxMemoryReadStream stream;
		FilePagePtr pPage = m_pDBStorage->getFilePage(nAddr, nFieldInfoPageSize);
		if(!pPage.get())
			return false;
		stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());
		SIndexProp ip;
		sFilePageHeader header (stream, pPage->getPageSize(), m_pDB->getCheckCRC());
		if(!header.isValid())
		{
			//TO DO log
			return false;
		}
		if(header.m_nObjectPageType != FIELD_PAGE || header.m_nSubObjectPageType != TABLE_INDEX_PAGE )
		{
			return false;
		}
	 
		ip.m_indexType = (indexTypes)stream.readIntu32();
		CommonLib::CString sFieldName;
		stream.read(sFieldName);

		IFieldPtr pField  = m_pFields->GetField(sFieldName);
		if(!pField.get())
			return false;

		ip.m_nNodePageSize = stream.readIntu32();

		IDBFieldHandler* pFieldHandler = (IDBFieldHandler*)pField.get();

		IDBIndexHandlerPtr pIndex = createIndexHandler(pFieldHandler, ip, pPage->getAddr());
		if(!pIndex.get())
		{
			//TO DO Error
			return false;
		}

		if(!pIndex->load(&stream, m_pDBStorage.get()))
			return false;
		pFieldHandler->setIndexHandler(pIndex.get());
		m_IndexByName.insert(std::make_pair(sFieldName, pIndex));
		return true;

	}
	bool CTable::ReadField(int64 nAddr)
	{
		ReadStreamPage stream(m_pDBStorage.get(), MIN_PAGE_SIZE, TABLE_PAGE, TABLE_FIELD_PAGE);
		FilePagePtr pPage = m_pDBStorage->getFilePage(nAddr, nFieldInfoPageSize);

		if(!stream.open(nAddr, 0))
			return false;
		SFieldProp sFP;
		sFP.m_dataType = (eDataTypes)stream.readIntu32();
		stream.read(sFP.m_nPageSize);
		stream.read(sFP.m_nLenField);

	
		IDBFieldHandlerPtr pField =  CreateValueField(&sFP, m_pDB, nAddr);
		if(!pField.get())
			return false;

		if(!pField->load(&stream, m_pDBStorage.get()))
		{
			return false;
		}
		m_pFields->AddField(pField.get());
		return true;

	}

	bool CTable::deleteField(IField* pField)
	{
		return delField(pField);
	}

	IFieldPtr CTable::getField(const CommonLib::CString& sName) const
	{
		return m_pFields->GetField(sName);
	}
	uint32  CTable::getFieldCnt() const
	{
		return m_pFields->GetFieldCount();
	}
	IFieldPtr CTable::getField(uint32 nIdx) const
	{
		return m_pFields->GetField(nIdx);
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


}