#include "stdafx.h"
#include "GeoShape.h"
#include "stream.h"
namespace CommonLib
{

	bool isTypeSimple(eShapeType shapeType)
	{
		return shapeType<= shape_type_multipatch  && shapeType >= shape_type_null;
	}

	eShapeType generalType(eShapeType _general_type)
	{
		eShapeType shapeType = _general_type;

		if(isTypeSimple(shapeType))
		{
			switch(shapeType)
			{
			case shape_type_null:
				return shape_type_null;

			case shape_type_point:
			case shape_type_point_z:
			case shape_type_point_m:
			case shape_type_point_zm:
				return shape_type_general_point;

			case shape_type_multipoint:
			case shape_type_multipoint_z:
			case shape_type_multipoint_m:
			case shape_type_multipoint_zm:
				return shape_type_general_multipoint;

			case shape_type_polyline:
			case shape_type_polyline_z:
			case shape_type_polyline_m:
			case shape_type_polyline_zm:
				return shape_type_general_polyline;

			case shape_type_polygon:
			case shape_type_polygon_z:
			case shape_type_polygon_m:
			case shape_type_polygon_zm:
				return shape_type_general_polygon;

			case shape_type_multipatch:
			case shape_type_multipatch_m:
				return shape_type_general_multipatch;
			}
		}

		return (eShapeType)(shapeType & shape_basic_type_mask);
	}
	void CGeoShape::getTypeParams(eShapeType shapeType, eShapeType* pGenType, bool* has_z, bool* has_m, bool* has_curve, bool* has_id)
	{
		if(isTypeSimple(shapeType))
		{
			if(has_z != NULL)
				*has_z = false;
			if(has_m != NULL)
				*has_m = false;
			if(has_curve != NULL)
				*has_curve = false;
			if(has_id != NULL)
				*has_id = false;

			eShapeType gType;
			switch(shapeType)
			{
			case shape_type_point:
			case shape_type_point_m:
			case shape_type_point_zm:
			case shape_type_point_z:
				gType = shape_type_general_point;
				break;
			case shape_type_multipoint:
			case shape_type_multipoint_m:
			case shape_type_multipoint_zm:
			case shape_type_multipoint_z:
				gType = shape_type_general_multipoint;
				break;
			case shape_type_polyline:
			case shape_type_polyline_m:
			case shape_type_polyline_zm:
			case shape_type_polyline_z:
				gType = shape_type_general_polyline;
				break;
			case shape_type_polygon:
			case shape_type_polygon_m:
			case shape_type_polygon_zm:
			case shape_type_polygon_z:
				gType = shape_type_general_polygon;
				break;
			case shape_type_multipatch_m:
			case shape_type_multipatch:
				gType = shape_type_general_multipatch;
				break;
			default:
				gType = shape_type_null;
			}

			if(gType != shape_type_null)
			{
				if(has_m != NULL)
				{
					if(shapeType == shape_type_point_m ||
						shapeType == shape_type_multipoint_m ||
						shapeType == shape_type_polyline_m ||
						shapeType == shape_type_polygon_m ||
						shapeType == shape_type_multipatch_m ||
						shapeType == shape_type_point_zm ||
						shapeType == shape_type_multipoint_zm ||
						shapeType == shape_type_polyline_zm ||
						shapeType == shape_type_polygon_zm)
						*has_m = true;
				}

				if(has_z != NULL)
				{
					if(shapeType == shape_type_point_z ||
						shapeType == shape_type_multipoint_z ||
						shapeType == shape_type_polyline_z ||
						shapeType == shape_type_polygon_z ||
						shapeType == shape_type_multipatch ||
						shapeType == shape_type_multipatch_m ||
						shapeType == shape_type_point_zm ||
						shapeType == shape_type_multipoint_zm ||
						shapeType == shape_type_polyline_zm ||
						shapeType == shape_type_polygon_zm)
						*has_z = true;
				}
			}

			if(pGenType != NULL)
				*pGenType = gType;
		}
		else 
		{
			if(pGenType != NULL)
				*pGenType = (eShapeType)(shapeType & shape_basic_type_mask);
			if(has_z != NULL)
			{
				if((eShapeType)(shapeType & shape_basic_type_mask) == shape_type_general_multipatch)
					*has_z = true;
				else
					*has_z = (shapeType & shape_has_zs) != 0;
			}
			if(has_m != NULL)
				*has_m = (shapeType & shape_has_ms) != 0;
			if(has_curve != NULL)
			{
				if((shapeType & shape_non_basic_modifier_mask) == 0 && 
					(shapeType == shape_type_general_polyline || shapeType == shape_type_general_polygon))
					*has_curve = true;
				else
					*has_curve = (shapeType & shape_has_curves) != 0;
			}
			if(has_id != NULL)
				*has_id = (shapeType & shape_has_ids) != 0;
		}

	}

