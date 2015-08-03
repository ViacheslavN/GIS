#ifndef GIS_ENGINE_GEO_DATABASE_H_
#define GIS_ENGINE_GEO_DATABASE_H_

#include "CommonLibrary/str_t.h" 
#include "Common/GisEngineCommon.h"
#include "CommonLibrary/Variant.h"
#include "CommonLibrary/IGeoShape.h"
#include "GisGeometry/Geometry.h"
#include "CommonLibrary/IRefCnt.h"

namespace GisEngine
{
	namespace GeoDatabase
	{

		enum eWorkspaceID
		{
			wiShapeFile = 1,
			wiEmbDB,
			wiSpatialLite,
			wiOracle,
			wiMSSQL,
			wiPostGIS

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

		COMMON_LIB_REFPTR_TYPEDEF(IRow);
		COMMON_LIB_REFPTR_TYPEDEF(IQueryFilter);
		COMMON_LIB_REFPTR_TYPEDEF(IField);
		COMMON_LIB_REFPTR_TYPEDEF(IGeometryDef);
		COMMON_LIB_REFPTR_TYPEDEF(IDataset);
		COMMON_LIB_REFPTR_TYPEDEF(IFields);
		COMMON_LIB_REFPTR_TYPEDEF(ITable);
		COMMON_LIB_REFPTR_TYPEDEF(IFeatureClass);
		COMMON_LIB_REFPTR_TYPEDEF(IDomain);
		COMMON_LIB_REFPTR_TYPEDEF(IShapeField);
		COMMON_LIB_REFPTR_TYPEDEF(ICursor);

		struct IWorkspace  : public CommonLib::AutoRefCounter
		{
				IWorkspace(){}
				virtual ~IWorkspace(){}
				virtual const CommonLib::str_t& GetWorkspaceName() const = 0; 
				virtual Common::IPropertySetPtr GetConnectionProperties() const = 0; 
				virtual eWorkspaceID GetWorkspaceID() const = 0;
				//virtual IDatasetContainer* GetDatasetContainer() = 0;
				virtual uint32 GetDatasetCount() const = 0;
				virtual IDatasetPtr GetDataset(uint32 nIdx) const = 0;
				virtual void RemoveDataset(uint32 nIdx) = 0;
				virtual void RemoveDataset(IDataset *pDataset) = 0;

				virtual ITablePtr  CreateTable(const CommonLib::str_t& name, IFields* fields) = 0;
				virtual IFeatureClassPtr CreateFeatureClass(const CommonLib::str_t& name, IFields* fields, const CommonLib::str_t& shapeFieldName) = 0;

				virtual ITablePtr OpenTable(const CommonLib::str_t& name) = 0;
				virtual IFeatureClassPtr OpenFeatureClass(const CommonLib::str_t& name) = 0;

		};


		/*struct IDatasetContainer 
		{
			IDatasetContainer();
			virtual ~IDatasetContainer();
			virtual void reset() = 0;
			virtual bool next(IDataset** pObj) = 0;
		};*/
		
		struct IDataset : public CommonLib::AutoRefCounter
		{
			IDataset(){}
			virtual ~IDataset(){}
			virtual eDatasetType  GetDatasetType() const = 0;
			virtual IWorkspace*    GetWorkspace() const = 0;
			virtual const CommonLib::str_t&   GetDatasetName() const = 0;
			virtual const CommonLib::str_t&   GetDatasetViewName() const = 0;
		};

 

		struct ITable : public IDataset
		{
			ITable(){}
			virtual ~ITable(){}
			virtual void                 AddField(IField* field) = 0;
			virtual void                 DeleteField(const CommonLib::str_t& fieldName) = 0;
			virtual IFieldsPtr             GetFields() const  = 0;
			virtual bool                 HasOIDField() const = 0;
			virtual const CommonLib::str_t& GetOIDFieldName() const = 0;
			virtual IRowPtr				  GetRow(int64 id) = 0;
			virtual ICursorPtr			  Search(IQueryFilter* filter, bool recycling) = 0;
		};


		struct IFeatureClass : public ITable 
		{
			IFeatureClass(){}
			virtual ~IFeatureClass(){}
			virtual CommonLib::eShapeType GetGeometryType() const = 0;
			virtual const CommonLib::str_t&         GetShapeFieldName() const = 0;
			virtual const GisBoundingBox& GetExtent() const = 0;
			virtual Geometry::ISpatialReferencePtr GetSpatialReference() const = 0;
		};


		struct IField  : public CommonLib::AutoRefCounter
		{
			IField(){}
			virtual ~IField(){}
			virtual const CommonLib::str_t& GetName() const = 0;
			virtual void                 SetName( const CommonLib::str_t& name) = 0;
			virtual  const CommonLib::str_t&  GetAliasName() const = 0;
			virtual void                 SetAliasName(const   CommonLib::str_t&  name) = 0;
			virtual bool                 GetIsEditable() const = 0;
			virtual void                 SetIsEditable(bool flag) = 0;
			virtual bool                 GetIsNullable() const = 0;
			virtual void                 SetIsNullable(bool flag) = 0;
			virtual bool                 GetIsRequired() const = 0;
			virtual void                 SetIsRequired(bool flag) = 0;
			virtual CommonLib::eDataTypes      GetType() const = 0;
			virtual void                 SetType(CommonLib::eDataTypes type) = 0;
			virtual int                  GetLength() const = 0;
			virtual void                 SetLength(int length) = 0;
			virtual int                  GetPrecision() const = 0;
			virtual void                 SetPrecision(int precision) = 0;
			virtual int                  GetScale() const = 0;
			virtual void                 SetScale(int scale) = 0;
			virtual bool                 CheckValue(CommonLib::IVariant* value) = 0;
			virtual IDomainPtr           GetDomain() const = 0;
			virtual void                 SetDomain(IDomain* domain) = 0;
		};


