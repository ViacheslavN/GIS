#ifndef _EMBEDDED_DATABASE_EMB_DB_INTERNAL_H_
#define _EMBEDDED_DATABASE_EMB_DB_INTERNAL_H_

#include "embDB.h"
#include "FilePage.h"
namespace embDB
{
	struct IFieldIterator;
	struct IIndexPageIterator;
	struct IValueFiled;
	struct IDBTransaction;


	COMMON_LIB_REFPTR_TYPEDEF(IFieldIterator); 
	COMMON_LIB_REFPTR_TYPEDEF(IIndexPageIterator); 
	COMMON_LIB_REFPTR_TYPEDEF(IValueFiled); 
	COMMON_LIB_REFPTR_TYPEDEF(IDBTransaction);

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

 



	struct IValueFiled: public CommonLib::RefCounter
	{
	public:
		IValueFiled() {}
		virtual ~IValueFiled() {}
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
	};

	template<class TKeyType, class TIterator, class TIteratorPtr>
	class TIndexFiled  
	{
	public:
		TIndexFiled() {}
		virtual ~TIndexFiled() {}
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
	class ILink
	{
		ILink(){}
		virtual ~ILink(){}
		virtual eLinkType getType() const = 0;
		virtual ITable* getTableFrom() const = 0;
		virtual ITable* getTableTo() const = 0;
		virtual IField* getFieldFrom() const = 0;
		virtual IField* getFieldTo() const = 0;
	};
	struct IDBShema : public IShema
	{
	public:
		IDBShema(){}
		virtual ~IDBShema(){}

		virtual ILink* AddLink(ITable* pTableFrom, IField* pFieldFrom, ITable* pTableTo, IField* pFieldTo,  eLinkType nLinkType) = 0;
		virtual ILink* AddLink(const CommonLib::CString& sTableFrom, const CommonLib::CString& sFieldFrom, const CommonLib::CString& sTableTo, const CommonLib::CString& sFieldTo,  eLinkType nLinkType) = 0;
		virtual bool deleteLink(ILink *pLink) = 0;
		virtual size_t getLinkCnt() const = 0;
		virtual ILink* getLink(size_t nIndex) const = 0;
	};

	class IDBStorage
	{
	public:
		IDBStorage(){}
		virtual ~IDBStorage(){};

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


	class IDBTable : public ITable
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

	};
	
}

#endif