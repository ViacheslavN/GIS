#ifndef GIS_ENGINE_GEOMETRY_TOPOLOGICAL_OPERATOR_H_
#define GIS_ENGINE_GEOMETRY_TOPOLOGICAL_OPERATOR_H_
 
#include "Geometry.h"
#include "Common/Common.h"
#include "Common/Units.h"

namespace ClipperLib
{
	class Clipper;
}

namespace GisEngine
{
	namespace GisGeometry
	{
		class CTopologicalOperator : public ITopologicalOperator
		{
		public:
			CTopologicalOperator(CommonLib::alloc_t *pAlloc);
			~CTopologicalOperator(void);

			virtual CommonLib::IGeoShapePtr Intersect(CommonLib::IGeoShape* shape1, CommonLib::IGeoShape* shape2) const;
			virtual CommonLib::IGeoShapePtr AddShapeToIntersect(CommonLib::IGeoShape* shape) ;
			virtual CommonLib::IGeoShapePtr ClearIntersect() ;
			virtual CommonLib::IGeoShapePtr Intersect(CommonLib::IGeoShape* shape) const;
			virtual bool  IsIntersection(const CommonLib::CGeoShape* pShapeL, const CommonLib::CGeoShape* pShapeR, CommonLib::CGeoShape* pShapeRes); 

			virtual CommonLib::IGeoShapePtr  Union(CommonLib::IGeoShape* shape1, CommonLib::IGeoShape* shape2) const;
			virtual CommonLib::IGeoShapePtr  AddShapeToUnion(CommonLib::IGeoShape* shape) ;
			virtual CommonLib::IGeoShapePtr  Union(CommonLib::IGeoShape* shape) const;
			virtual CommonLib::IGeoShapePtr  ClearUnion() ;


			virtual CommonLib::IGeoShapePtr  GetDifference(CommonLib::IGeoShape* shape1, CommonLib::IGeoShape* shape2) const;
			virtual CommonLib::IGeoShapePtr  GetSymmetricDifference(CommonLib::IGeoShape* shape1, CommonLib::IGeoShape* shape2) const;
			virtual bool      Contains(CommonLib::IGeoShape* shape1, CommonLib::IGeoShape* shape2) const;

			virtual CommonLib::IGeoShapePtr  Clip(CommonLib::IGeoShape* shape, const GisBoundingBox& box) const;
			virtual CommonLib::IGeoShapePtr  CalcConvexHull(CommonLib::IGeoShape* shape) const;
			virtual CommonLib::IGeoShapePtr  CalcBuffer(CommonLib::IGeoShape* shape, double distance) const;

			virtual double    CalcDistance(CommonLib::IGeoShape* shape1, CommonLib::IGeoShape* shape2) const;
			virtual CommonLib::IGeoShapePtr  QuerySubCurve (CommonLib::IGeoShape* shape, int geomidx, double dist_from, double dist_to) const;
			virtual GisXYPoint QuerySubPoint (CommonLib::IGeoShape* shape, int geomidx, double dist) const = 0;
			virtual double    QuerySubAngle (CommonLib::IGeoShape* shape, int geomidx, double dist) const = 0;
			virtual void QuerySubPointAndAngle(CommonLib::IGeoShape* shape, int geomidx, double dist,GisXYPoint *pPoint,double *pAngle, bool firstTime) const;
			virtual double    CalcDistanceToPoint(CommonLib::IGeoShape* shape, const GisXYPoint& pnt, GisXYPoint* pnt_out) const;
			virtual double    CalcDistanceToPoint(const GisXYPoint* pnt_in, size_t npnt_in, const GisXYPoint& pnt, GisXYPoint& pnt_out, size_t &outidx) const;
			virtual double    CalcLineLength(CommonLib::IGeoShape* shape) const;
			virtual double    CalcPolygonArea(CommonLib::IGeoShape* shape) const;
			virtual double    CalcPolygonPerimeter(CommonLib::IGeoShape* shape) const;

			virtual bool      IsPointInside(CommonLib::IGeoShape* shape, const GisXYPoint& pnt) const;
			virtual bool      IsPointOutside(CommonLib::IGeoShape* shape, const GisXYPoint& pnt) const;
			virtual bool      IsPointOnBorder(CommonLib::IGeoShape* shape, const GisXYPoint& pnt) const;
			virtual bool      IsIntersectionNotNull(CommonLib::IGeoShape* shape1, CommonLib::IGeoShape* shape2) const;
			virtual bool      IsClippingNotNull(CommonLib::IGeoShape* shape, const GisBoundingBox& box) const;

			virtual bool      IsSimple(CommonLib::IGeoShape* shape) const;
			virtual void      Simplify(CommonLib::IGeoShape* shape) const;

			virtual bool CreateBufferZone(const CommonLib::CGeoShape* shape, CommonLib::CGeoShape* pShapeRes, double dDelta, eTOJoinType type) const;



		private:
			std::auto_ptr<Clipper> m_pClipper;			
		}
	}
}
#endif