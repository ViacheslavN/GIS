#ifndef _EMBEDDED_DATABASE_EMB_DB_INTERNAL_H_
#define _EMBEDDED_DATABASE_EMB_DB_INTERNAL_H_

#include "embDB.h"
#include "FilePage.h"
namespace embDB
{

	
	struct sFieldInfo
	{
		sFieldInfo() : 
			m_nFieldType(0)
			,m_nFieldDataType(dtUnknown)
			,m_nBaseFieldProp(0)
			,m_nIndexType(0)
			,m_bPrimeryKey(false)
			,m_bSecondaryKey(false)
			,m_nRefTableID(-1)
			,m_nRefTFieldID(-1)
			,m_nFieldPage(-1)
			,m_nFIPage(-1)
			,m_bCheckCRC32(true)
			,m_nOffsetX(0)
			,m_nOffsetY(0)
			,m_nLenField(0)
		{

		}
		virtual ~sFieldInfo(){}

		CommonLib::CString m_sFieldName;
		CommonLib::CString m_sFieldAlias;
		uint32 m_nFieldType;
		uint32 m_nFieldDataType;
		uint32 m_nBaseFieldProp;
		uint32 m_nIndexType;
		bool m_bPrimeryKey;
		bool m_bSecondaryKey;
		int64 m_nRefTableID;
		int64 m_nRefTFieldID;
		int64 m_nFieldPage;
		int64 m_nFIPage;
		bool m_bCheckCRC32;
		CommonLib::TRect2D64 m_Extent;
		uint64	  m_nOffsetX;
		uint64	  m_nOffsetY;
		uint32   m_nLenField;


		bool Read(CommonLib::FxMemoryReadStream* pStream)
		{
			size_t nlenStr = pStream->readInt32();
			if(nlenStr == 0 || nlenStr > size_t(pStream->size() - pStream->pos()))
				return false;
			std::vector<wchar_t> Namebuf(nlenStr + 1, L'\0');
			pStream->read((byte*)&Namebuf[0], nlenStr * 2);
			m_sFieldName = CommonLib::CString(&Namebuf[0]);


			nlenStr = pStream->readInt32();
			if(nlenStr > size_t(pStream->size() - pStream->pos()))
				return false;
			if(nlenStr != 0)
			{
				std::vector<wchar_t> Aliasbuf(nlenStr + 1, L'\0');
				pStream->read((byte*)&Aliasbuf[0], nlenStr * 2);
				m_sFieldAlias = CommonLib::CString(&Aliasbuf[0]);
			}
				
			m_nFieldType = pStream->readIntu32();
			m_nFieldDataType = pStream->readIntu32();
			m_nBaseFieldProp= pStream->readIntu32();
			m_nIndexType = pStream->readIntu32(); 
			m_bPrimeryKey = pStream->readBool();
			m_bSecondaryKey = pStream->readBool();
			m_nRefTableID = pStream->readInt64();
			m_nRefTFieldID = pStream->readInt64();
			m_nFieldPage =  pStream->readInt64();
			m_bCheckCRC32 = pStream->readBool();
			m_nLenField= pStream->readIntu32(); 
			return true;
		}
		void Write(CommonLib::FxMemoryWriteStream* pStream)
		{
			pStream->write((uint32)m_sFieldName.length());
			pStream->write((byte*)m_sFieldName.cwstr(), m_sFieldName.length() * 2);
			pStream->write((uint32)m_sFieldAlias.length());
			pStream->write((byte*)m_sFieldAlias.cwstr(), m_sFieldAlias.length() * 2);
			pStream->write(m_nFieldType);
			pStream->write(m_nFieldDataType);
			pStream->write(m_nBaseFieldProp);
			pStream->write(m_nIndexType);
			pStream->write(m_bPrimeryKey);
			pStream->write(m_bSecondaryKey);
			pStream->write(m_nRefTableID);
			pStream->write(m_nRefTFieldID);
			pStream->write(m_nFieldPage);
			pStream->write(m_bCheckCRC32);
			pStream->write(m_nLenField);
		}
	};


	struct sSpatialFieldInfo : public sFieldInfo
	{
		sSpatialFieldInfo() :  m_nCoordType(scuUnknown), m_dScaleX(1.), m_dScaleY(1.), m_dOffsetX(0.), m_dOffsetY(0.), m_ShapeType(CommonLib::shape_type_null)
		{

		}
		 virtual ~sSpatialFieldInfo(){}
		//CommonLib::TRect2D64 m_nExtent;


		double m_dScaleX;
		double m_dScaleY;
		double m_dOffsetX;
		double m_dOffsetY;

