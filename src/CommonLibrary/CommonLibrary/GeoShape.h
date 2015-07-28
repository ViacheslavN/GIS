#ifndef _LIB_COMMON_GEO_SHAPE_H_
#define _LIB_COMMON_GEO_SHAPE_H_
#include "IGeoShape.h"
#include "PodVector.h"
namespace CommonLib
{
	class CGeoShape : public IGeoShape
	{

		typedef TPodVector<GisXYPoint> TVecGeoPt;
		typedef TPodVector<double> TVecZ;
		typedef TPodVector<double> TVecM;
		typedef TPodVector<uint32> TVecParts;
	public:


		CGeoShape(alloc_t *pAlloc = 0);
		CGeoShape(eShapeType shapeType, alloc_t *pAlloc = 0);
		CGeoShape(eShapeType shapeType, size_t npoints, size_t nparts = 1, size_t ncurves = 0, size_t mpatchSpecificSize = 0, alloc_t *pAlloc = 0);
	 

		~CGeoShape();

		void create(eShapeType shapeType);
		void create(eShapeType shapeType, size_t npoints, size_t nparts = 1, size_t ncurves = 0, size_t mpatchSpecificSize = 0);
 



		virtual eShapeType type() const;
	 	virtual uint32  getPartCount() const;
		virtual uint32  getPartSize(uint32 idx) const;
		virtual const uint32*  getParts() const;
		virtual uint32*  getParts();
		virtual GisXYPoint* getPoints();
		virtual const GisXYPoint* getPoints() const;

		virtual uint32 getPointCnt() const;

	private:
		simple_alloc_t m_alloc;
		alloc_t *m_pAlloc;

		TVecParts m_vecParts;
		TVecGeoPt m_vecPoints;
		TVecZ	  m_vecZ;
		TVecM	  m_vecM;

		eShapeType m_type;
		eShapeType m_general_type;

	};

}

#endif