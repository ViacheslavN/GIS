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
					m_ShapeType(CommonLib::shape_type_null), m_bAnnotation(false)
				{
					this->m_DatasetType = dtFeatureClass;
					
				}
				virtual ~IFeatureClassBase()
				{

				}
				virtual	void SetGeometryType(CommonLib::eShapeType shapeType)
				{
					m_ShapeType = shapeType;
				}
				virtual CommonLib::eShapeType GetGeometryType() const
				{
					return m_ShapeType;
				}
				virtual const CommonLib::CString&         GetShapeFieldName() const
				{
					return m_sShapeFieldName;
				}
				virtual void	SetShapeFieldName(const CommonLib::CString& sName)
				{
					m_sShapeFieldName = sName;
				}
				virtual bool	GetIsAnnoClass() const
				{
					return m_bAnnotation;
				}
				virtual const CommonLib::CString&	GetAnnoFieldName() const
				{
					return m_sAnnotationName;
				}
				virtual void	SetIsAnnoClass(bool bAnno)
				{
					m_bAnnotation = bAnno;
				}
				virtual void	SetAnnoFieldName(const CommonLib::CString& sAnnoName)
				{
					m_sAnnotationName = sAnnoName;
				}
				virtual GisGeometry::IEnvelopePtr			 GetExtent() const
				{
					return m_pExtent;
				}
				virtual GisGeometry::ISpatialReferencePtr GetSpatialReference() const
				{
					return m_pSpatialReference;
				}
				virtual void	SetExtent(GisGeometry::IEnvelope* pEnvelope)
				{
					m_pExtent = pEnvelope;
				}
				virtual void	SetSpatialReference(GisGeometry::ISpatialReference* pSpatRef)
				{
					m_pSpatialReference = pSpatRef;
				}
			protected:
				bool m_bAnnotation;
				CommonLib::CString m_sAnnotationName;
				mutable GisGeometry::IEnvelopePtr  m_pExtent;
				mutable CommonLib::CString m_sShapeFieldName;
				mutable CommonLib::eShapeType m_ShapeType;
				mutable GisGeometry::ISpatialReferencePtr m_pSpatialReference;

		};
	}
}

#endif