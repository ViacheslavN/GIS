#ifndef GIS_ENGINE_GEOMETRY_I_SPATIAL_REFERENCE_PROJ4_H_
#define GIS_ENGINE_GEOMETRY_I_SPATIAL_REFERENCE_PROJ4_H_

#include "Geometry.h"


namespace GisEngine
{
	namespace Geometry
	{
		class CSpatialReferenceProj4 : public ISpatialReference
		{
		public:
			 typedef void* Handle;

			CSpatialReferenceProj4(const CommonLib::str_t& prj4Str);
			CSpatialReferenceProj4(int prjCode);
			CSpatialReferenceProj4(Handle hHandle);
			virtual ~CSpatialReferenceProj4();

			virtual bool  IsValid();
			virtual void*  GetHandle();
			virtual bool Project(ISpatialReference* destSpatRef, CommonLib::CGeoShape* pShape);
			virtual bool Project(ISpatialReference* destSpatRef, GisBoundingBox& bbox);
			virtual bool Project(ISpatialReference *destSpatRef, GisXYPoint* pPoint);

			virtual const CommonLib::str_t& GetProjectionString() const;
			virtual int   GetProjectionCode() const;
			virtual bool  IsProjection() const;

			virtual void save(CommonLib::IWriteStream *pStream) const;
			virtual void load(CommonLib::IReadStream *pStream);
		private:
			void CreateProjection();
			void PrepareGeometries();
			void PreTransform(CommonLib::CGeoShape *shp) const;
			bool IsEqual(CSpatialReferenceProj4* ref) const;
			void TestBounds(CommonLib::CGeoShape *geom) const;
			void PrepareGeometry(CommonLib::CGeoShape *pShp, double left_meridian, double bottom_parallel, double right_meridian, double top_parallel);
			void PrepareCutMeridian(double cut_meridian, double bottom_parallel, double top_parallel);
			void PrepareBoundShape(const GisBoundingBox& bbox);
			void DensifyBoundBox(CommonLib::CGeoShape *shp, const GisBoundingBox &bbox, int precision = 10) const;
		private:
			Handle m_hHandle;
			int m_prjCode;
			CommonLib::str_t m_prj4Str;

			CommonLib::CGeoShape m_LeftShp;
			CommonLib::CGeoShape m_RightShp; 
			CommonLib::CGeoShape m_BoundShape; 
			GisBoundingBox m_cutMeridian;
			GisBoundingBox m_fullExtent;
		};
	}
}
#endif