		eSpatialCoordinatesUnits m_nCoordType;
		CommonLib::eShapeType m_ShapeType;
		CommonLib::bbox m_extent;

		bool Read(CommonLib::FxMemoryReadStream* pStream)
		{
			if(!sFieldInfo::Read(pStream))
				return false;

			m_dScaleX = pStream->readDouble();
			m_dScaleY = pStream->readDouble();
			m_dOffsetX = pStream->readDouble();
			m_dOffsetY = pStream->readDouble();

			m_nCoordType = (eSpatialCoordinatesUnits)pStream->readIntu32();
			m_nIndexType = (eDataTypes)pStream->readIntu32();
			pStream->read(m_extent.xMin);
			pStream->read(m_extent.yMin);
			pStream->read(m_extent.xMax);
			pStream->read(m_extent.yMax);
			

			return true;
		}

		void Write(CommonLib::FxMemoryWriteStream* pStream)
		{
			sFieldInfo::Write(pStream);

			  pStream->write(m_dScaleX);
			  pStream->write(m_dScaleY);
			  pStream->write(m_dOffsetX);
			  pStream->write(m_dOffsetY);

			  pStream->write((uint32)m_nCoordType);
			  pStream->write((uint32)m_nIndexType);

			  pStream->write(m_extent.xMin);
			  pStream->write(m_extent.yMin);
			  pStream->write(m_extent.xMax);
			  pStream->write(m_extent.yMax);
		

		}
		/*template <class TPoint>
		void ReadExtent(CommonLib::FxMemoryReadStream* pStream)
		{
			TPoint xMin, xMax, yMin, yMax;
			pStream->read(xMin);
			pStream->read(yMin);
			pStream->read(xMax);
			pStream->read(yMax);
			m_nExtent.set(xMin, yMin, xMax, yMax);
		}

		template <class TPoint>
		void WriteExtent(CommonLib::FxMemoryWriteStream* pStream)
		{
			pStream->write((TPoint)m_nExtent.m_minX);
			pStream->write((TPoint)m_nExtent.m_minY);
			pStream->write((TPoint)m_nExtent.m_maxX);
			pStream->write((TPoint)m_nExtent.m_maxY);
		}*/

	};

	struct IFieldIterator;
	struct IIndexPageIterator;
	struct IValueField;
	struct IDBTransaction;
	struct IDBIndexHandler;
	struct IDBFieldHandler;
	struct IDBStorage;
	struct IDBShema;
	struct ILink;
	struct IDBStorage;
	struct IDBDatabase;
	struct IDBTable;

	COMMON_LIB_REFPTR_TYPEDEF(IFieldIterator); 
	COMMON_LIB_REFPTR_TYPEDEF(IIndexPageIterator); 
	COMMON_LIB_REFPTR_TYPEDEF(IValueField); 
	COMMON_LIB_REFPTR_TYPEDEF(IDBTransaction);
	COMMON_LIB_REFPTR_TYPEDEF(IDBIndexHandler);
	COMMON_LIB_REFPTR_TYPEDEF(IDBFieldHandler);
	COMMON_LIB_REFPTR_TYPEDEF(IDBShema);
	COMMON_LIB_REFPTR_TYPEDEF(ILink);
	COMMON_LIB_REFPTR_TYPEDEF(IDBStorage);
	COMMON_LIB_REFPTR_TYPEDEF(IDBTable);

	struct IFieldIterator : public CommonLib::AutoRefCounter
	{
	public:
		IFieldIterator(){};
		virtual ~IFieldIterator(){}
		virtual bool isValid() = 0;
		virtual bool next() = 0;
		virtual bool back() = 0;
		virtual bool isNull() = 0;
		virtual bool getVal(CommonLib::CVariant* pVal) = 0;
		virtual uint64 getRowID() = 0;

		virtual int64 addr() const = 0;
		virtual int32 pos() const = 0;

		virtual bool copy(IFieldIterator *pIter) = 0;
	};

	 

	template<class TKeyType>
	class TIIndexIterator : public CommonLib::RefCounter
	{
	public:
		TIIndexIterator(){};
		virtual ~TIIndexIterator(){};
		virtual bool isValid() = 0;
		virtual bool next() = 0;
		virtual bool isNull() = 0;
		virtual bool getKey(TKeyType* pIndexKey) = 0;
		virtual uint64 getRowID() = 0;


		virtual int64 addr() const = 0;
		virtual int32 pos() const = 0;

		virtual bool copy(TIIndexIterator *pIter) = 0;
	};

