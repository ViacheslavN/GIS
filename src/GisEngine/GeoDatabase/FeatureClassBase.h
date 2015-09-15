#ifndef GIS_ENGINE_GEO_DATABASE_FEATURE_CLASS_BASE_H
#define GIS_ENGINE_GEO_DATABASE_FEATURE_CLASS_BASE_H
#include "TableBase.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
		template<class I>
		class IFeatureClassBase : public ITableBase<I>
		{
			public:
				typedef ITableBase<I> TBase;
				IFeatureClassBase(IWorkspace *pWks) : TBase(pWks),
					m_ShapeType(CommonLib::shape_type_null)
				{
					m_DatasetType = dtFeatureClass;
					
				}
				virtual ~IFeatureClassBase()
				{

				}
				virtual CommonLib::eShapeType GetGeometryType() const
				{
					return m_ShapeType;
				}
				virtual const CommonLib::CString&         GetShapeFieldName() const
				{
					return m_sShapeFieldName;
				}
				virtual GisGeometry::IEnvelopePtr			 GetExtent() const
				{
					return m_pExtent;
				}
				virtual GisGeometry::ISpatialReferencePtr GetSpatialReference() const
				{
					return m_pSpatialReferencePtr;
				}
			protected:

				mutable GisGeometry::IEnvelopePtr  m_pExtent;
				mutable CommonLib::CString m_sShapeFieldName;
				mutable CommonLib::eShapeType m_ShapeType;
				mutable GisGeometry::ISpatialReferencePtr m_pSpatialReferencePtr;

		};
	}
}

#endif