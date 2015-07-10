#include "stdafx.h"
#include "GeoShape.h"

namespace CommonLib
{

	bool CGeoShape::isTypeSimple(eShapeType shapeType)
	{
		 return shapeType<= shape_type_multipatch  && shapeType >= shape_type_null;
	}

	void CGeoShape::getTypeParams(eShapeType shapeType, eShapeType* pGenType, bool* has_z, bool* has_m, bool* has_curve, bool* has_id)
	{
		if(CGeoShape::isTypeSimple(shapeType))
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

	size_t CGeoShape::calcSize(eShapeType shapeType, size_t npoints, size_t nparts, size_t ncurves, size_t mpatchSpecificSize)
	{
		 size_t nSize = 0;
		 size_t size = 0;
		 eShapeType genType;
		 bool has_z;
		 bool has_m;
		 bool has_curve;
		 bool has_id;
		 CGeoShape::getTypeParams(shapeType, &genType, &has_z, &has_m, &has_curve, &has_id);
		 size_t flag_z = has_z ? 1 : 0;
		 size_t flag_m = has_m ? 1 : 0;
		 size_t flag_curve = has_curve ? 1 : 0;
		 size_t flag_id = has_id ? 1 : 0;

		 size += 4; // type
		 if(shapeType != shape_type_null)
		 {
			 switch(genType)
			 {
			 case shape_type_general_polyline:
			 case shape_type_general_polygon:
				 size += 8 * (2 * (2 + flag_z + flag_m)) + 4 + 4 + nparts * 4 + npoints * (8 * (2 + flag_z + flag_m) + 4 * flag_id) + (4 + ncurves * sizeof(segment_modifier_t)) * flag_curve;
				 break;
			 case shape_type_general_point:
				 size += 8 * (2 + flag_z + flag_m) + 4 * flag_id;
				 break;
			 case shape_type_general_multipoint:
				 size += 8 * (2 * (2 + flag_z + flag_m)) + 4 + npoints * (8 * (2 + flag_z + flag_m) + 4 * flag_id);
				 break;
			 case shape_type_general_multipatch:
				 size += 8 * (2 * (2 + 1 + flag_m)) + 4 + 4 + nparts * 4 + nparts * 4 + npoints * (8 * (2 + 1 + flag_m) + 4 * flag_id);
				 if(!CGeoShape::isTypeSimple(shapeType))
					 size += 4 + 4; 
				 size += mpatchSpecificSize;
				 break;
			 }
		 }

		 return size;
	}
	CGeoShape::CGeoShape(alloc_t *pAlloc) : m_pAlloc(pAlloc), m_pBuffer(0), m_nBufSize(0), m_bAttach(false)
	{
		if(!m_pAlloc)
			m_pAlloc = &m_alloc;
	}
	CGeoShape::CGeoShape(const CGeoShape& geoShp) : m_pBuffer(NULL)
	{
		m_pAlloc = geoShp.m_pAlloc;
		m_nBufSize = geoShp.m_nBufSize;
		m_bAttach = geoShp.m_bAttach;
		if(m_bAttach)
			m_pBuffer = geoShp.m_pBuffer;
		else if(geoShp.m_pBuffer && m_nBufSize != 0)
		{
			m_pBuffer = (byte*)m_pAlloc->alloc(m_nBufSize);
			memcpy(m_pBuffer, geoShp.m_pBuffer, m_nBufSize);
		}
	}
	CGeoShape::~CGeoShape()
	{
		if(!m_bAttach && m_pBuffer)
			m_pAlloc->free(m_pBuffer);
	}


	void CGeoShape::create(size_t nSize)
	{
		if(!m_bAttach && m_pBuffer)
			m_pAlloc->free(m_pBuffer);

		m_bAttach = false;
		m_nBufSize = nSize;
		if(m_nBufSize)
			m_pBuffer = (byte*)m_pAlloc->alloc(m_nBufSize);
	}
	void CGeoShape::create(eShapeType shapeType)
	{
		if(m_bAttach && m_pBuffer)
			create(m_pBuffer, m_nBufSize, shapeType, 0);
	}
	void CGeoShape::create(unsigned char* pBuf, size_t nSize, eShapeType shapeType, size_t npoints, size_t nparts, size_t ncurves)
	{

	}
}