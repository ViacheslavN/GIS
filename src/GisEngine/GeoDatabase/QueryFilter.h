#ifndef GIS_ENGINE_GEO_DATABASE__QUERYFILTER_H
#define GIS_ENGINE_GEO_DATABASE__QUERYFILTER_H

#include "GeoDatabase.h"


namespace GisEngine
{
	namespace GeoDatabase
	{
		class  CQueryFilter : public ISpatialFilter
		{
		public:
			CQueryFilter(const CommonLib::CString& sWhere = CommonLib::CString(), IFieldSet* fieldSet = NULL, IOIDSet* oidSet = NULL);
			virtual ~CQueryFilter();

		private:
			CQueryFilter(const CQueryFilter&);
			CQueryFilter& operator=(const CQueryFilter&);

		public:
			// IQueryFilter
			virtual IFieldSetPtr                      GetFieldSet() const;
			virtual void                              SetFieldSet(IFieldSet* fieldSet);
			virtual GisGeometry::ISpatialReferencePtr    GetOutputSpatialReference() const;
			virtual void                              SetOutputSpatialReference(GisGeometry::ISpatialReference* spatRef);
			virtual const CommonLib::CString& 		  GetWhereClause() const;
			virtual void                              SetWhereClause(const CommonLib::CString& where);
			virtual IOIDSetPtr                        GetOIDSet() const;
			virtual void                              SetOIDSet(IOIDSet* oidSet);

			// ISpatialFilter
			virtual const CommonLib::CString&     GetShapeField() const;
			virtual void                    SetShapeField(const CommonLib::CString& name);
			virtual CommonLib::IGeoShapePtr GetShape() const;
			virtual void                    SetShape(CommonLib::CGeoShape* geom);
			virtual eSpatialRel             GetSpatialRel() const;
			virtual void                    SetSpatialRel(eSpatialRel rel);
			virtual double                  GetPrecision() const;
			virtual void                    SetPrecision(double precision);
			virtual GisBoundingBox			GetBB() const;
			virtual void                    SetBB(const  GisBoundingBox& bbox );
		private:
			CommonLib::CString             m_sWhereClause;
			mutable IFieldSetPtr              m_pFieldSet;
			mutable IOIDSetPtr                m_pOidSet;
			GisGeometry::ISpatialReferencePtr m_pOutputSpatialRef;
			CommonLib::CString               m_sShapeField;
			CommonLib::IGeoShapePtr            m_pShape;
			eSpatialRel                     m_SpatialRel;
			double                            m_dPecision;
			GisBoundingBox					m_bbox;

		};
	}
}

#endif