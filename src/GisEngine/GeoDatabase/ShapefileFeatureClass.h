#ifndef GIS_ENGINE_GEO_DATABASE_SHAPE_FEATURE_CLASS_H_
#define GIS_ENGINE_GEO_DATABASE_SHAPE_FEATURE_CLASS_H_


#include "GeoDatabase.h"
#include "ShapefileUtils.h"

namespace GisEngine
{
	namespace GeoDatabase
	{
		class CShapefileFeatureClass : IFeatureClass
		{
		public:
			CShapefileFeatureClass(IWorkspace *pWorkSpace, const CommonLib::str_t& sPath, const CommonLib::str_t& sName, const CommonLib::str_t& sViewName);
			~CShapefileFeatureClass();

			//IDataset
			virtual eDatasetType  GetDatasetType() const {return dtFeatureClass;}
			virtual IWorkspace*    GetWorkspace() const {return m_pWorkSpace;}
			virtual const CommonLib::str_t&   GetDatasetName() const {return m_sName;}
			virtual const CommonLib::str_t&   GetDatasetViewName() const {return m_sViewName;}

			//ITable
			virtual void                 AddField(IField* field);
			virtual void                 DeleteField(const CommonLib::str_t& fieldName);
			virtual IFieldsPtr             GetFields() const;
			virtual bool                 HasOIDField() const;
			virtual const CommonLib::str_t& GetOIDFieldName() const;
			virtual IRowPtr				  GetRow(int64 id);
			virtual ICursorPtr			  Search(IQueryFilter* filter, bool recycling);

			//IFeatureClass
			virtual CommonLib::eShapeType GetGeometryType() const;
			virtual const CommonLib::str_t&         GetShapeFieldName() const;
			virtual GisGeometry::IEnvelopePtr			 GetExtent() const ;
			virtual GisGeometry::ISpatialReferencePtr GetSpatialReference() const;


			bool reload(bool write);
			void close();
			ShapefileUtils::SHPGuard* GetSHP();
			ShapefileUtils::DBFGuard* GetDBF();
		private:
			IWorkspace *m_pWorkSpace;
			IShapeFieldPtr m_pShapeField;
			IFieldPtr m_pOIDField;
			IFieldsPtr m_FieldsPtr;

			CommonLib::str_t m_sPath;
			CommonLib::str_t m_sName;
			CommonLib::str_t m_sViewName;
			CommonLib::str_t m_sShapeFieldName;
			CommonLib::str_t m_sOIDName;


			ShapefileUtils::SHPGuard m_shp;
			ShapefileUtils::DBFGuard m_dbf;
			GisGeometry::IEnvelopePtr	 m_pExtent;
			CommonLib::eShapeType m_ShapeType;
			GisGeometry::ISpatialReferencePtr m_pSpatialReferencePtr;
			

	

		};
	}
}

#endif