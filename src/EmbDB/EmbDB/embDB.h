#ifndef _EMBEDDED_DATABASE_EMB_DB_H_
#define _EMBEDDED_DATABASE_EMB_DB_H_

#include "CommonLibrary/String.h"
#include "CommonLibrary/SpatialKey.h"
#include "CommonLibrary/MemoryStream.h"
#include "CommonLibrary/IRefCnt.h"
#include "CommonLibrary/Variant.h"
#include "CommonLibrary/BoundaryBox.h"
namespace embDB
{
	enum eDataTypes
	{
		dtUnknown   = 0,
		dtNull,
		dtInteger8,
		dtInteger16,
		dtInteger32,
		dtInteger64,
		dtUInteger8,
		dtUInteger16,
		dtUInteger32,
		dtUInteger64,
		dtOid,
		dtFloat,
		dtDouble,
		dtString,
		dtBlob ,
		dtGeometry , 
		dtRaster,
		dtDate,
		dtSerializedObject
	};


	enum eSpatialType
	{
		stUnknown = 0,
		stPoint16,
		stPoint32,
		stPoint64,
		stRect16,
		stRect32,
		stRect64
	};
		

	struct STypeSize
	{
		short nLineNo;
		size_t nSize;
	};

	static STypeSize  arrTypeSizes[] = {
		{dtUnknown,			 0},
		{dtNull,			 0},
		{dtInteger8,		 1},
		{dtInteger16,		 2},
		{dtInteger32,		 4},
		{dtInteger64,		 8},
		{dtUInteger8,		 1},
		{dtUInteger16,		 2},
		{dtUInteger32,		 4},
		{dtUInteger64,		 8},
		{dtOid,				 8},
		{dtFloat,			 4},
		{dtDouble,			 8},
		{dtString,			 0},
		{dtBlob,			 8},
		{dtRaster,	 		 8},
		{dtDate,	         4},
		{dtSerializedObject, 8}

	};

	enum eDataTypesExt
	{
		dteSimple = 1,
		dteIsNotEmpty = 2,
		dteIsUNIQUE = 4,
		dteIsCounter = 8,
		dteIsLink = 16
	};
	enum indexTypes
	{
		itUnknown   = 0x00,
		itUnique,
		itMultiRegular,
		itSpatial,
		itFreeText,
		itRouting
	};
	enum SpatialQueryMode
	{
		sqmIntersect = 0,
		sqmOver,
		sqmInside,
		sqmByFeature
	};

	enum eSpatialCoordinatesUnits
	{

		scuUnknown         = 0,
		scuInches          = 1,
		scuPoints          = 2,
		scuFeet            = 3,
		scuYards           = 4,
		scuMiles           = 5,
		scuNauticalMiles   = 6,
		scuMillimeters     = 7,
		scuCentimeters     = 8,
		scuMeters          = 9,
		scuKilometers      = 10,
		scuDecimalDegrees  = 11,
		scuDecimeters      = 12
	};

	enum eShapeType
	{
		stNotDef =0,
		stPoint,
		stLine,
		stPolyLine,
		stRect,
		stPolygon,
		stMuitiPolygon
	};

	enum ePointType
	{
		ptNotDef = 0,
		ptUINT16,
		ptUINT32,
		ptUINT64
	};
	enum eStringCoding
	{
		scUndefined =0,
		scASCII,
		scUTF8
		//scUTF16
	};

	enum eGeoSpatialCoordinatesFormat
	{
		gsfNotDef = 0,
		gsfDDDDDDD,
		gsfDDMMSS,
		gsfDDMMMM
	};
	//http://gis-lab.info/qa/dms2dd.html#sel=

	enum DBTransactionMode
	{
		eTMSingleTransactions,
		eTMOneReadersManywriter,
		eTMMultiTransactions
	};

	struct SFieldProp
	{
		CommonLib::CString m_sFieldName;
		CommonLib::CString m_sFieldAlias;
		eDataTypes m_dataType;
		uint32 m_nLenField;
		CommonLib::CVariant m_devaultValue;
		double m_dScale;
		int m_nPrecision;
		bool m_bNotNull;
		bool m_bUNIQUE;
		bool m_bCounter;
		uint32 m_nPageSize;



		SFieldProp() : m_nLenField(0), m_dataType(dtUnknown), m_dScale(0), m_nPrecision(0),
			m_bNotNull(false), m_bUNIQUE(false), m_bCounter(false), m_nPageSize(8192)
		{}
	};

