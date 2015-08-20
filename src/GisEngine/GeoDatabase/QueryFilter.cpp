#include "stdafx.h"
#include "QueryFilter.h"
#include "OIDSet.h"
#include "FieldSet.h"


namespace GisEngine
{
	namespace GeoDatabase
	{
		CQueryFilter::CQueryFilter(const CommonLib::str_t& sWhere, IFieldSet* fieldSet, IOIDSet* oidSet)
			: m_sWhereClause(sWhere)
			, m_pFieldSet(fieldSet)
			, m_pOidSet(oidSet)
			, m_SpatialRel(srlUndefined)
			, m_dPecision(0.0)
		{}

		CQueryFilter::~CQueryFilter()
		{}

		
		// IQueryFilter
		IFieldSetPtr CQueryFilter::GetFieldSet() const
		{
			if(!m_pFieldSet)
				m_pFieldSet = new CFieldSet();

			return m_pFieldSet;
		}

		void CQueryFilter::SetFieldSet(IFieldSet* fieldSet)
		{
			m_pFieldSet = fieldSet;
		}

		GisGeometry::ISpatialReferencePtr CQueryFilter::GetOutputSpatialReference() const
		{
			return m_pOutputSpatialRef;
		}

		void CQueryFilter::SetOutputSpatialReference(GisGeometry::ISpatialReference* spatRef)
		{
			m_pOutputSpatialRef = spatRef;
		}

		const CommonLib::str_t& CQueryFilter::GetWhereClause() const
		{
			return m_sWhereClause;
		}

		void CQueryFilter::SetWhereClause(const CommonLib::str_t& sWhere)
		{
			m_sWhereClause = sWhere;
		}

		IOIDSetPtr CQueryFilter::GetOIDSet() const
		{
			if(!m_pOidSet)
				m_pOidSet = new COIDSet();

			return m_pOidSet;
		}

		void CQueryFilter::SetOIDSet(IOIDSet* oidSet)
		{
			m_pOidSet = oidSet;
		}

		// ISpatialFilter
		const CommonLib::str_t& CQueryFilter::GetShapeField() const
		{
			return m_sShapeField;
		}

		void CQueryFilter::SetShapeField(const CommonLib::str_t& name)
		{
			m_sShapeField = name;
		}

		CommonLib::IGeoShapePtr CQueryFilter::GetShape() const
		{
			return m_pShape;
		}

		void CQueryFilter::SetShape(CommonLib::CGeoShape* pShape)
		{
			m_pShape = pShape;
		}

		eSpatialRel CQueryFilter::GetSpatialRel() const
		{
			return m_SpatialRel;
		}

		void CQueryFilter::SetSpatialRel(eSpatialRel rel)
		{
			m_SpatialRel = rel;
		}

		double CQueryFilter::GetPrecision() const
		{
			return m_dPecision;
		}

		void CQueryFilter::SetPrecision(double precision)
		{
			m_dPecision = precision;
		}
			
	}
}