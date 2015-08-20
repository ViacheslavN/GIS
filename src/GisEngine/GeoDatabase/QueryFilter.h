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
			CQueryFilter(const CommonLib::str_t& sWhere = CommonLib::str_t(), IFieldSet* fieldSet = NULL, IOIDSet* oidSet = NULL);
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
			virtual const CommonLib::str_t& 		  GetWhereClause() const;
			virtual void                              SetWhereClause(const CommonLib::str_t& where);
			virtual IOIDSetPtr                        GetOIDSet() const;
			virtual void                              SetOIDSet(IOIDSet* oidSet);

			// ISpatialFilter
			virtual const CommonLib::str_t&     GetShapeField() const;
			virtual void                    SetShapeField(const CommonLib::str_t& name);
			virtual CommonLib::IGeoShapePtr GetShape() const;
			virtual void                    SetShape(CommonLib::CGeoShape* geom);
			virtual eSpatialRel             GetSpatialRel() const;
			virtual void                    SetSpatialRel(eSpatialRel rel);
			virtual double                  GetPrecision() const;
			virtual void                    SetPrecision(double precision);

		private:
			CommonLib::str_t             m_sWhereClause;
			mutable IFieldSetPtr              m_pFieldSet;
			mutable IOIDSetPtr                m_pOidSet;
			GisGeometry::ISpatialReferencePtr m_pOutputSpatialRef;
			CommonLib::str_t               m_sShapeField;
			CommonLib::IGeoShapePtr            m_pShape;
			eSpatialRel                     m_SpatialRel;
			double                            m_dPecision;

		};
	}
}

#endif