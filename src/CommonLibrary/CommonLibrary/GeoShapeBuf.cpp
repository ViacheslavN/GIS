#include "stdafx.h"
#include "GeoShapeBuf.h"

namespace CommonLib
{

	bool isTypeSimple(eShapeType shapeType);
	eShapeType GetGeneralType(eShapeType _general_type)
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

	uint32 CGeoShapeBuf::calcSize(eShapeType shapeType, uint32 npoints, uint32 nparts, uint32 ncurves, uint32 mpatchSpecificSize)
	{
		 uint32 nSize = 0;
		 uint32 size = 0;
		 eShapeType genType;
		 bool has_z;
		 bool has_m;
		 bool has_curve;
		 bool has_id;
		 getTypeParams(shapeType, &genType, &has_z, &has_m, &has_curve, &has_id);
		 uint32 flag_z = has_z ? 1 : 0;
		 uint32 flag_m = has_m ? 1 : 0;
		 uint32 flag_curve = has_curve ? 1 : 0;
		 uint32 flag_id = has_id ? 1 : 0;

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
				 if(!isTypeSimple(shapeType))
					 size += 4 + 4; 
				 size += mpatchSpecificSize;
				 break;
			 }
		 }

		 return size;
	}
	void CGeoShapeBuf::initShapeBufferBuffer(unsigned char* buf, eShapeType shapeType, uint32 npoints, uint32 nparts, uint32 ncurves)
	{
		eShapeType genType;
		bool has_z;
		bool has_m;
		bool has_curve;
		bool has_id;
		double* dbuf;

		getTypeParams(shapeType, &genType, &has_z, &has_m, &has_curve, &has_id);

		// Type
		*reinterpret_cast<long*>(buf) = shapeType;
		buf += 4;
		if(shapeType == shape_type_null || genType == shape_type_null)
			return;

		if(genType == shape_type_general_point)
		{
			*reinterpret_cast<double*>(buf) = 0;
			return;
		}

		// Bounding box
		dbuf = reinterpret_cast<double*>(buf);
		*dbuf++ = 0.;
		*dbuf++ = 0.;
		*dbuf++ = 0.;
		*dbuf++ = 0.;
		buf += 8 * 4;

		// part count
		if(genType != shape_type_general_multipoint)
		{
			*reinterpret_cast<long*>(buf) = static_cast<long>(nparts);
			buf += 4;
		}

		// point count
		*reinterpret_cast<long*>(buf) = static_cast<long>(npoints);
		buf += 4;

		// parts starts
		if(genType != shape_type_general_multipoint)
			buf += 4 * nparts;

		// parts types
		if(genType == shape_type_general_multipatch)
			buf += 4 * nparts;

		// x,y of points
		buf += 8 * 2 * npoints;

		if(has_z)
		{
			// range of z
			dbuf = reinterpret_cast<double*>(buf);
			*dbuf++ = 0.;
			*dbuf++ = 0.;
			buf += 8 * 2;

			// z of points
			buf += 8 * npoints;
		}
		if((genType == shape_type_general_multipatch) && (!isTypeSimple(shapeType))) 
		{
			if(has_m)
				*reinterpret_cast<long*>(buf) = static_cast<long>(npoints);
			else
				*reinterpret_cast<long*>(buf) = 0;
			buf += 4;
		}

		if(has_m)
		{
			dbuf = reinterpret_cast<double*>(buf);
			*dbuf++ = 0.;
			*dbuf++ =  0.;
			buf += 8 * 2;

	
			buf += 8 * npoints;
		}

		if(has_curve)
		{
			*reinterpret_cast<long*>(buf) = static_cast<long>(ncurves);
			buf += 4;
		}
		if((genType == shape_type_general_multipatch) && (!isTypeSimple(shapeType))) //numIds
		{
			if(has_id)
				*reinterpret_cast<long*>(buf) = static_cast<long>(npoints);
			else
				*reinterpret_cast<long*>(buf) = 0;
			buf += 4;
		}
	}

	CGeoShapeBuf::CGeoShapeBuf(alloc_t *pAlloc) : m_blob(pAlloc)
	{
	}
	CGeoShapeBuf::CGeoShapeBuf(const CGeoShapeBuf& geoShp) : m_blob(geoShp.m_blob)
	{}

	CGeoShapeBuf& CGeoShapeBuf::operator=(const CGeoShapeBuf& shp)
	{
		if(this == &shp)
			return *this;

		if(m_bAttach)
		{
			m_pBuffer = shp.m_pBuffer;
			m_nBufSize = shp.m_nBufSize;
			m_params.set(m_pBuffer);
			return *this;
		}

		if(m_pBuffer)
			m_pAlloc->free(m_pBuffer);
		m_pBuffer = NULL;
		m_nBufSize = shp.m_nBufSize;
		m_params.reset();

		if(m_nBufSize)
			m_pBuffer = (byte*)m_pAlloc->alloc(m_nBufSize);

		memcpy(m_pBuffer, shp.m_pBuffer, m_nBufSize);
		m_params.set(m_pBuffer);

		return *this;

	 
		return *this;
	}

	CGeoShapeBuf::~CGeoShapeBuf()
	{
		if(!m_bAttach && m_pBuffer)
		{
			m_pAlloc->free(m_pBuffer);
			m_pBuffer = NULL;
		}
		
	}


	eShapeType CGeoShapeBuf::type() const
	{
		if(m_params.m_bIsValid)
			return m_params.m_type;

		if (m_pBuffer == NULL)
			return shape_type_null;
		return type(m_pBuffer);
	}

	eShapeType CGeoShapeBuf::type(const unsigned char* buf)
	{
		if(buf == 0)
			return shape_type_null;
		return *reinterpret_cast<const eShapeType*>(buf);
	}
	eShapeType CGeoShapeBuf::generalType(const unsigned char* buf)
	{
		eShapeType shapeType = type(buf);
		return GetGeneralType(shapeType);
	}
	uint32  CGeoShapeBuf::size() const
	{
		return m_nBufSize;
	}

	uint32  CGeoShapeBuf::getPartCount() const
	{
		if(m_params.m_bIsValid)
			return m_params.m_nPartCount;
		return partCount(m_pBuffer);
	}
	uint32  CGeoShapeBuf::getPartSize(uint32 idx) const
	{
		uint32 nparts = getPartCount();

		if(nparts == 0 || idx >= nparts)
			return 0;

		 const uint32* partStarts = getParts();

		if(idx == nparts - 1)
			return pointCount() - (uint32)partStarts[idx];
		else
			return (uint32)partStarts[idx + 1] - (uint32)partStarts[idx];
	}
	const uint32*  CGeoShapeBuf::getParts() const 
	{
		eShapeType genType = generalType();

		switch(genType)
		{
		case shape_type_general_multipatch:
		case shape_type_general_polyline:
		case shape_type_general_polygon:
			{
				const unsigned char* buf = m_pBuffer;
				buf += 4 + 8 * 4 + 4 + 4;
				return reinterpret_cast<const uint32*>(buf);
			}
		}

		return NULL;
	}
	uint32*  CGeoShapeBuf::getParts()
	{
		eShapeType genType = generalType();

		switch(genType)
		{
		case shape_type_general_multipatch:
		case shape_type_general_polyline:
		case shape_type_general_polygon:
			{
				unsigned char* buf = m_pBuffer;
				buf += 4 + 8 * 4 + 4 + 4;
				return reinterpret_cast<uint32*>(buf);
			}
		}

		return NULL;
	}

	GisXYPoint* CGeoShapeBuf::getPoints()
	{
		if(m_params.m_bIsValid)
			return const_cast<GisXYPoint*>(m_params.m_pPoints);
		return const_cast<GisXYPoint*>(getXYs(m_pBuffer));
	}
	const GisXYPoint* CGeoShapeBuf::getPoints() const
	{
		if(m_params.m_bIsValid)
			return m_params.m_pPoints;
		return getXYs(m_pBuffer);
	}
	uint32 CGeoShapeBuf::getPointCnt() const
	{
		if(m_params.m_bIsValid)
			return m_params.m_nPointCount;
		return pointCount(m_pBuffer);
	}

	void CGeoShapeBuf::create(uint32 nSize)
	{
		if(!m_bAttach && m_pBuffer)
			m_pAlloc->free(m_pBuffer);

		m_bAttach = false;
		m_nBufSize = nSize;
		if(m_nBufSize)
			m_pBuffer = (byte*)m_pAlloc->alloc(m_nBufSize);
		m_params.reset();
	}
	bool CGeoShapeBuf::create(eShapeType shapeType)
	{
		if(m_bAttach && m_pBuffer)
		{
			create(m_pBuffer, m_nBufSize, shapeType, 0);
		}
		else
		{

			create(shapeType, 0);
		}
		return true;
		
	}
	void CGeoShapeBuf::create(unsigned char* pBuf, uint32 nSize, eShapeType shapeType, uint32 npoints, uint32 nparts, uint32 ncurves)
	{
		initShapeBufferBuffer(pBuf, shapeType, npoints, nparts, ncurves);
		m_params.set(pBuf);
	}

	bool CGeoShapeBuf::create(eShapeType shapeType, uint32 npoints, uint32 nparts, uint32 ncurves, uint32 mpatchSpecificSize)
	{
		if(m_pBuffer)
			m_pAlloc->free(m_pBuffer);

		m_bAttach = false;
		m_nBufSize = calcSize(shapeType, npoints, nparts, ncurves,mpatchSpecificSize);
		if(m_nBufSize)
		{
			m_pBuffer = (byte*)m_pAlloc->alloc(m_nBufSize);
			if(m_pBuffer)
				initShapeBufferBuffer(m_pBuffer, shapeType, npoints, nparts, ncurves);
		}
		m_params.set(m_pBuffer);
		return true;
	}

	void CGeoShapeBuf::import(const unsigned char* extBuf, uint32 extBufSize)
	{
		if(m_bAttach)
			detach();
		if(!m_bAttach && m_pBuffer)
			m_pAlloc->free(m_pBuffer);

		m_pBuffer = (byte*)m_pAlloc->alloc(extBufSize);
		m_nBufSize = extBufSize;
		 memcpy(m_pBuffer, extBuf, extBufSize);
		m_params.set(m_pBuffer);
	}

	void CGeoShapeBuf::attach(unsigned char* extBuf, uint32 extBufSize)
	{
		if(m_bAttach)
			detach();
		if(!m_bAttach && m_pBuffer)
			m_pAlloc->free(m_pBuffer);
		m_pBuffer = extBuf;
		m_nBufSize = extBufSize;
		m_params.set(m_pBuffer);
	}

	unsigned char* CGeoShapeBuf::detach()
	{
		if(!m_bAttach)
			return NULL;
		m_bAttach = false;
		unsigned char* buf = m_pBuffer;
		m_pBuffer = NULL;
		m_params.reset();
		return buf;
	}

	uint32 CGeoShapeBuf::pointCount() const
	{
		if(m_params.m_bIsValid)
			return m_params.m_nPointCount;
		return pointCount(m_pBuffer);
	}

	uint32 CGeoShapeBuf::pointCount(const unsigned char* buf)
	{
		eShapeType genType = generalType(buf);
		return pointCount(buf, genType);
	}

	uint32 CGeoShapeBuf::pointCount(const unsigned char* buf, eShapeType general_type)
	{
		eShapeType genType = general_type;
		long flag_has_parts = 1;

		switch(genType)
		{
		case shape_type_general_point:
			return 1;
		case shape_type_general_multipoint:
			flag_has_parts = 0;
		case shape_type_general_polyline:
		case shape_type_general_polygon:
		case shape_type_general_multipatch:
			{
				return *reinterpret_cast<const uint32*>(buf + 4 + 8 * 4 + 4 * flag_has_parts);
			}
		}

		return 0;
	}

	const GisXYPoint* CGeoShapeBuf::getXYs(const unsigned char* buf)
	{
		eShapeType genType = generalType(buf);
		uint32 nparts = partCount(buf);
		return getXYs(buf, genType, nparts);
	}

	const GisXYPoint* CGeoShapeBuf::getXYs(const unsigned char* buf, eShapeType general_type, uint32 partCount)
	{
		eShapeType genType = general_type;
		uint32 nparts = partCount;

		switch(genType)
		{
		case shape_type_general_point:
			buf += 4;
			break;
		case shape_type_general_multipoint:
			buf += 4 + 8 * 4 + 4;
			break;
		case shape_type_general_polyline:
		case shape_type_general_polygon:
			buf += 4 + 8 * 4 + 4 + 4 + 4 * nparts;
			break;
		case shape_type_general_multipatch:
			buf += 4 + 8 * 4 + 4 + 4 + (4 + 4) * nparts;
			break;
		default:
			return NULL;
		}

		return reinterpret_cast<const GisXYPoint*>(buf);
	}
	double* CGeoShapeBuf::getZs()
	{
		return NULL;
	}
	const double* CGeoShapeBuf::getZs() const
	{
		return NULL;
	}

	double* CGeoShapeBuf::getMs()
	{
		return NULL;
	}
	const double* CGeoShapeBuf::getMs() const
	{
		return NULL;
	}

	////////////////////////////////////////////////////////

	CGeoShapeBuf::sShapeParams::sShapeParams()
		: m_bIsValid(false), m_type(shape_type_null), m_general_type(shape_type_null), m_nPointCount(0), m_nPartCount(0),  m_pPoints(0)
	{}

	void CGeoShapeBuf::sShapeParams::reset()
	{
		m_bIsValid = false;
		m_type = shape_type_null;
		m_general_type = shape_type_null;
		m_nPointCount = 0;
		m_nPartCount = 0;
		m_pPoints = NULL;
	}

	void CGeoShapeBuf::sShapeParams::set(const unsigned char* buf)
	{
		m_bIsValid = true;
 		// shape type
		m_type = CGeoShapeBuf::type(buf);
		m_general_type = GetGeneralType(m_type);
		m_nPointCount = CGeoShapeBuf::pointCount(buf, m_general_type);
		m_nPartCount = CGeoShapeBuf::partCount(buf, m_general_type);
		m_pPoints = CGeoShapeBuf::getXYs(buf, m_general_type, m_nPartCount);
	}

 
}