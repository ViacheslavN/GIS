#include "stdafx.h"
#include "GeoShape.h"

namespace CommonLib
{
	CGeoShape::CGeoShape(alloc_t *pAlloc) :
	m_pAlloc(pAlloc),
	m_vecParts(0, pAlloc),
	m_vecPoints(0,pAlloc),
	m_vecZs(0, pAlloc),
	m_vecMs(0, pAlloc),
	m_type(shape_type_null),
	m_general_type (shape_type_null),
	m_bbox(bbox_type_normal)
	{
		if(!m_pAlloc)
			m_pAlloc = &m_alloc;
	}
	CGeoShape::CGeoShape(eShapeType shapeType, alloc_t *pAlloc) :
		m_pAlloc(pAlloc),
		m_vecParts(0, pAlloc),
		m_vecPoints(0,pAlloc),
		m_vecZs(0, pAlloc),
		m_vecMs(0, pAlloc),
		m_type(shapeType),
		m_bbox(bbox_type_normal)
	{
		if(!m_pAlloc)
			m_pAlloc = &m_alloc;
		create(shapeType);
	}
	CGeoShape::CGeoShape(eShapeType shapeType, size_t npoints, size_t nparts, size_t ncurves, size_t mpatchSpecificSize, alloc_t *pAlloc) :
		m_pAlloc(pAlloc),
		m_vecParts(0, pAlloc),
		m_vecPoints(0,pAlloc),
		m_vecZs(0, pAlloc),
		m_vecMs(0, pAlloc),
		m_type(shapeType),
		m_bbox(bbox_type_normal)
	{
		if(!m_pAlloc)
			m_pAlloc = &m_alloc;
		create(shapeType, npoints, nparts, ncurves, mpatchSpecificSize);
	}
	 

	CGeoShape::~CGeoShape()
	{

	}

	bool CGeoShape::create(eShapeType shapeType)
	{
		m_general_type = IGeoShape::generalType(shapeType);
		
		return true;

	}
	bool CGeoShape::create(eShapeType shapeType, size_t npoints, size_t nparts, size_t ncurves , size_t mpatchSpecificSize)
	{

		eShapeType genType;
		bool has_z;
		bool has_m;
		bool has_curve;
		bool has_id;
		IGeoShape::getTypeParams(shapeType, &genType, &has_z, &has_m, &has_curve, &has_id);

		m_general_type = IGeoShape::generalType(shapeType);


		m_vecPoints.reserve(npoints);
		m_vecParts.reserve(nparts);
		if(has_z)
			m_vecZs.reserve(npoints);
		if(has_m)
			m_vecMs.reserve(npoints);

		return true;
	}
	 



	eShapeType CGeoShape::type() const
	{
		return m_type;
	}
	uint32  CGeoShape::getPartCount() const
	{
		return m_vecParts.size();
	}
	uint32  CGeoShape::getPartSize(uint32 idx) const
	{
		assert(idx < m_vecParts.size());
		return m_vecParts[idx];

	}
	const uint32*  CGeoShape::getParts() const
	{
		return m_vecParts.begin();
	}
	uint32*  CGeoShape::getParts()
	{
		return m_vecParts.begin();
	}
	GisXYPoint* CGeoShape::getPoints()
	{
		return m_vecPoints.begin();
	}
	const GisXYPoint* CGeoShape::getPoints() const
	{
		return m_vecPoints.begin();
	}

	 uint32 CGeoShape::getPointCnt() const
	 {
		 return m_vecPoints.size();
	 }


	 double* CGeoShape::getZs()
	 {
		 return m_vecZs.begin();
	 }
	 const double* CGeoShape::getZs() const
	 {
		  return m_vecZs.begin();
	 }

	 double* CGeoShape::getMs()
	 {
		  return m_vecMs.begin();
	 }
	 const double* CGeoShape::getMs() const
	 {
		  return m_vecMs.begin();
	 }
	 void CGeoShape::calcBB()
	 {
		 eShapeType genType;
		 bool has_z;
		 bool has_m;
		 bool has_curve;
		 bool has_id;

		 IGeoShape::getTypeParams(type(), &genType, &has_z, &has_m, &has_curve, &has_id);
		 if(genType == shape_type_null || type() == shape_type_null || genType == shape_type_general_point)
			 return;

		 size_t npoints = getPointCnt();
		 if(npoints == 0)
			 return;
		
		 m_bbox.xMin = DBL_MAX;
		 m_bbox.yMin = DBL_MAX;
		 m_bbox.xMax = -DBL_MAX;
		 m_bbox.yMax = -DBL_MAX;
		 {
			 for(size_t i = 0, sz = m_vecPoints.size(); i < sz; ++i)
			 {
				 m_bbox.xMin = min(m_bbox.xMin, m_vecPoints[i].x);
				 m_bbox.yMin = min(m_bbox.yMin, m_vecPoints[i].y);
				 m_bbox.xMax = max(m_bbox.xMax, m_vecPoints[i].x);
				 m_bbox.yMax = max(m_bbox.yMax, m_vecPoints[i].y);
			 }
		 }
		

		 if(has_z)
		 {
			 m_bbox.zMin = DBL_MAX;
			 m_bbox.zMax = -DBL_MAX;
			 for (size_t i = 0, sz = m_vecZs.size(); i < sz; ++i)
			 {

				 m_bbox.zMin = min(m_bbox.zMin, m_vecZs[i]);
				 m_bbox.zMax = max(m_bbox.zMax, m_vecZs[i]);
			 }
		 }

		 if(has_m)
		 {
			 m_bbox.mMin = DBL_MAX;
			 m_bbox.mMax = -DBL_MAX;
			 for (size_t i = 0, sz = m_vecMs.size(); i < sz; ++i)
			 {

				 m_bbox.mMin = min(m_bbox.mMin, m_vecMs[i]);
				 m_bbox.mMax = max(m_bbox.mMax, m_vecMs[i]);
			 }
		 }
	 }

	 const bbox& CGeoShape::getBB() const
	 {
		 return m_bbox;
	 }
	 bbox& CGeoShape::getBB()
	 {
		 return m_bbox;
	 }
}