	CGeoShape::CGeoShape(alloc_t *pAlloc) :
	m_pAlloc(pAlloc),
	m_vecParts(0, pAlloc),
	m_vecPoints(0,pAlloc),
	m_vecZs(0, pAlloc),
	m_vecMs(0, pAlloc),
	m_vecPartTypes(0, pAlloc),
	m_type(shape_type_null),
	m_general_type (shape_type_null)

	{
		if(!m_pAlloc)
			m_pAlloc = &m_alloc;

		m_bbox.type = bbox_type_normal;
	}
	/*CGeoShape::CGeoShape(eShapeType shapeType, alloc_t *pAlloc) :
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
	}*/
	 

	CGeoShape::~CGeoShape()
	{

	}
	CGeoShape::CGeoShape(const CGeoShape& shp)
	{
		if(shp.m_pAlloc != &shp.m_alloc)
			m_pAlloc = shp.m_pAlloc;

		m_vecParts = shp.m_vecParts;
		m_vecPoints = shp.m_vecPoints;
		m_vecZs =  shp.m_vecZs;
		m_vecMs =  shp.m_vecMs;
		m_type =		shp.m_type;
		m_vecPartTypes = shp.m_vecPartTypes;
		m_general_type = shp.m_general_type;

	}
	CGeoShape& CGeoShape::operator = (const CGeoShape& shp)
	{
		if(shp.m_pAlloc != &shp.m_alloc && m_pAlloc != shp.m_pAlloc)
		{
			m_pAlloc = shp.m_pAlloc;
		}

		m_vecParts = shp.m_vecParts;
		m_vecPoints = shp.m_vecPoints;
		m_vecZs =  shp.m_vecZs;
		m_vecMs =  shp.m_vecMs;
		m_type =		shp.m_type;
		m_vecPartTypes = shp.m_vecPartTypes;
		m_general_type = shp.m_general_type;
		return *this;
	}
	CGeoShape& CGeoShape::operator += (const CGeoShape& shp)
	{
		if(	m_type != shp.m_type)
		{
			assert(false);
			return *this;
		}
		if(shp.getPointCnt() == 0)
			return *this;

		if(!m_vecParts.empty())
			m_vecParts.push_back(getPointCnt());
		if(!shp.m_vecParts.empty())
			m_vecParts.push_back(shp.getPointCnt());
		if(!shp.m_vecZs.empty())
			m_vecZs.push_back(shp.m_vecZs);
		if(!shp.m_vecMs.empty())
			m_vecZs.push_back(shp.m_vecMs);
		if(!shp.m_vecPartTypes.empty())
			m_vecPartTypes.push_back(shp.m_vecPartTypes);


		return *this;
	}
	bool CGeoShape::create(eShapeType shapeType)
	{
		m_type = shapeType;
		m_general_type = generalType(shapeType);
		
		return true;

	}
	bool CGeoShape::create(eShapeType shapeType, size_t npoints, size_t nparts, size_t ncurves , size_t mpatchSpecificSize)
	{

		eShapeType genType;
		bool has_z;
		bool has_m;
		bool has_curve;
		bool has_id;
		getTypeParams(shapeType, &genType, &has_z, &has_m, &has_curve, &has_id);
		m_type = shapeType;
		m_general_type = generalType(shapeType);


		m_vecPoints.resize(npoints);
		if(nparts)
		{
			m_vecParts.resize(nparts);
			m_vecPartTypes.resize(mpatchSpecificSize);
		}

		if(has_z)
			m_vecZs.resize(npoints);
		if(has_m)
			m_vecMs.resize(npoints);

		return true;
	}
	 



