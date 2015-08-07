#ifndef GIS_ENGINE_GEOMETRY_I_GEOMETRY_H_
#define GIS_ENGINE_GEOMETRY_I_GEOMETRY_H_

#include "Common/Common.h"
#include "Common/Units.h"
#include "CommonLibrary/IRefCnt.h"

namespace GisEngine
{
	namespace Geometry
	{

		enum ePolygonOpType
		{
			PolyIntersection,
			PolyUnion, 
			PolyDifference,
			PolyXor
		};


		enum eTOJoinType 
		{
			ToJtSquare,
			ToJtRound, 
			ToJtMiter
		};

		struct ISpatialReference;
		struct ITopologicalOperator;
		struct IEnvelope;

		COMMON_LIB_REFPTR_TYPEDEF(ISpatialReference);
		COMMON_LIB_REFPTR_TYPEDEF(ITopologicalOperator);
		COMMON_LIB_REFPTR_TYPEDEF(IEnvelope);


		struct ISpatialReference : public CommonLib::AutoRefCounter
		{
		public:
			ISpatialReference(){};
			virtual ~ISpatialReference(){}

			virtual bool  IsValid() = 0;
			virtual void*  GetHandle() = 0;
			virtual bool Project(ISpatialReference* destSpatRef, CommonLib::CGeoShape* pShape) = 0;
			virtual bool Project(ISpatialReference* destSpatRef, GisBoundingBox& bbox) = 0;
			virtual bool Project(ISpatialReference *destSpatRef, GisXYPoint* pPoint) = 0;

			virtual const CommonLib::str_t& GetProjectionString() const = 0;
			virtual int   GetProjectionCode() const = 0;
			virtual bool  IsProjection() const = 0;

			virtual void save(CommonLib::IWriteStream *pStream) const = 0;
			virtual void load(CommonLib::IReadStream *pStream) = 0;
			virtual bool IsEqual(ISpatialReference* pSpatRef) const= 0;
		};



		struct  ITopologicalOperator : public CommonLib::AutoRefCounter
		{

			ITopologicalOperator(){}
			virtual ~ITopologicalOperator(){}
			virtual CommonLib::IGeoShapePtr Intersect(CommonLib::IGeoShape* shape1, CommonLib::IGeoShape* shape2) const = 0;
			virtual CommonLib::IGeoShapePtr AddShapeToIntersect(CommonLib::IGeoShape* shape)  = 0;
			virtual CommonLib::IGeoShapePtr ClearIntersect()  = 0;
			virtual CommonLib::IGeoShapePtr Intersect(CommonLib::IGeoShape* shape) const = 0;
			virtual bool  IsIntersection(const CommonLib::CGeoShape* pShapeL, const CommonLib::CGeoShape* pShapeR, CommonLib::CGeoShape* pShapeRes = 0) = 0; 

			virtual CommonLib::IGeoShapePtr  Union(CommonLib::IGeoShape* shape1, CommonLib::IGeoShape* shape2) const = 0;
			virtual CommonLib::IGeoShapePtr  AddShapeToUnion(CommonLib::IGeoShape* shape)  = 0;
			virtual CommonLib::IGeoShapePtr  Union(CommonLib::IGeoShape* shape) const = 0;
			virtual CommonLib::IGeoShapePtr  ClearUnion()  = 0;


			virtual CommonLib::IGeoShapePtr  GetDifference(CommonLib::IGeoShape* shape1, CommonLib::IGeoShape* shape2) const = 0;
			virtual CommonLib::IGeoShapePtr  GetSymmetricDifference(CommonLib::IGeoShape* shape1, CommonLib::IGeoShape* shape2) const = 0;
			virtual bool      Contains(CommonLib::IGeoShape* shape1, CommonLib::IGeoShape* shape2) const = 0;

			virtual CommonLib::IGeoShapePtr  Clip(CommonLib::IGeoShape* shape, const GisBoundingBox& box) const = 0;
			virtual CommonLib::IGeoShapePtr  CalcConvexHull(CommonLib::IGeoShape* shape) const = 0;
			virtual CommonLib::IGeoShapePtr  CalcBuffer(CommonLib::IGeoShape* shape, double distance) const = 0;

			virtual double    CalcDistance(CommonLib::IGeoShape* shape1, CommonLib::IGeoShape* shape2) const = 0;
			virtual CommonLib::IGeoShapePtr  QuerySubCurve (CommonLib::IGeoShape* shape, int geomidx, double dist_from, double dist_to) const = 0;
			virtual GisXYPoint QuerySubPoint (CommonLib::IGeoShape* shape, int geomidx, double dist) const = 0;
			virtual double    QuerySubAngle (CommonLib::IGeoShape* shape, int geomidx, double dist) const = 0;
			virtual void QuerySubPointAndAngle(CommonLib::IGeoShape* shape, int geomidx, double dist,GisXYPoint *pPoint,double *pAngle, bool firstTime) const = 0;
			virtual double    CalcDistanceToPoint(CommonLib::IGeoShape* shape, const GisXYPoint& pnt, GisXYPoint* pnt_out) const = 0;
			virtual double    CalcDistanceToPoint(const GisXYPoint* pnt_in, size_t npnt_in, const GisXYPoint& pnt, GisXYPoint& pnt_out, size_t &outidx) const = 0;
			virtual double    CalcLineLength(CommonLib::IGeoShape* shape) const = 0;
			virtual double    CalcPolygonArea(CommonLib::IGeoShape* shape) const = 0;
			virtual double    CalcPolygonPerimeter(CommonLib::IGeoShape* shape) const = 0;

			virtual bool      IsPointInside(CommonLib::IGeoShape* shape, const GisXYPoint& pnt) const = 0;
			virtual bool      IsPointOutside(CommonLib::IGeoShape* shape, const GisXYPoint& pnt) const = 0;
			virtual bool      IsPointOnBorder(CommonLib::IGeoShape* shape, const GisXYPoint& pnt) const = 0;
			virtual bool      IsIntersectionNotNull(CommonLib::IGeoShape* shape1, CommonLib::IGeoShape* shape2) const = 0;
			virtual bool      IsClippingNotNull(CommonLib::IGeoShape* shape, const GisBoundingBox& box) const = 0;

			virtual bool      IsSimple(CommonLib::IGeoShape* shape) const = 0;
			virtual void      Simplify(CommonLib::IGeoShape* shape) const = 0;

			virtual bool CreateBufferZone(const CommonLib::CGeoShape* shape, CommonLib::CGeoShape* pShapeRes, double dDelta, eTOJoinType type) const = 0;

		};

		struct  IEnvelope  : public CommonLib::AutoRefCounter
		{
			IEnvelope(){}
			virtual ~IEnvelope(){}

			virtual GisBoundingBox&				GetBoundingBox() = 0;
			virtual void                       SetBoundingBox(const GisBoundingBox& box) = 0;
			virtual ISpatialReferencePtr       GetSpatialReference() const = 0;
			virtual void                       SetSpatialReference(ISpatialReference* spatRef) = 0;
			virtual void                       Expand(IEnvelope* envelope) = 0;
			virtual bool                       Intersect(IEnvelope* envelope) = 0;
			virtual void                       Project(ISpatialReference* spatRef) = 0;
		};

 
	}
}

#endif