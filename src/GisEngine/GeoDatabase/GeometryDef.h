#ifndef GIS_ENGINE_GEO_DATABASE_GEOMETRYDEF_H
#define GIS_ENGINE_GEO_DATABASE_GEOMETRYDEF_H

#include "GeoDatabase.h"

namespace GisEngine
{
	namespace GeoDatabase
	{

		class CGeometryDef : public IGeometryDef
		{

		public:
			CGeometryDef(CommonLib::eShapeType = CommonLib::shape_type_null, bool hasZ = false, bool hasM = false);
			virtual ~CGeometryDef();

		public:
			// IGeometryDef
			virtual CommonLib::eShapeType      GetGeometryType() const;
			virtual void SetGeometryType(CommonLib::eShapeType type);
			virtual bool GetHasZ() const;
			virtual void SetHasZ(bool flag);
			virtual bool GetHasM() const;
			virtual void SetHasM(bool flag);
			virtual GisGeometry::ISpatialReferencePtr GetSpatialReference() const;
			virtual void SetSpatialReference(GisGeometry::ISpatialReference* pSpRef);
			virtual GisBoundingBox	GetBaseExtent() const;
			virtual void SetBaseExtent(const GisBoundingBox& box);
		private:
			CommonLib::eShapeType m_geometryType;
			bool m_bIsHasZ;
			bool m_bHasM;
			GisGeometry::ISpatialReferencePtr m_pSpatialRefPtr;
			GisBoundingBox m_baseExtent;
		};

	}
}

#endif