	struct SIndexProp
	{
		indexTypes indexType;
	};
	enum eTransactionType
	{
		eTT_UNDEFINED=1,  //предпологатеься что тип может быть любой
		eTT_MODIFY = 2,
		eTT_SELECT = 4,
		eTT_DDL = 8  
	};


	struct IField;
	struct IFields;
	struct IFieldSet;
	struct IRow;
	struct ICursor;
	struct IInsertCursor;
	struct IUpdateCursor;
	struct IDeleteCursor;
	struct ITable;
	struct ISchema;
	struct IStatement;
	struct ITransaction;
	struct IDatabase;
	struct IIndex;

	COMMON_LIB_REFPTR_TYPEDEF(IField);
	COMMON_LIB_REFPTR_TYPEDEF(IFields);
	COMMON_LIB_REFPTR_TYPEDEF(IFieldSet);
	COMMON_LIB_REFPTR_TYPEDEF(IRow);
	COMMON_LIB_REFPTR_TYPEDEF(ICursor);
	COMMON_LIB_REFPTR_TYPEDEF(IInsertCursor);
	COMMON_LIB_REFPTR_TYPEDEF(IUpdateCursor);
	COMMON_LIB_REFPTR_TYPEDEF(IDeleteCursor);
	COMMON_LIB_REFPTR_TYPEDEF(ITable);
	COMMON_LIB_REFPTR_TYPEDEF(ISchema);
	COMMON_LIB_REFPTR_TYPEDEF(IStatement); 
	COMMON_LIB_REFPTR_TYPEDEF(ITransaction); 
	COMMON_LIB_REFPTR_TYPEDEF(IDatabase); 
	COMMON_LIB_REFPTR_TYPEDEF(IIndex);

	struct IField: public CommonLib::AutoRefCounter
	{
	public:
		IField(){}
		virtual ~IField(){}
		virtual eDataTypes getType() const = 0;
		virtual const CommonLib::CString& getName() const = 0;
		virtual const CommonLib::CString& getAlias() const = 0;
		virtual uint32 GetLength()	const = 0;
		virtual bool GetIsNotNull() const = 0;
		virtual double GetScale() const = 0;
		virtual const CommonLib::CVariant& 	GetDefaultValue() const  = 0;
		virtual int  GetPrecision() const  = 0;
	};


	struct IShapeField
	{
		IShapeField(){}
		virtual ~IShapeField(){}

		virtual eShapeType GetShapeType() const = 0;
		virtual eSpatialType GetPointType() const = 0;
		virtual eSpatialCoordinatesUnits GetUnits() const = 0;
		virtual const CommonLib::bbox& GetBoundingBox() const = 0;
		virtual double GetOffsetX()  const = 0;
		virtual double GetOffsetY()  const = 0;
		virtual double GetScaleX()  const = 0;
		virtual double GetScaleY()  const = 0;

	};

	struct IFields: public CommonLib::AutoRefCounter
	{

		IFields(){}
		virtual ~IFields(){}
		virtual int       GetFieldCount() const = 0;
		virtual void      SetFieldCount(int count) = 0;
		virtual IFieldPtr GetField(int index) const = 0;
		virtual IFieldPtr  GetField(const CommonLib::CString& name) const = 0;
		virtual void      SetField(int index, IField* field) = 0;
		virtual void      AddField(IField* field) = 0;
		virtual void      RemoveField(int index) = 0;
		virtual int       FindField(const CommonLib::CString& name) const = 0;
		virtual bool      FieldExists(const CommonLib::CString& name) const = 0;
		virtual void	  Clear() = 0;
	};


	struct IIndex : public CommonLib::AutoRefCounter
	{
		IIndex(){}
		virtual ~IIndex(){}
		virtual indexTypes GetType() const = 0;
		virtual  IFieldPtr GetField() const = 0;


	};

	struct IFieldSet: public CommonLib::AutoRefCounter
	{
		IFieldSet(){}
		virtual ~IFieldSet(){}
		virtual int  GetCount() const = 0;
		virtual int  Find(const CommonLib::CString& field) const = 0;
		virtual void Reset() = 0;
		virtual bool Next(CommonLib::CString* field) = 0;
		virtual void Add(const CommonLib::CString& field) = 0;
		virtual void Remove(const CommonLib::CString& field) = 0;
		virtual	const CommonLib::CString& Get(int nIndex) const = 0;
		virtual void Clear() = 0;
	};

