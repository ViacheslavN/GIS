#include "stdafx.h"
#include "GeometryDef.h"

namespace GisEngine
{
	namespace GeoDatabase
	{

		CGeometryDef::CGeometryDef(CommonLib::eShapeType type, bool hasZ, bool hasM)
			: m_geometryType(type)
			, m_bIsHasZ(hasZ)
			, m_bHasM(hasM)
		{
			m_baseExtent.type = CommonLib::bbox_type_null;
		}

		CGeometryDef::~CGeometryDef()
		{}

		
		// IGeometryDef
		CommonLib::eShapeType CGeometryDef::GetGeometryType() const
		{
			return m_geometryType;
		}

		void CGeometryDef::SetGeometryType(CommonLib::eShapeType type)
		{
			m_geometryType = type;
		}

		bool CGeometryDef::GetHasZ() const
		{
			return m_bIsHasZ;
		}

		void CGeometryDef::SetHasZ(bool flag)
		{
			m_bIsHasZ = flag;
		}

		bool CGeometryDef::GetHasM() const
		{
			return m_bHasM;
		}

		void CGeometryDef::SetHasM(bool flag)
		{
			m_bHasM = flag;
		}

		GisGeometry::ISpatialReferencePtr CGeometryDef::GetSpatialReference() const
		{
			return m_pSpatialRefPtr;
		}

		void CGeometryDef::SetSpatialReference(GisGeometry::ISpatialReference* pSpRef)
		{
			m_pSpatialRefPtr = pSpRef;
		}

		GisBoundingBox CGeometryDef::GetBaseExtent() const
		{
			return m_baseExtent;
		}

		void CGeometryDef::SetBaseExtent(const GisBoundingBox& box)
		{
			m_baseExtent = box;
		}
		IGeometryDefPtr CGeometryDef::clone() const
		{
			IGeometryDefPtr pGeomDef(new CGeometryDef());
			pGeomDef->SetBaseExtent(GetBaseExtent());
			pGeomDef->SetHasM(GetHasM());
			pGeomDef->SetHasZ(GetHasZ());
			pGeomDef->SetGeometryType(GetGeometryType());
			if(GetSpatialReference().get())
				pGeomDef->SetSpatialReference(GetSpatialReference()->clone().get());

			return pGeomDef;
		}
	}
}