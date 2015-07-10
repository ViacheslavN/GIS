#ifndef GIS_ENGINE_GEOMETRY_TOPOLOGICAL_OPERATOR_H_
#define GIS_ENGINE_GEOMETRY_TOPOLOGICAL_OPERATOR_H_
 

#include "Common/Common.h"
#include "Common/Units.h"


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
		class CTopologicalOperator
		{
		public:
			CTopologicalOperator(CommonLib::alloc_t *pAlloc);
			~CTopologicalOperator(void);

			//polygons
			static bool   IsIntersection(const CommonLib::CGeoShape* shape, const GisBoundingBox& box); 
			bool   IsIntersection(const CommonLib::CGeoShape* pShapeL, const CommonLib::CGeoShape* pShapeR, 
				CommonLib::CGeoShape* pShapeRes = 0); 
			bool  IsInsidePolygon(const GisXYPoint& point, const CommonLib::CGeoShape *pPolygon);
			bool PolyBolean(const CommonLib::CGeoShape* pShapeL, const CommonLib::CGeoShape* pShapeR, 
				CommonLib::CGeoShape* pShapeRes, ePolygonOpType type);

			bool CreateBufferZone(const CommonLib::CGeoShape* shape, CommonLib::CGeoShape* pShapeRes, double dDelta, eTOJoinType type);


			static double distance(const GisXYPoint& point, CommonLib::CGeoShape* pShape, GisXYPoint& closestPoint);
			static double distance(const GisXYPoint& p1, const GisXYPoint& p2);
		}
	}
}
#endif