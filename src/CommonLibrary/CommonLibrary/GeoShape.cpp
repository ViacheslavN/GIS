#include "stdafx.h"
#include "GeoShape.h"

namespace CommonLib
{
	CGeoShape::CGeoShape(alloc_t *pAlloc) :
	m_pAlloc(pAlloc),
	m_vecParts(0, pAlloc),
	m_vecPoints(0,pAlloc),
	m_vecZ(0, pAlloc),
	m_vecM(0, pAlloc),
	m_type(shape_type_null),
	m_general_type (shape_type_null)
	{
		if(!m_pAlloc)
			m_pAlloc = &m_alloc;
	}
	CGeoShape::CGeoShape(eShapeType shapeType, alloc_t *pAlloc) :
		m_pAlloc(pAlloc),
		m_vecParts(0, pAlloc),
		m_vecPoints(0,pAlloc),
		m_vecZ(0, pAlloc),
		m_vecM(0, pAlloc),
		m_type(shapeType)
	{
		if(!m_pAlloc)
			m_pAlloc = &m_alloc;
		create(shapeType);
	}
	CGeoShape::CGeoShape(eShapeType shapeType, size_t npoints, size_t nparts, size_t ncurves, size_t mpatchSpecificSize, alloc_t *pAlloc) :
		m_pAlloc(pAlloc),
		m_vecParts(0, pAlloc),
		m_vecPoints(0,pAlloc),
		m_vecZ(0, pAlloc),
		m_vecM(0, pAlloc),
		m_type(shapeType)
	{
		if(!m_pAlloc)
			m_pAlloc = &m_alloc;
		create(shapeType, npoints, nparts, ncurves, mpatchSpecificSize);
	}
	 

	CGeoShape::~CGeoShape()
	{

	}

	void CGeoShape::create(eShapeType shapeType)
	{
		m_general_type = IGeoShape::generalType(shapeType);

	}
	void CGeoShape::create(eShapeType shapeType, size_t npoints, size_t nparts, size_t ncurves , size_t mpatchSpecificSize)
	{
		m_general_type = IGeoShape::generalType(shapeType);
		m_vecPoints.reserve(npoints);
		m_vecParts.reserve(nparts);

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
	GeoPt* CGeoShape::getPoints()
	{
		return m_vecPoints.begin();
	}
	const GeoPt* CGeoShape::getPoints() const
	{
		return m_vecPoints.begin();
	}

	 uint32 CGeoShape::getPointCnt() const
	 {
		 return m_vecPoints.size();
	 }

}