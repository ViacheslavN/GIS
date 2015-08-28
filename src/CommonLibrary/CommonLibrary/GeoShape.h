#ifndef _LIB_COMMON_GEO_SHAPE_H_
#define _LIB_COMMON_GEO_SHAPE_H_
#include "IGeoShape.h"
#include "PodVector.h"
namespace CommonLib
{
	class CGeoShape  : public AutoRefCounter
	{

		typedef TPodVector<GisXYPoint> TVecGeoPt;
		typedef TPodVector<double> TVecZ;
		typedef TPodVector<double> TVecM;
		typedef TPodVector<uint32> TVecParts;
		typedef TPodVector<patch_type> TVecPartTypes;
	public:


		CGeoShape(alloc_t *pAlloc = 0);

		CGeoShape(const CGeoShape& shp);
		CGeoShape& operator = (const CGeoShape& shp);
		CGeoShape& operator += (const CGeoShape& shp);
		/*
		CGeoShape(eShapeType shapeType, alloc_t *pAlloc = 0);
		CGeoShape(eShapeType shapeType, size_t npoints, size_t nparts = 1, size_t ncurves = 0, size_t mpatchSpecificSize = 0, alloc_t *pAlloc = 0);
		 */

		~CGeoShape();
		
		bool create(eShapeType shapeType);
		bool create(eShapeType shapeType, size_t npoints, size_t nparts = 1, size_t ncurves = 0, size_t mpatchSpecificSize = 0);
		
		
		
		void calcBB();
		const bbox& getBB() const;
		bbox& getBB();
		
		eShapeType type() const;
		eShapeType GetGeneralType() const;
		uint32  getPartCount() const;
		const uint32&  getPart(uint32 idx) const;
		uint32&  getPart(uint32 idx);
		const uint32*  getParts() const;
		uint32*  getParts();
		
		patch_type*       getPartsTypes();
		const patch_type* getPartsTypes() const;
		patch_type&       partType(size_t idx);
		const patch_type& partType(size_t idx) const;
		
		
		GisXYPoint* getPoints();
		const GisXYPoint* getPoints() const;
		void setPoints(const double *pPoint);
		uint32 getPointCnt() const;
		
		double&       ptX(size_t idx); 
		const double& ptX(size_t idx) const; 
		
		double&       ptY(size_t idx); 
		const double& ptY(size_t idx) const; 
		
		
		double* getZs();
		const double* getZs() const;
		void setZs(const double *pZs);
		double&       ptZ(size_t idx);
		const double& ptZ(size_t idx) const;
		
		double* getMs();
		const double* getMs() const;
		double&       ptM(size_t idx);
		const double& ptM(size_t idx) const;

	private:
		simple_alloc_t m_alloc;
		alloc_t *m_pAlloc;

		TVecParts m_vecParts;
		TVecGeoPt m_vecPoints;
		TVecZ	  m_vecZs;
		TVecM	  m_vecMs;
		TVecPartTypes  m_vecPartTypes;

		eShapeType m_type;
		eShapeType m_general_type;
		bbox m_bbox;

	};

	typedef IRefCntPtr<CGeoShape> IGeoShapePtr;

}

#endif