	typedef TIIndexIterator<CommonLib::CVariant> IIndexIterator;
	COMMON_LIB_REFPTR_TYPEDEF(IIndexIterator); 
 

	struct IIndexPageIterator  : public CommonLib::RefCounter
	{
	public:
		IIndexPageIterator();
		virtual ~IIndexPageIterator();
		virtual bool isValid() = 0;
		virtual bool next() = 0;
		virtual bool back() = 0;
		virtual bool isNull() = 0;
		virtual bool getKey(CommonLib::CVariant* pIndexKey) = 0;
		virtual uint64 getPage() = 0;
		virtual uint32 getPos() = 0;
	};

	template<class TKeyType, class TIterator, class TIteratorPtr>
	struct  TIndexFiled  : public CommonLib::AutoRefCounter
	{
	public:
		TIndexFiled() {}
		virtual ~TIndexFiled() {}
		virtual indexTypes GetType() const = 0;
		virtual bool insert (TKeyType* pIndexKey, uint64 nOID, TIterator* pFromIter = NULL, TIterator** pRetIter = NULL) = 0;
		virtual bool update (TKeyType* pOldIndexKey, TKeyType* pNewIndexKey, uint64 nOID, TIterator* pFromIter = NULL, TIterator** pRetIter = NULL) = 0;
		virtual bool remove (TKeyType* pIndexKey, TIterator** pRetIter = NULL) = 0;
		virtual bool remove (IIndexIterator* pIter ) = 0;
		virtual TIteratorPtr find(TKeyType* pIndexKey) = 0;
		virtual TIteratorPtr lower_bound(TKeyType* pIndexKey) = 0;
		virtual TIteratorPtr upper_bound(TKeyType* pIndexKey) = 0;
		virtual bool commit() = 0;
	};
	
	typedef TIndexFiled<CommonLib::CVariant, IIndexIterator, IIndexIteratorPtr> IndexFiled;

	COMMON_LIB_REFPTR_TYPEDEF(IndexFiled); 

	struct IValueField: public CommonLib::RefCounter
	{
	public:
		IValueField() {}
		virtual ~IValueField() {}
		virtual const sFieldInfo* getFieldInfoType() const = 0;
		//virtual bool insert (uint64 nOID, IVariant* pFieldVal, IFieldIterator* pFromIter = NULL, IFieldIterator **pRetIter = NULL) = 0;
		//virtual uint64 insert (IVariant* pFieldVal, IFieldIterator* pFromIter = NULL, IFieldIterator **pRetIter = NULL) = 0;

		virtual bool insert (uint64 nOID, CommonLib::CVariant* pVal, IFieldIterator* pFromIter = NULL, IFieldIterator **pRetIter = NULL) = 0;
		virtual uint64 insert ( CommonLib::CVariant* pVal, IFieldIterator* pFromIter = NULL, IFieldIterator **pRetIter = NULL) = 0;


		virtual bool update (uint64 nRowID, CommonLib::CVariant* pFieldVal) = 0;
		virtual bool remove (uint64 nRowID, IFieldIterator **pRetIter = NULL) = 0;
		virtual bool find(uint64 nOID, CommonLib::CVariant* pFieldVal) = 0;
		virtual IFieldIteratorPtr find(uint64 nRowID) = 0;
		virtual IFieldIteratorPtr begin() = 0;
		virtual IFieldIteratorPtr last() = 0;
		virtual bool commit() = 0;

		virtual IndexFiledPtr GetIndex() = 0;
		virtual void SetIndex(IndexFiled *pIndex) = 0;
	};

	

	
	struct IDBIndexHandler : public IField
	{
	public:
		IDBIndexHandler(){}
		~IDBIndexHandler(){}
		virtual sFieldInfo* getFieldInfoType() = 0;
		virtual void setFieldInfoType(sFieldInfo* fi) = 0;
		virtual bool save(int64 nAddr, IDBTransaction *pTran) = 0;
		virtual bool load(int64 nAddr, IDBStorage *pStorage) = 0;
		virtual IndexFiledPtr getIndex(IDBTransaction* pTransactions, IDBStorage *pStorage) = 0;
		virtual bool release(IndexFiled* pInxex) = 0;

		virtual bool lock() =0;
		virtual bool unlock() =0;
		virtual bool isCanBeRemoving() = 0;

	};


	struct IDBFieldHandler : public IField
	{
	public:
		IDBFieldHandler(){}
		~IDBFieldHandler(){}
		virtual sFieldInfo* getFieldInfoType() = 0;
		virtual void setFieldInfoType(sFieldInfo* fi) = 0;
		virtual bool save(int64 nAddr, IDBTransaction *pTran) = 0;
		virtual bool load(int64 nAddr, IDBStorage *pStorage) = 0;
		virtual IValueFieldPtr getValueField(IDBTransaction* pTransactions, IDBStorage *pStorage) = 0;
		virtual bool release(IValueField* pField) = 0;