		struct IFields: public CommonLib::AutoRefCounter
		{

			IFields(){}
			virtual ~IFields(){}
			virtual int       GetFieldCount() const = 0;
			virtual void      SetFieldCount(int count) = 0;
			virtual IFieldPtr GetField(int index) const = 0;
			virtual void      SetField(int index, IField* field) = 0;
			virtual void      AddField(IField* field) = 0;
			virtual void      RemoveField(int index) = 0;
			virtual int       FindField(const CommonLib::str_t& name) const = 0;
			virtual bool      FieldExists(const CommonLib::str_t& name) const = 0;
			virtual void	  Clear() = 0;
		};


		struct IFieldSet: public CommonLib::AutoRefCounter
		{
			IFieldSet(){}
			virtual ~IFieldSet(){}
			virtual int  GetCount() const = 0;
			virtual bool Find(const CommonLib::str_t& field) const = 0;
			virtual void Reset() = 0;
			virtual bool Next(CommonLib::str_t* field) = 0;
			virtual void Add(const CommonLib::str_t& field) = 0;
			virtual void Remove(const CommonLib::str_t& field) = 0;
			virtual void Clear() = 0;
		};


		struct IRow : public CommonLib::AutoRefCounter
		{
			IRow(){}
			virtual ~IRow(){}
			virtual IFieldSet*			   GetFieldSet() const = 0;
			virtual IFields*               GetSourceFields() const = 0;
			virtual bool                   IsFieldSelected(int index) const = 0;
			virtual CommonLib::IVariant*   GetValue(int index) const = 0;
			virtual void                   SetValue(int index, CommonLib::IVariant* value) = 0;
			virtual bool                   HasOID() const = 0;
			virtual int64                  GetOID() const = 0;
			virtual void                   SetOID(int64 id) = 0;
		};

		struct IFeature : public IRow
		{
			IFeature(){}
			virtual ~IFeature(){}
			virtual CommonLib::IGeoShape* GetShape() const = 0;
			virtual void                  SetShape(CommonLib::IGeoShape* pShape) = 0;
		};

		struct ICursor : public CommonLib::AutoRefCounter
		{
			ICursor(){}
			virtual ~ICursor(){}
			virtual bool NextRow(IRow** row) = 0;
		};


		struct  IInsertCursor : public ICursor
		{
			IInsertCursor(){}
			virtual ~IInsertCursor(){}
			virtual int InsertRow(IRow* pRow) = 0;
		};

		struct  IUpdateCursor : public ICursor
		{
			IUpdateCursor(){}
			virtual ~IUpdateCursor(){}
			virtual void UpdateRow(IRow* pRow) = 0;
		};

		struct IDeleteCursor : public ICursor
		{
			IDeleteCursor(){}
			virtual ~IDeleteCursor(){}
			virtual void DeleteRow(int64 oid) = 0;
		};

		struct  IOIDSet 
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
			virtual IFieldSet*                        GetFieldSet() const = 0;
			virtual void                              SetFieldSet(IFieldSet* fieldSet) = 0;
			virtual Geometry::ISpatialReference*      GetOutputSpatialReference() const = 0;
			virtual void                              SetOutputSpatialReference(Geometry::ISpatialReference* spatRef) = 0;
			virtual const CommonLib::str_t&           GetWhereClause() const = 0;
			virtual void                              SetWhereClause(const CommonLib::str_t& where) = 0;
			virtual IOIDSet*                          GetOIDSet() const = 0;
			virtual void                              SetOIDSet(IOIDSet* oidSet) = 0;
		};

		struct ISpatialFilter : public IQueryFilter
		{
	
			virtual const CommonLib::str_t&    GetShapeField() const = 0;
			virtual void                    SetShapeField(const CommonLib::str_t& name) = 0;
			virtual CommonLib::IGeoShape*	GetShape() const = 0;
			virtual void                    SetShape( CommonLib::IGeoShape* pShape) = 0;
			virtual double                  GetPrecision() const = 0; 
			virtual void                    SetPrecision(double precision) = 0;
		};



		struct  IGeometryDef : public CommonLib::AutoRefCounter
		{
			 
			virtual  CommonLib::eShapeType			  GetGeometryType() const = 0;
			virtual void                              SetGeometryType(CommonLib::eShapeType type) = 0;
			virtual bool                              GetHasZ() const = 0;
			virtual void                              SetHasZ(bool flag) = 0;
			virtual bool                              GetHasM() const = 0;
			virtual void                              SetHasM(bool flag) = 0;
			virtual Geometry::ISpatialReferencePtr    GetSpatialReference() const = 0;
			virtual void                              SetSpatialReference(Geometry::ISpatialReference* ref) = 0;
			virtual GisBoundingBox					  GetBaseExtent() const = 0;
			virtual void                              SetBaseExtent(const GisBoundingBox& box) = 0;
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
			virtual const CommonLib::str_t& GetName() const = 0;
			virtual void                 SetName(const CommonLib::str_t& name) = 0;
			virtual const CommonLib::str_t& GetDescription() const = 0;
			virtual void                 SetDescription(const CommonLib::str_t& description) = 0;
			virtual CommonLib::eDataTypes  GetFieldType() const = 0;
			virtual void                 SetFieldType(CommonLib::eDataTypes ) = 0;
		};
	}
}

#endif