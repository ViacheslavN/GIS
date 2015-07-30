#ifndef GIS_ENGINE_GEO_DATABASE_SHAPE_FEATURE_CLASS_H_
#define GIS_ENGINE_GEO_DATABASE_SHAPE_FEATURE_CLASS_H_


#include "GeoDatabase.h"
#include "ShapefileUtils.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
		class CShapefileFeatureClass : public IFeatureClass
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
			virtual IFields*             GetFields() const;
			virtual bool                 HasOIDField() const;
			virtual const CommonLib::str_t& GetOIDFieldName() const;
			virtual IRow*				  GetRow(int64 id);
			virtual ICursor*			  Search(IQueryFilter* filter, bool recycling);

			//IFeatureClass
			virtual CommonLib::eShapeType GetGeometryType() const;
			virtual bool     IsGeometryTypeSupported(CommonLib::eShapeType type) const;
			virtual const CommonLib::str_t&         GetShapeFieldName() const;
			virtual const GisBoundingBox& GetExtent() const;
			virtual Geometry::ISpatialReference* GetSpatialReference() const;


			bool load(bool write);
		private:
			IWorkspace *m_pWorkSpace;
			CommonLib::str_t m_sPath;
			CommonLib::str_t m_sName;
			CommonLib::str_t m_sViewName;

			ShapefileUtils::SHPGuard m_shp;
			ShapefileUtils::DBFGuard m_dbf;
			CommonLib::eShapeType m_ShapeType;
			CommonLib::str_t m_sShapeFieldName;
			GisBoundingBox m_Extent;
			std::auto_ptr<Geometry::ISpatialReference> m_pSpatialReference;

		};
	}
}

#endif