		virtual void setIndexHandler(IDBIndexHandler *pIndexHandler) = 0;
		virtual IDBIndexHandlerPtr getIndexIndexHandler() = 0;

		virtual bool lock() =0;
		virtual bool unlock() =0;

		virtual bool isCanBeRemoving() = 0;

	};
	
	/*struct ICounterFiled: public CommonLib::RefCounter
	{
	public:
		ICounterFiled() {}
		virtual ~ICounterFiled() {}
		virtual bool insert (uint64 nRowID, IFieldIterator* pFromIter = NULL, IFieldIterator **pRetIter = NULL) = 0;
		virtual bool update (uint64 nRowID, CommonLib::CVariant* pFieldVal) = 0;
		virtual bool remove (uint64 nRowID, IFieldIterator **pRetIter = NULL) = 0;
		virtual bool find(uint64 nOID, CommonLib::CVariant* pFieldVal) = 0;
		virtual FieldIteratorPtr find(uint64 nOID) = 0;
		virtual FieldIteratorPtr begin() = 0;
		virtual FieldIteratorPtr last() = 0;
		virtual bool commit() = 0;
	};*/

	enum eLinkType
	{
		ltOneToOne,
		ltOneToMany,
		ltManyToMany

	};
	struct ILink : CommonLib::AutoRefCounter
	{
		ILink(){}
		virtual ~ILink(){}
		virtual eLinkType getType() const = 0;
		virtual ITablePtr getTableFrom() const = 0;
		virtual ITablePtr getTableTo() const = 0;
		virtual IFieldPtr getFieldFrom() const = 0;
		virtual IFieldPtr getFieldTo() const = 0;
	};
	struct IDBShema : public ISchema
	{
	public:
		IDBShema(){}
		virtual ~IDBShema(){}

		virtual ILinkPtr AddLink(ITable* pTableFrom, IField* pFieldFrom, ITable* pTableTo, IField* pFieldTo,  eLinkType nLinkType) = 0;
		virtual ILinkPtr AddLink(const CommonLib::CString& sTableFrom, const CommonLib::CString& sFieldFrom, const CommonLib::CString& sTableTo, const CommonLib::CString& sFieldTo,  eLinkType nLinkType) = 0;
		virtual bool deleteLink(ILink *pLink) = 0;
		virtual size_t getLinkCnt() const = 0;
		virtual ILinkPtr getLink(size_t nIndex) const = 0;

		virtual bool open(IDBStorage* pStorage, __int64 nFileAddr, bool bNew = false) = 0;
		virtual bool close() = 0;
		virtual bool save(IDBTransaction *pTran) = 0;
	};

	struct IDBStorage : CommonLib::AutoRefCounter
	{
	public:
		IDBStorage(){}
		virtual ~IDBStorage(){};

		virtual bool open(const wchar_t* pszName, bool bReadOnle, bool bNew, bool bCreate, bool bOpenAlways, size_t nPageSize) = 0;
		virtual bool close() = 0;
		virtual void setPageSize(size_t nPageSize) = 0;
 		virtual int64 getFileSzie() = 0;
 

		virtual FilePagePtr getFilePage(int64 nAddr, bool bRead = true, uint32 nSize = 0) = 0;
		virtual bool dropFilePage(FilePagePtr pPage) = 0;
		virtual bool dropFilePage(int64 nAddr) = 0;
		virtual FilePagePtr getNewPage(bool bWrite = false, uint32 nSize = 0) = 0;
		virtual bool saveFilePage(CFilePage* pPage, size_t nDataSize = 0,  bool ChandgeInCache = false) = 0;
		virtual bool saveFilePage(FilePagePtr pPage, size_t nDataSize = 0,  bool ChandgeInCache = false) = 0;
		virtual bool saveNewPage(FilePagePtr pPage) = 0;
		virtual size_t getPageSize() const = 0;
		virtual int64 getNewPageAddr(uint32* nType = NULL, uint32 nSize = 0) = 0;
		//virtual FilePagePtr createPage(int64 nAddr) = 0;
		virtual bool commit() = 0;
		virtual bool removeFromFreePage(int64 nAddr) = 0;
		//virtual bool saveChache() = 0;
		///для восстановления после сбоев, или отката транзакции
		virtual int64 getFileSize()  = 0;
		virtual bool setFileSize(int64 nSize) = 0;
		virtual int64 getBeginFileSize() const = 0;
		virtual bool isDirty() const = 0;
		virtual const CommonLib::CString & getTranFileName() const = 0;
		virtual void clearDirty() = 0;
		virtual void setStoragePageInfo(int64 nStorageInfo)= 0;
		virtual bool initStorage(int64 nStorageInfo)= 0;
		virtual bool loadStorageInfo()= 0;
		virtual bool saveStorageInfo()= 0;


