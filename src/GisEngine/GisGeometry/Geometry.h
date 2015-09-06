#ifndef GIS_ENGINE_GEOMETRY_I_GEOMETRY_H_
#define GIS_ENGINE_GEOMETRY_I_GEOMETRY_H_

#include <set>
#include "Common/Common.h"
#include "Common/Units.h"
#include "CommonLibrary/IRefCnt.h"
#include "Common/GisEngineCommon.h"
#include "CommonLibrary/stream.h"

namespace GisEngine
{
	namespace GisGeometry
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


		struct ISpatialReference : public CommonLib::AutoRefCounter, public GisCommon::IStreamSerialize, public GisCommon::IXMLSerialize
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

			virtual bool IsEqual(ISpatialReference* pSpatRef) const= 0;
			virtual ISpatialReference* clone() const = 0;

			virtual GisCommon::Units GetUnits() const = 0;
		};



		struct  ITopologicalOperator : public CommonLib::AutoRefCounter
		{

			ITopologicalOperator(){}
			virtual ~ITopologicalOperator(){}
			virtual CommonLib::IGeoShapePtr Intersect(CommonLib::CGeoShape* shape1, CommonLib::CGeoShape* shape2) const = 0;
			virtual CommonLib::IGeoShapePtr AddShapeToIntersect(CommonLib::CGeoShape* shape)  = 0;
			virtual CommonLib::IGeoShapePtr ClearIntersect()  = 0;
			virtual CommonLib::IGeoShapePtr Intersect(CommonLib::CGeoShape* shape) const = 0;
			virtual bool  IsIntersection(const CommonLib::CGeoShape* pShapeL, const CommonLib::CGeoShape* pShapeR, CommonLib::CGeoShape* pShapeRes = 0) = 0; 

			virtual CommonLib::IGeoShapePtr  Union(CommonLib::CGeoShape* shape1, CommonLib::CGeoShape* shape2) const = 0;
			virtual CommonLib::IGeoShapePtr  AddShapeToUnion(CommonLib::CGeoShape* shape)  = 0;
			virtual CommonLib::IGeoShapePtr  Union(CommonLib::CGeoShape* shape) const = 0;
			virtual CommonLib::IGeoShapePtr  ClearUnion()  = 0;


			virtual CommonLib::IGeoShapePtr  GetDifference(CommonLib::CGeoShape* shape1, CommonLib::CGeoShape* shape2) const = 0;
			virtual CommonLib::IGeoShapePtr  GetSymmetricDifference(CommonLib::CGeoShape* shape1, CommonLib::CGeoShape* shape2) const = 0;
			virtual bool      Contains(CommonLib::CGeoShape* shape1, CommonLib::CGeoShape* shape2) const = 0;

			virtual CommonLib::IGeoShapePtr  Clip(CommonLib::CGeoShape* shape, const GisBoundingBox& box) const = 0;
			virtual CommonLib::IGeoShapePtr  CalcConvexHull(CommonLib::CGeoShape* shape) const = 0;
			virtual CommonLib::IGeoShapePtr  CalcBuffer(CommonLib::CGeoShape* shape, double distance) const = 0;

			virtual double    CalcDistance(CommonLib::CGeoShape* shape1, CommonLib::CGeoShape* shape2) const = 0;
			virtual CommonLib::IGeoShapePtr  QuerySubCurve (CommonLib::CGeoShape* shape, int geomidx, double dist_from, double dist_to) const = 0;
			virtual GisXYPoint QuerySubPoint (CommonLib::CGeoShape* shape, int geomidx, double dist) const = 0;
			virtual double    QuerySubAngle (CommonLib::CGeoShape* shape, int geomidx, double dist) const = 0;
			virtual void QuerySubPointAndAngle(CommonLib::CGeoShape* shape, int geomidx, double dist,GisXYPoint *pPoint,double *pAngle, bool firstTime) const = 0;
			virtual double    CalcDistanceToPoint(CommonLib::CGeoShape* shape, const GisXYPoint& pnt, GisXYPoint* pnt_out) const = 0;
			virtual double    CalcDistanceToPoint(const GisXYPoint* pnt_in, size_t npnt_in, const GisXYPoint& pnt, GisXYPoint& pnt_out, size_t &outidx) const = 0;
			virtual double    CalcLineLength(CommonLib::CGeoShape* shape) const = 0;
			virtual double    CalcPolygonArea(CommonLib::CGeoShape* shape) const = 0;
			virtual double    CalcPolygonPerimeter(CommonLib::CGeoShape* shape) const = 0;

			virtual bool      IsPointInside(CommonLib::CGeoShape* shape, const GisXYPoint& pnt) const = 0;
			virtual bool      IsPointOutside(CommonLib::CGeoShape* shape, const GisXYPoint& pnt) const = 0;
			virtual bool      IsPointOnBorder(CommonLib::CGeoShape* shape, const GisXYPoint& pnt) const = 0;
			virtual bool      IsIntersectionNotNull(CommonLib::CGeoShape* shape1, CommonLib::CGeoShape* shape2) const = 0;
			virtual bool      IsClippingNotNull(CommonLib::CGeoShape* shape, const GisBoundingBox& box) const = 0;

			virtual bool      IsSimple(CommonLib::CGeoShape* shape) const = 0;
			virtual void      Simplify(CommonLib::CGeoShape* shape) const = 0;

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

			virtual IEnvelope*				  clone() const = 0; 
		};

 
	}
}

#endif