	struct IRow : public CommonLib::RefCounter
	{
	public:
		IRow(){}
		virtual ~IRow(){};
		virtual int32 count() const = 0;
		virtual bool IsFieldSelected(int index) const = 0;
		virtual CommonLib::CVariant* value(int32 nNum) = 0;
		virtual const CommonLib::CVariant* value(int32 nNum) const = 0;
		virtual bool set(CommonLib::CVariant& pValue, int32 nNum) = 0;
		virtual IFieldSetPtr		   GetFieldSet() const = 0;
		virtual IFieldsPtr             GetSourceFields() const = 0;
		virtual int64				GetRowID() const = 0;
		virtual void				SetRow(int64 nRowID) = 0;
	};

	struct INameRow : public IRow
	{
	public:
		INameRow(){}
		virtual ~INameRow(){};

		virtual CommonLib::CVariant* value(const wchar_t* pszName) = 0;
		virtual bool set(const CommonLib::CString& sName,  CommonLib::CVariant* pValue) = 0;
	};
	
	struct ICursor : public CommonLib::AutoRefCounter
	{
	public:
		ICursor(){}
		virtual ~ICursor(){}
		virtual IFieldSetPtr GetFieldSet() const = 0;
		virtual IFieldsPtr   GetSourceFields() const = 0;
		virtual bool         IsFieldSelected(int index) const = 0;
		virtual bool NextRow(IRowPtr* pRow = NULL) = 0;
		virtual bool  value(CommonLib::CVariant* pValue, int32 nNum) = 0;


	};

	struct IInsertCursor : public CommonLib::AutoRefCounter
	{
		IInsertCursor(){}
		virtual ~IInsertCursor(){}
		virtual IRowPtr createRow() = 0;
		virtual int64 insert(IRow* pRow) = 0;
		virtual IFieldSetPtr GetFieldSet() const = 0;
		virtual IFieldsPtr   GetSourceFields() const = 0;
	};
	struct IUpdateCursor : public ICursor
	{
		IUpdateCursor(){}
		virtual ~IUpdateCursor(){}
		virtual bool update(IRow* pRow) = 0;
	};
	struct IDeleteCursor : public CommonLib::AutoRefCounter
	{
		IDeleteCursor(){}
		virtual ~IDeleteCursor(){}
		virtual bool remove(IRow* pRow) = 0;
		virtual bool remove(int64 nRowID) = 0;
	};

	
	struct ITable : public CommonLib::AutoRefCounter
	{
	public:
		ITable(){}
		virtual ~ITable(){}
		virtual bool getOIDFieldName(CommonLib::CString& sOIDName) = 0;
		virtual bool setOIDFieldName(const CommonLib::CString& sOIDName) = 0;
		virtual const CommonLib::CString& getName() const  = 0;
		virtual IFieldPtr getField(const CommonLib::CString& sName) const= 0 ;
		virtual size_t getFieldCnt() const= 0;
		virtual IFieldPtr getField(size_t nIdx) const = 0;
		virtual IFieldsPtr getFields() const = 0;
	};

	struct ISchema : public CommonLib::AutoRefCounter
	{
	public:
		ISchema(){}
		virtual ~ISchema(){}
		virtual size_t getTableCnt() const = 0;
		virtual ITablePtr getTable(size_t nIndex) const = 0;
		virtual ITablePtr getTableByID(int64 nID) const = 0;
		virtual ITablePtr getTableByName(const wchar_t* pszTableName) const = 0;

		virtual bool addTable(const  wchar_t*  sTableName, ITransaction *Tran = NULL)= 0;
		virtual bool dropTable(const CommonLib::CString& sTableName, ITransaction *Tran = NULL)= 0;
		virtual bool dropTable(int64 nID, ITransaction *Tran = NULL)= 0;
		virtual bool dropTable(ITable *pTable, ITransaction *Tran = NULL)= 0;


	};

	struct IRecordset
	{
	public:
		IRecordset(){}
		virtual ~IRecordset(){};
		virtual int32 count() const = 0;

		virtual IRowPtr row(int32 nNum) = 0;
		virtual bool set(IRow* pRow, int32 nNum) = 0;
		virtual bool add(IRow* pRow) = 0;
	};


	struct IStatement : public CommonLib::AutoRefCounter
	{
	public:

		IStatement(){}
		virtual ~IStatement(){}
		virtual bool setValue(const wchar_t *pszValueName, CommonLib::CVariant* pVal) = 0;
		virtual CommonLib::CVariant* getValue(const wchar_t *pszValueName)= 0;
		virtual bool setValue(uint32 nNum, CommonLib::CVariant* pVal) = 0;
		virtual CommonLib::CVariant* getValue(uint32 Col) = 0;

		virtual IFieldPtr getField(uint32 nCount) = 0; 
		virtual uint32 getFieldCount() const = 0;
		virtual int32 getColNum(const wchar_t *pszValueName) const = 0;



		virtual bool setValueByte(uint32 Col, byte val) = 0;
		virtual bool setValueInt8(uint32 Col, int8 val) = 0;
		virtual bool setValueInt16(uint32 Col, int16 val) = 0;
		virtual bool setValueUInt16(uint32 Col, uint16 val) = 0;
		virtual bool setValueInt32(uint32 Col, int32 val) = 0;
		virtual bool setValueUInt32(uint32 Col, int32 val) = 0;
		virtual bool setValueInt64(uint32 Col, int64 val) = 0;
		virtual bool setValueUInt64(uint32 Col, int64 val) = 0;
		virtual bool setValueFloat(uint32 Col, float val) = 0;
		virtual bool setValueDouble(uint32 Col, double val) = 0;
		virtual bool setValueBlob(uint32 Col, const CommonLib::CBlob& blob) = 0;
		virtual bool setValueBlob(uint32 Col, const byte* pBuf, uint32 nSize) = 0;
		virtual bool setValueText(uint32 Col, const CommonLib::CString& sting) = 0;
		virtual bool setValueText(uint32 Col, const char* pBuf, uint32 nLen) = 0;
		virtual bool setValueShape(uint32 Col,const CommonLib::CGeoShape& shape) = 0;
	};


	struct ITransaction : public CommonLib::AutoRefCounter
	{
	public:
		ITransaction(){}
		virtual ~ITransaction(){}


		virtual eTransactionType getType() const = 0;
		virtual bool begin() = 0;
		virtual bool commit() = 0;
		virtual bool rollback() = 0;
		virtual bool isError() const = 0 ;
		virtual size_t getErrorMessageSize() const = 0;
		virtual size_t getErroMessage(wchar_t * pBuf, size_t nSize) const = 0;

		virtual IStatementPtr createStatement(const wchar_t *pszSQLQuery) = 0;
		virtual ICursorPtr executeQuery(IStatement* pStatement) = 0;
		virtual ICursorPtr executeQuery(const wchar_t* pszQuery = NULL) = 0;
		virtual ICursorPtr executeSpatialQuery(const CommonLib::bbox& extent, const wchar_t *pszTable, const wchar_t* pszSpatialField, SpatialQueryMode mode = sqmIntersect,  IFieldSet *pFileds = 0) = 0; // For test
		virtual ICursorPtr executeSelectQuery(const wchar_t *pszTable, IFieldSet *pFileds = 0, const wchar_t *pszSQLQuery = NULL) = 0; // For test


		virtual IInsertCursorPtr createInsertCursor(const wchar_t *pszTable, IFieldSet *pFileds = 0) = 0;
		virtual IUpdateCursorPtr createUpdateCursor() = 0;
		virtual IDeleteCursorPtr createDeleteCursor(const wchar_t *pszTable) = 0;

	};
	

	
	struct  IDatabase: public CommonLib::AutoRefCounter
	{
	public:
		IDatabase(){}
		virtual ~IDatabase(){}
		virtual bool open(const wchar_t* pszName, DBTransactionMode mode = eTMMultiTransactions, const wchar_t* pszWorkingPath = NULL, const wchar_t* pszPassword = NULL)  = 0;
		virtual bool create(const wchar_t* pszDbName,/* size_t nPageSize, */DBTransactionMode mode = eTMMultiTransactions, const wchar_t* pszWorkingPath = NULL, const wchar_t* pszPassword = NULL)  = 0;
		virtual bool close()  = 0;
		virtual ITransactionPtr startTransaction(eTransactionType trType) = 0;
		virtual bool closeTransaction(ITransaction* ) = 0;
		virtual ISchemaPtr getSchema() const = 0;
		
		static IDatabasePtr CreateDatabase();
	};


}


#endif