#ifndef GIS_ENGINE_GEO_DATABASE_H_
#define GIS_ENGINE_GEO_DATABASE_H_

#include <set>

#include "CommonLibrary/String.h" 
#include "Common/GisEngineCommon.h"
#include "CommonLibrary/Variant.h"
#include "CommonLibrary/IGeoShape.h"
#include "GisGeometry/Geometry.h"
#include "CommonLibrary/IRefCnt.h"

namespace GisEngine
{
	namespace GeoDatabase
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
			dtOid32,
			dtOid64,
			dtFloat,
			dtDouble,
			dtString,
			dtBlob,
			dtRaster,
			dtDate,
			dtGeometry,
			dtSerializedObject,
			dtAnnotation 
		};

		enum eWorkspaceType
		{
			wtUndefined = 0,
			wtShapeFile,
			wtEmbDB,
			wtSqlLite,
			wtOracle,
			wtMSSQL,
			wtPostGIS

		};
		enum eDatasetType
		{
			dtUndefined    = 0,
			dtTypeTable    = 1,
			dtFeatureClass = 2,
			dtGroupDataset = 4,
			dtTypeRaster   = 8,
			dtTypeAny      = -1
		};

		enum eSpatialRel
		{
			srlUndefined          = 0,
			srlIntersects         = 1,
			srlTouches            = 2,
			srlOverlaps           = 3,
			srlCrosses            = 4,
			srlWithin             = 5,
			srlContains           = 6,
			srlRelation           = 7
		};


	 

		struct IField;
		struct IRow;
		struct IQueryFilter;
		struct ICursor;
		struct IDatasetContainer;
		struct IDataset;
		struct IFields;
		struct IFeatureClass;
		struct ITable;
		struct IGeometryDef;
		struct IDomain;
		struct IShapeField;
		struct IFieldSet;
		struct IOIDSet;
		struct IWorkspace;
		struct IFeature;
		struct IInsertCursor;
		struct IUpdateCursor;
		struct IDeleteCursor;
		struct ITransaction;

		COMMON_LIB_REFPTR_TYPEDEF(IRow);
		COMMON_LIB_REFPTR_TYPEDEF(IQueryFilter);
		COMMON_LIB_REFPTR_TYPEDEF(IField);
		COMMON_LIB_REFPTR_TYPEDEF(IGeometryDef);
		COMMON_LIB_REFPTR_TYPEDEF(IDataset);
		COMMON_LIB_REFPTR_TYPEDEF(IFields);
		COMMON_LIB_REFPTR_TYPEDEF(IFieldSet);
		COMMON_LIB_REFPTR_TYPEDEF(ITable);
		COMMON_LIB_REFPTR_TYPEDEF(IFeatureClass);
		COMMON_LIB_REFPTR_TYPEDEF(IDomain);
		COMMON_LIB_REFPTR_TYPEDEF(IShapeField);
		COMMON_LIB_REFPTR_TYPEDEF(ICursor);
		COMMON_LIB_REFPTR_TYPEDEF(IOIDSet);
		COMMON_LIB_REFPTR_TYPEDEF(IWorkspace);
		COMMON_LIB_REFPTR_TYPEDEF(IFeature);
		COMMON_LIB_REFPTR_TYPEDEF(IInsertCursor);
		COMMON_LIB_REFPTR_TYPEDEF(IUpdateCursor);
		COMMON_LIB_REFPTR_TYPEDEF(IDeleteCursor);
		COMMON_LIB_REFPTR_TYPEDEF(ITransaction);
		
		struct IWorkspace  : public CommonLib::AutoRefCounter, 
							 public GisCommon::IStreamSerialize,
							 public GisCommon::IXMLSerialize
		{
				IWorkspace(){}
				virtual ~IWorkspace(){}

				virtual int32 GetID() const = 0;
				virtual const CommonLib::CString& GetHash() const = 0;
				virtual const CommonLib::CString& GetWorkspaceName() const = 0; 
				virtual GisCommon::IPropertySetPtr GetConnectionProperties() const = 0; 
				virtual eWorkspaceType GetWorkspaceType() const = 0;
				//virtual IDatasetContainer* GetDatasetContainer() = 0;
				virtual uint32 GetDatasetCount() const = 0;
				virtual IDatasetPtr GetDataset(uint32 nIdx) const = 0;
				virtual void RemoveDataset(uint32 nIdx) = 0;
				virtual void RemoveDataset(IDataset *pDataset) = 0;

				virtual ITablePtr  CreateTable(const CommonLib::CString& name, IFields* fields, const CommonLib::CString& sOIDName = L"") = 0;
				virtual IFeatureClassPtr CreateFeatureClass(const CommonLib::CString& name,
							IFields* fields, const CommonLib::CString& sOIDName = L"",  
							const CommonLib::CString& shapeFieldName = L"",
							const CommonLib::CString& sAnnotationName = L"") = 0;					

				virtual ITablePtr OpenTable(const CommonLib::CString& name) = 0;
				virtual IFeatureClassPtr OpenFeatureClass(const CommonLib::CString& name) = 0;

				virtual ITablePtr GetTable(const CommonLib::CString& name) = 0;
				virtual IFeatureClassPtr GetFeatureClass(const CommonLib::CString& name) = 0;

				virtual bool IsError() const = 0;
				virtual uint32 GetErrorCode() const = 0;
				virtual void GetErrorText( CommonLib::CString& sStr, uint32 nCode) = 0;

				virtual ITransactionPtr startTransaction() = 0;
			

				static IWorkspacePtr GetWorkspaceByID(uint32 nID);

				static bool SaveWks(CommonLib::IWriteStream *pStream);
				static bool LoadWks(CommonLib::IReadStream *pStream);

				static bool SaveWks(GisCommon::IXMLNode *pXML);
				static bool LoadWks(const  GisCommon::IXMLNode *pXML);
		};



		struct  ITransaction : public CommonLib::AutoRefCounter
		{
			public:
				 ITransaction(){}
				virtual ~ ITransaction(){}
				virtual bool commit() = 0;
				virtual bool rollback() = 0;
				virtual void GetError(CommonLib::CString& sText) = 0;

				virtual IInsertCursorPtr CreateInsertCusor(ITable *pTable, IFieldSet *pFileds = 0) = 0;
				virtual IUpdateCursorPtr CreateUpdateCusor(ITable *pTable, IFieldSet *pFileds = 0) = 0;
				virtual IDeleteCursorPtr CreateDeleteCusor(ITable *pTable, IFieldSet *pFileds = 0) = 0;


		};

		/*struct IDatasetContainer 
		{
			IDatasetContainer();
			virtual ~IDatasetContainer();
			virtual void reset() = 0;
			virtual bool next(IDataset** pObj) = 0;
		};*/
		
		struct IDataset : public CommonLib::AutoRefCounter, 
						  public GisCommon::IStreamSerialize,
						  public GisCommon::IXMLSerialize
		{
			IDataset(){}
			virtual ~IDataset(){}
			virtual eDatasetType  GetDatasetType() const = 0;
			virtual IWorkspacePtr    GetWorkspace() const = 0;
			virtual const CommonLib::CString&   GetDatasetName() const = 0;
			virtual const CommonLib::CString&   GetDatasetViewName() const = 0;
		};

 

		struct ITable : public IDataset
		{
			ITable(){}
			virtual ~ITable(){}
			virtual void						 AddField(IField* field) = 0;
			virtual void						 DeleteField(const CommonLib::CString& fieldName) = 0;
			virtual IFieldsPtr					 GetFields() const  = 0;
			virtual void						 SetFields(IFields *pFields)  = 0;
			virtual bool						 HasOIDField() const = 0;
			virtual void						 SetHasOIDField(bool bFlag) = 0;
			virtual const CommonLib::CString&	 GetOIDFieldName() const = 0;
			virtual void						 SetOIDFieldName(const CommonLib::CString& sOIDFieldName) = 0;
			virtual IRowPtr						 GetRow(int64 id) = 0;
			virtual ICursorPtr					 Search(IQueryFilter* filter, bool recycling) = 0;
		};


		struct IFeatureClass : public ITable 
		{
			IFeatureClass(){}
			virtual ~IFeatureClass(){}
			virtual CommonLib::eShapeType				 GetGeometryType() const = 0;
			virtual	void								 SetGeometryType(CommonLib::eShapeType shapeType)	= 0;
			virtual const CommonLib::CString&			 GetShapeFieldName() const = 0;
			virtual void						         SetShapeFieldName(const CommonLib::CString& sName)  = 0;
			virtual bool								 GetIsAnnoClass() const = 0;
			virtual const CommonLib::CString&			 GetAnnoFieldName() const = 0;
			virtual void								 SetIsAnnoClass(bool bAnno) = 0;
			virtual void								 SetAnnoFieldName(const CommonLib::CString& sAnnoName) = 0;
			virtual GisGeometry::IEnvelopePtr			 GetExtent() const = 0;
			virtual GisGeometry::ISpatialReferencePtr	 GetSpatialReference() const = 0;
			virtual void								 SetExtent(GisGeometry::IEnvelope* pEnvelope)  = 0;
			virtual void								 SetSpatialReference(GisGeometry::ISpatialReference* pSpatRef)  = 0;
		};


		struct IField  : public CommonLib::AutoRefCounter
		{
			IField(){}
			virtual ~IField(){}
			virtual const CommonLib::CString& GetName() const = 0;
			virtual void                 SetName( const CommonLib::CString& name) = 0;
			virtual  const CommonLib::CString&  GetAliasName() const = 0;
			virtual void                 SetAliasName(const   CommonLib::CString&  name) = 0;
			virtual bool                 GetIsEditable() const = 0;
			virtual void                 SetIsEditable(bool flag) = 0;
			virtual bool                 GetIsNullable() const = 0;
			virtual void                 SetIsNullable(bool flag) = 0;
			virtual bool                 GetIsRequired() const = 0;
			virtual void                 SetIsRequired(bool flag) = 0;
			virtual eDataTypes			 GetType() const = 0;
			virtual void                 SetType(eDataTypes type) = 0;
			virtual int                  GetLength() const = 0;
			virtual void                 SetLength(int length) = 0;
			virtual int                  GetPrecision() const = 0;
			virtual void                 SetPrecision(int precision) = 0;
			virtual int                  GetScale() const = 0;
			virtual void                 SetScale(int scale) = 0;
			virtual IDomainPtr           GetDomain() const = 0;
			virtual void                 SetDomain(IDomain* domain) = 0;
			virtual const CommonLib::CVariant& 	 GetDefaultValue() const = 0;
			virtual void					 SetIsDefault(const CommonLib::CVariant& value) = 0;
			virtual bool                 GetIsPrimaryKey() const = 0;
			virtual void                 SetIsPrimaryKey(bool flag) = 0;
			virtual IFieldPtr			 clone() const = 0;
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
			virtual IFieldsPtr		clone() const = 0;
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


		struct IRow : public CommonLib::AutoRefCounter
		{
			IRow(){}
			virtual ~IRow(){}
			virtual IFieldSetPtr		   GetFieldSet() const = 0;
			virtual IFieldsPtr             GetSourceFields() const = 0;
			virtual bool                   IsFieldSelected(int index) const = 0;
			virtual const CommonLib::CVariant*   GetValue(int index) const = 0;
			virtual void                   SetValue(int index, const CommonLib::CVariant& value) = 0;
			virtual CommonLib::CVariant*   GetValue(int index) = 0;
			virtual bool                   HasOID() const = 0;
			virtual int64                  GetOID() const = 0;
			virtual void                   SetOID(int64 id) = 0;
		};

		struct IFeature : public IRow
		{
			IFeature(){}
			virtual ~IFeature(){}
			virtual CommonLib::IGeoShapePtr GetShape() const = 0;
			virtual void                  SetShape(CommonLib::CGeoShape* pShape) = 0;
		};

		struct ICursor : public CommonLib::AutoRefCounter
		{
			ICursor(){}
			virtual ~ICursor(){}
			virtual IFieldSetPtr GetFieldSet() const = 0;
			virtual IFieldsPtr   GetSourceFields() const = 0;
			virtual bool         IsFieldSelected(int index) const = 0;
			virtual bool NextRow(IRowPtr* row) = 0;
		};

		struct ISpatialCursor : ICursor
		{
			ISpatialCursor(){}
			virtual ~ISpatialCursor(){}
			virtual bool NextRow(IFeaturePtr* row) = 0;
		};


		struct  IInsertCursor : public CommonLib::AutoRefCounter
		{
			IInsertCursor(){}
			virtual ~IInsertCursor(){}
			virtual IFieldSetPtr GetFieldSet() const = 0;
			virtual IFieldsPtr   GetSourceFields() const = 0;
			virtual int64 InsertRow(IRow* pRow) = 0;
		};

		struct  IUpdateCursor : public ICursor
		{
			IUpdateCursor(){}
			virtual ~IUpdateCursor(){}
			virtual void UpdateRow(IRow* pRow) = 0;
		};

		struct IDeleteCursor  : public CommonLib::AutoRefCounter
		{
			IDeleteCursor(){}
			virtual ~IDeleteCursor(){}
			virtual void DeleteRow(int64 oid) = 0;
		};

		struct  IOIDSet : public CommonLib::AutoRefCounter
		{
			IOIDSet(){}
			virtual ~IOIDSet(){}
			virtual int  GetCount() const = 0;
			virtual bool Find(int id) const = 0;
			virtual void Reset() = 0;
			virtual bool Next(int* id) = 0;
			virtual void Add(int id) = 0;
			virtual void AddList(const int* id, int count) = 0;
			virtual void Remove(int id) = 0;
			virtual void RemoveList(const int* id, int count) = 0;
			virtual void Clear() = 0;
		};


		struct IQueryFilter : public CommonLib::AutoRefCounter
		{
			IQueryFilter(){}
			virtual ~IQueryFilter(){}
			virtual IFieldSetPtr                        GetFieldSet() const = 0;
			virtual void                                SetFieldSet(IFieldSet* fieldSet) = 0;
			virtual GisGeometry::ISpatialReferencePtr   GetOutputSpatialReference() const = 0;
			virtual void								SetOutputSpatialReference(GisGeometry::ISpatialReference* spatRef) = 0;
			virtual const CommonLib::CString&           GetWhereClause() const = 0;
			virtual void								SetWhereClause(const CommonLib::CString& where) = 0;
			virtual IOIDSetPtr                          GetOIDSet() const = 0;
			virtual void								SetOIDSet(IOIDSet* oidSet) = 0;
		};

		struct ISpatialFilter : public IQueryFilter
		{
	
			virtual const CommonLib::CString&    GetShapeField() const = 0;
			virtual void						 SetShapeField(const CommonLib::CString& name) = 0;
			virtual CommonLib::IGeoShapePtr		 GetShape() const = 0;
			virtual void						 SetShape( CommonLib::CGeoShape* pShape) = 0;
			virtual GisBoundingBox				 GetBB() const = 0;
			virtual void						 SetBB(const  GisBoundingBox& bbox ) = 0;
			virtual double						 GetPrecision() const = 0; 
			virtual void						 SetPrecision(double precision) = 0;
			virtual eSpatialRel					 GetSpatialRel() const = 0;
			virtual void						 SetSpatialRel(eSpatialRel rel) = 0;
		};



		struct  IGeometryDef : public CommonLib::AutoRefCounter
		{
			 
			virtual  CommonLib::eShapeType			  GetGeometryType() const = 0;
			virtual void                              SetGeometryType(CommonLib::eShapeType type) = 0;
			virtual bool                              GetHasZ() const = 0;
			virtual void                              SetHasZ(bool flag) = 0;
			virtual bool                              GetHasM() const = 0;
			virtual void                              SetHasM(bool flag) = 0;
			virtual GisGeometry::ISpatialReferencePtr    GetSpatialReference() const = 0;
			virtual void                              SetSpatialReference(GisGeometry::ISpatialReference* ref) = 0;
			virtual GisBoundingBox					  GetBaseExtent() const = 0;
			virtual void                              SetBaseExtent(const GisBoundingBox& box) = 0;
			virtual IGeometryDefPtr					 clone() const = 0;
		};
	

		struct  IShapeField : public IField
		{
			IShapeField(){}
			virtual ~IShapeField(){}
			virtual IGeometryDefPtr  GetGeometryDef() const = 0;
			virtual void            SetGeometryDef(IGeometryDef* def) = 0;
		};


		struct IDomain : public CommonLib::AutoRefCounter
		{
			IDomain(){}
			virtual ~IDomain(){}
			virtual const CommonLib::CString&		GetName() const = 0;
			virtual void							SetName(const CommonLib::CString& name) = 0;
			virtual const CommonLib::CString&		GetDescription() const = 0;
			virtual void							SetDescription(const CommonLib::CString& description) = 0;
			virtual CommonLib::eDataTypes			GetFieldType() const = 0;
			virtual void							SetFieldType(CommonLib::eDataTypes ) = 0;
			virtual IDomainPtr						clone() const = 0;
		};
	}
}

#endif