	eShapeType CGeoShape::type() const
	{
		return m_type;
	}
	eShapeType CGeoShape::GetGeneralType() const
	{
		return m_general_type;
	}
	uint32  CGeoShape::getPartCount() const
	{
		return m_vecParts.size();
	}
	uint32  CGeoShape::getPart(uint32 idx) const
	{
		size_t nparts = getPartCount();
		assert(idx < nparts);
 
		
		const uint32* partStarts = getParts();
		if(idx == nparts - 1)
			return getPointCnt() - (size_t)partStarts[idx];
		else
			return (size_t)partStarts[idx + 1] - (size_t)partStarts[idx];

	}
/*	uint32&  CGeoShape::getPart(uint32 idx)
	{
		assert(idx < m_vecParts.size());
		return m_vecParts[idx];

	}*/
	const uint32*  CGeoShape::getParts() const
	{
		return m_vecParts.begin();
	}
	uint32*  CGeoShape::getParts()
	{
		return m_vecParts.begin();
	}
	patch_type* CGeoShape::getPartsTypes()
	{
		return m_vecPartTypes.begin();
	}
	const patch_type* CGeoShape::getPartsTypes() const
	{
		return m_vecPartTypes.begin();
	}
	patch_type& CGeoShape::partType(size_t idx)
	{
		return m_vecPartTypes[idx];
	}
	const patch_type& CGeoShape::partType(size_t idx) const
	{
		return m_vecPartTypes[idx];
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
	 double&  CGeoShape::ptX(size_t idx)
	 {
		return m_vecPoints[idx].x;
	 }
	 const double& CGeoShape::ptX(size_t idx) const
	 {
		 return m_vecPoints[idx].x;
	 }
	 double&  CGeoShape::ptY(size_t idx)
	 {
		 return m_vecPoints[idx].y;
	 }
	 const double& CGeoShape::ptY(size_t idx) const
	 {
		return m_vecPoints[idx].y;
	 }

	 double* CGeoShape::getZs()
	 {
		 return m_vecZs.begin();
	 }
	 const double* CGeoShape::getZs() const
	 {
		  return m_vecZs.begin();
	 }
	 double& CGeoShape::ptZ(size_t idx)
	 {
		 return m_vecZs[idx];
	 }
	 const double& CGeoShape::ptZ(size_t idx) const
	 {
		 return m_vecZs[idx];
	 }
	 double* CGeoShape::getMs()
	 {
		  return m_vecMs.begin();
	 }
	 const double* CGeoShape::getMs() const
	 {
		  return m_vecMs.begin();
	 }
	 double& CGeoShape::ptM(size_t idx)
	 {
		 return m_vecMs[idx];
	 }
	 const double& CGeoShape::ptM(size_t idx) const
	 {
		  return m_vecMs[idx];
	 }
	 void CGeoShape::calcBB()
	 {
		 eShapeType genType;
		 bool has_z;
		 bool has_m;
		 bool has_curve;
		 bool has_id;

		 getTypeParams(type(), &genType, &has_z, &has_m, &has_curve, &has_id);
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

	 void CGeoShape::setPoints(const double *pPoint)
	 {
		 for(size_t i = 0, sz = m_vecPoints.size(); i < sz; ++i)
		 {
			 m_vecPoints[i].x = pPoint[i * 2 + 0];
			 m_vecPoints[i].y = pPoint[i * 2 + 0];
		 }
	 }

	 void CGeoShape::setZs(const double *pZs)
	 {
		 for (size_t i = 0, sz = m_vecZs.size(); i < sz; ++i)
		 {
			 m_vecZs[i] = pZs[i];
		 }
	 }

	 //TO DO compress
	 bool CGeoShape::write(IWriteStream *pStream) const
	 {
		 pStream->write((byte)m_type);
		 pStream->write(m_vecParts.size());
		 for (size_t i = 0, sz = m_vecParts.size(); i < sz; ++i)
		 {
			 pStream->write(m_vecParts[i]);
		 }
		 pStream->write(m_vecPartTypes.size());
		 for (size_t i = 0, sz = m_vecPartTypes.size(); i < sz; ++i)
		 {
			 pStream->write((byte)m_vecPartTypes[i]);
		 }
		 pStream->write(m_vecPoints.size());
		 for (size_t i = 0, sz = m_vecPoints.size(); i < sz; ++i)
		 {
			 pStream->write(m_vecPoints[i].x);
			 pStream->write(m_vecPoints[i].y);
		 }
		 pStream->write(m_vecZs.size());
		 for (size_t i = 0, sz = m_vecZs.size(); i < sz; ++i)
		 {
			 pStream->write(m_vecZs[i]);
		 }
		 pStream->write(m_vecMs.size());
		 for (size_t i = 0, sz = m_vecZs.size(); i < sz; ++i)
		 {
			 pStream->write(m_vecMs[i]);
		 }
		 return true;
	 }
	 bool CGeoShape::read(IReadStream *pStream)
	 {
		 m_type = (eShapeType)pStream->readByte();
		 m_general_type = generalType(m_type);
		 uint32 nSize = pStream->readIntu32();
		 m_vecParts.reserve(nSize, true);
	 
		 for (size_t i = 0; i < nSize; ++i)
		 {
			 m_vecParts.push_back(pStream->readIntu32());
		 }

		 nSize = pStream->readIntu32();
		 m_vecPartTypes.reserve(nSize, true);
		 for (size_t i = 0; i < nSize; ++i)
		 {
			 m_vecPartTypes.push_back(pStream->readByte());
		 }

		 nSize = pStream->readIntu32();
		 m_vecPoints.reserve(nSize, true);
 
		 GisXYPoint pt;
		 for (size_t i = 0; i < nSize; ++i)
		 {
			 pt.x = pStream->readDouble();
			 pt.y = pStream->readDouble();
			 m_vecPoints.push_back(pt);
		 }

		nSize = pStream->readIntu32();
		m_vecZs.reserve(nSize, true);
		for (size_t i = 0; i < nSize; ++i)
		{
			m_vecZs.push_back(pStream->readDouble());
		}
		nSize = pStream->readIntu32();
		m_vecMs.reserve(nSize, true);
		for (size_t i = 0; i < nSize; ++i)
		{
			m_vecMs.push_back(pStream->readDouble());
		}
		return true;
	 }

	 void CGeoShape::clear()
	 {
		 m_type = shape_type_null;
		 m_general_type = shape_type_null;
		 m_vecParts.clear();
		 m_vecPartTypes.clear();
		 m_vecPoints.clear();
		 m_vecZs.clear();
		 m_vecMs.clear();
	 }
}