		//for write/save
		virtual bool isLockWrite() = 0;
		virtual bool lockWrite(IDBTransaction *pTran = NULL) = 0;
		virtual bool try_lockWrite() = 0;
		virtual bool unlockWrite(IDBTransaction *pTran = NULL) = 0;

		virtual bool saveForUndoState(IDBTransaction *pTran) = 0;
		virtual bool undo(IDBTransaction *pTran) = 0;
		virtual bool reload()= 0;
		//
		virtual bool saveState() = 0;
	};


	struct IDBTable : public ITable
	{
	public:
		IDBTable(){}
		virtual ~IDBTable(){}

		virtual bool isLockWrite() = 0;
		virtual bool lockWrite() = 0;
		virtual bool try_lockWrite() = 0;
		virtual bool unlockWrite() = 0;

		


		//	virtual bool insert(IRecordset *pRecordSet, IDBTransactions *Tran = NULL) = 0;
		//	virtual bool insert(INameRecordset *pRecordSet, IDBTransactions *Tran = NULL) = 0;

		virtual IFieldPtr createField(SFieldProp& sFP) = 0;
		virtual IFieldPtr createShapeField(const wchar_t *pszFieldName, const wchar_t* pszAlias, CommonLib::eShapeType shapeType, const CommonLib::bbox& extent, eSpatialCoordinatesUnits CoordUnits, bool bCreateIndex = true) = 0;
		virtual bool deleteField(IField* pField) = 0;
		virtual bool createIndex(const CommonLib::CString& sName, SIndexProp& ip) = 0;
		virtual bool createCompositeIndex(std::vector<CommonLib::CString>& vecFields, SIndexProp& ip) = 0;

		virtual uint64 GetNextOID() = 0;
		virtual bool commit(IDBTransaction *pTran) = 0;
	};


	enum eRestoreType
	{
		rtUndefined,
		rtUndo,
		rtRedo
	};
 
	class IDBBtree
	{
	public:
		virtual bool commit() = 0;
	};
	struct  IDBTransaction : public ITransaction
	{
	public:
		IDBTransaction(){}
		virtual ~IDBTransaction(){}


		virtual bool restore(bool Force = false) = 0;


		virtual FilePagePtr getFilePage(int64 nAddr, bool bRead = true, uint32 nSize = 0) = 0;
		virtual void dropFilePage(FilePagePtr pPage) = 0;
		virtual void dropFilePage(int64 nAddr) = 0;
		virtual FilePagePtr getNewPage(uint32 nSize = 0) = 0;
		virtual void saveFilePage(FilePagePtr pPage,  size_t nSize = 0,  bool bChandgeInCache = false) = 0;
		virtual size_t getPageSize() const = 0;


		virtual FilePagePtr getTranNewPage(uint32 nSize = 0)= 0;
		virtual FilePagePtr getTranFilePage(int64 nAddr, bool bRead = true, uint32 nSize = 0) = 0;
		virtual void saveTranFilePage(FilePagePtr pPage,  size_t nSize = 0,  bool bChandgeInCache = false) = 0;
		virtual void addUndoPage(FilePagePtr pPage, bool bReadFromDB = false) = 0;

		virtual void addInnerTransactions(IDBTransaction *pTran) = 0;

		virtual void error(const wchar_t *pszFormat, ...) = 0;
		virtual uint32 getLogLevel() const = 0;
		virtual void log(uint32 nLevel, const wchar_t *pszFormat, ...) = 0;

		virtual void addDBBTree(IDBBtree *pTree) = 0;
		virtual int64 getID() const = 0;
		virtual const CommonLib::CString& getFileTranName() const = 0;
		virtual bool isCompleted() const = 0;
		virtual void setDBStorage(IDBStorage *pStorage)  = 0;
		virtual void stop() = 0;
		virtual void wait() = 0;

		virtual IValueFieldPtr GetField(const wchar_t* pszTableName, const wchar_t* pszFieldName) = 0;

	};
	struct IDBDatabase : public IDatabase
	{
		IDBDatabase(){}
		virtual ~IDBDatabase(){}
		virtual IDBStoragePtr getDBStorage() const = 0;
	};
}

#endif