#include "stdafx.h"
#include "GeoShapeBuf.h"
#include "FixedMemoryStream.h"

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


	void CGeoShapeBuf::getTypeParams(eShapeType shapeType, eShapeType* pGenType, bool* has_z, bool* has_m, bool* has_curve, bool* has_id)
	{
		if (isTypeSimple(shapeType))
		{
			if (has_z != NULL)
				*has_z = false;
			if (has_m != NULL)
				*has_m = false;
			if (has_curve != NULL)
				*has_curve = false;
			if (has_id != NULL)
				*has_id = false;

			eShapeType gType;
			switch (shapeType)
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

			if (gType != shape_type_null)
			{
				if (has_m != NULL)
				{
					if (shapeType == shape_type_point_m ||
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

				if (has_z != NULL)
				{
					if (shapeType == shape_type_point_z ||
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

			if (pGenType != NULL)
				*pGenType = gType;
		}
		else
		{
			if (pGenType != NULL)
				*pGenType = (eShapeType)(shapeType & shape_basic_type_mask);
			if (has_z != NULL)
			{
				if ((eShapeType)(shapeType & shape_basic_type_mask) == shape_type_general_multipatch)
					*has_z = true;
				else
					*has_z = (shapeType & shape_has_zs) != 0;
			}
			if (has_m != NULL)
				*has_m = (shapeType & shape_has_ms) != 0;
			if (has_curve != NULL)
			{
				if ((shapeType & shape_non_basic_modifier_mask) == 0 &&
					(shapeType == shape_type_general_polyline || shapeType == shape_type_general_polygon))
					*has_curve = true;
				else
					*has_curve = (shapeType & shape_has_curves) != 0;
			}
			if (has_id != NULL)
				*has_id = (shapeType & shape_has_ids) != 0;
		}

	}

	uint32 CGeoShapeBuf::calcSize(eShapeType shapeType, uint32 npoints, uint32 nparts, uint32 ncurves, uint32 mpatchSpecificSize)
	{
	 
		 uint32 size =1; //flag
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

		 size += 2; // type
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

		*buf = (byte)(eNoSuccinct);
		buf += 1;

		// Type
		*reinterpret_cast<short*>(buf) = shapeType;
		buf += 2;
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
		if (nparts > 1)
		{
			if (genType != shape_type_general_multipoint)
				buf += 4 * nparts;

			// parts types
			if (genType == shape_type_general_multipatch)
				buf += 4 * nparts;
		}
	
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
			buf += 8 * 2; // range of m

	
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

	CGeoShapeBuf::CGeoShapeBuf(alloc_t *pAlloc) : m_blob(pAlloc) , m_bInitSuccinct(false)
	{
	}
	CGeoShapeBuf::CGeoShapeBuf(const CGeoShapeBuf& geoShp) : m_blob(geoShp.m_blob), m_bInitSuccinct(false)
	{}

	CGeoShapeBuf& CGeoShapeBuf::operator=(const CGeoShapeBuf& shp)
	{
		if(this == &shp)
			return *this;

		m_blob = shp.m_blob;
		m_params.reset();
			 
		m_params.set(m_blob.buffer());
		m_bInitSuccinct = false;
	 
		return *this;
	}

	CGeoShapeBuf::~CGeoShapeBuf()
	{}


	eShapeType CGeoShapeBuf::type() const
	{
		if(m_params.m_bIsValid)
			return m_params.m_type;

		if (m_blob.empty())
			return shape_type_null;
		return type(m_blob.buffer());
	}

	eShapeType CGeoShapeBuf::type(const byte* buf)
	{
		if(buf == 0)
			return shape_type_null;
		return(eShapeType)(*reinterpret_cast<const short*>(buf + 1));
	}

	eShapeType CGeoShapeBuf::generalType() const
	{
		return generalType(m_blob.buffer());
	}

	eShapeType CGeoShapeBuf::generalType(const byte* buf)
	{
		eShapeType shapeType = type(buf);
		return GetGeneralType(shapeType);
	}
	uint32  CGeoShapeBuf::size() const
	{
		return m_blob.size();
	}

	uint32  CGeoShapeBuf::getPartCount() const
	{
		if (IsSuccinct())
			return m_Encoder.cntParts();

		if(m_params.m_bIsValid)
			return m_params.m_nPartCount;
		return partCount(m_blob.buffer());
	}


	uint32 CGeoShapeBuf::partCount(const unsigned char* buf)
	{
		eShapeType genType = generalType(buf);
		return partCount(buf, genType);
	}

	uint32 CGeoShapeBuf::partCount(const unsigned char* buf, eShapeType _general_type)
	{
		eShapeType genType = _general_type;

		switch (genType)
		{
			case shape_type_general_polyline:
			case shape_type_general_polygon:
			case shape_type_general_multipatch:
			{
				//const unsigned char* buf = cbuffer();
				//buf += 4 + 8 * 4;
				return *reinterpret_cast<const long*>(buf + 1 + 2 + 8 * 4); // flag type bbox
			}
		}

		return 0;
	}

	uint32  CGeoShapeBuf::getPart(uint32 idx) const
	{
		if (IsSuccinct())
			return 0;

		uint32 nparts = getPartCount();

		if(nparts == 0 || idx >= nparts)
			return 0;

		if (nparts == 1)
			return pointCount();

		 const uint32* partStarts = getParts();

		if(idx == nparts - 1)
			return pointCount() - (uint32)partStarts[idx];
		else
			return (uint32)partStarts[idx + 1] - (uint32)partStarts[idx];
	}
	const uint32*  CGeoShapeBuf::getParts() const 
	{
		if (IsSuccinct())
			return nullptr;

		eShapeType genType = generalType();

		switch(genType)
		{
		case shape_type_general_multipatch:
		case shape_type_general_polyline:
		case shape_type_general_polygon:
			{

				uint32 nparts = getPartCount();
				if(nparts < 2)
					return nullptr;

				const unsigned char* buf = m_blob.buffer();
				buf +=1 + 2 + 8 * 4 + 4; //flag type bbox part cnt
				return reinterpret_cast<const uint32*>(buf);
			}
		}

		return nullptr;
	}
	uint32*  CGeoShapeBuf::getParts()
	{
		if (IsSuccinct())
			return nullptr;

		eShapeType genType = generalType();

		switch(genType)
		{
		case shape_type_general_multipatch:
		case shape_type_general_polyline:
		case shape_type_general_polygon:
			{
				uint32 nparts = getPartCount();
				if (nparts < 2)
					return nullptr;

				unsigned char* buf = m_blob.buffer();
				buf += 1 + 2 + 8 * 4 + 4; //flag type bbox part cnt
				return reinterpret_cast<uint32*>(buf);
			}
		}

		return nullptr;
	}

	GisXYPoint* CGeoShapeBuf::getPoints()
	{
		if (IsSuccinct())
			return nullptr;

		if(m_params.m_bIsValid)
			return const_cast<GisXYPoint*>(m_params.m_pPoints);
		return const_cast<GisXYPoint*>(getXYs(m_blob.buffer()));
	}
	const GisXYPoint* CGeoShapeBuf::getPoints() const
	{
		if (IsSuccinct())
			return nullptr;

		if(m_params.m_bIsValid)
			return m_params.m_pPoints;
		return getXYs(m_blob.buffer());
	}
	uint32 CGeoShapeBuf::getPointCnt() const
	{
		if (IsSuccinct())
			return m_Encoder.cntPoints();

		if(m_params.m_bIsValid)
			return m_params.m_nPointCount;
		return pointCount(m_blob.buffer());
	}

	void CGeoShapeBuf::create(uint32 nSize)
	{
		m_blob.reserve(nSize);
		m_params.reset();
	}
	bool CGeoShapeBuf::create(eShapeType shapeType)
	{
		create(shapeType, 0);
		return true;
		
	}
	void CGeoShapeBuf::create(unsigned char* pBuf, uint32 nSize, eShapeType shapeType, uint32 npoints, uint32 nparts, uint32 ncurves)
	{
		initShapeBufferBuffer(pBuf, shapeType, npoints, nparts, ncurves);
		m_params.set(pBuf);
	}

	bool CGeoShapeBuf::create(eShapeType shapeType, uint32 npoints, uint32 nparts, uint32 ncurves, uint32 mpatchSpecificSize)
	{
		

		uint32  nBufSize  = calcSize(shapeType, npoints, nparts, ncurves,mpatchSpecificSize);
		if(nBufSize)
		{
			m_blob.resize(nBufSize);
			initShapeBufferBuffer(m_blob.buffer(), shapeType, npoints, nparts, ncurves);
		}
		m_params.set(m_blob.buffer());
		return true;
	}

	void CGeoShapeBuf::import(const byte* extBuf, uint32 extBufSize)
	{

		m_blob.copy(extBuf, extBufSize);
		m_params.set(m_blob.buffer());
	}

	void CGeoShapeBuf::attach(byte* extBuf, uint32 extBufSize)
	{
		m_blob.attach(extBuf, extBufSize);
		m_params.set(m_blob.buffer());
	}

	unsigned char* CGeoShapeBuf::detach()
	{
		
		m_params.reset();
		return m_blob.deattach();
	}

	uint32 CGeoShapeBuf::pointCount() const
	{
		if(m_params.m_bIsValid)
			return m_params.m_nPointCount;
		return pointCount(m_blob.buffer());
	}

	uint32 CGeoShapeBuf::pointCount(const byte* buf)
	{
		eShapeType genType = generalType(buf);
		return pointCount(buf, genType);
	}

	uint32 CGeoShapeBuf::pointCount(const byte* buf, eShapeType general_type)
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
				return *reinterpret_cast<const uint32*>(buf + 1  + 2 + 8 * 4 +  4 * flag_has_parts); // flag type bbox npart_cnt, 
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

		buf += 1 + 2; //flag, type
		switch(genType)
		{
		case shape_type_general_point:
			buf += 4; //cnt
			break;
		case shape_type_general_multipoint:
			buf +=  8 * 4 + 4; //bbox, cnt
			break;
		case shape_type_general_polyline:
		case shape_type_general_polygon:
			buf += 8 * 4 + 4 + 4 + 4 * nparts; //bbox, part cnt, point cnt, parts,
			break;
		case shape_type_general_multipatch:
			buf +=  8 * 4 + 4 + 4 + (4 + 4) * nparts; //bbox, part cnt, point cnt, parts, type parts,
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

	void CGeoShapeBuf::sShapeParams::set(const byte* buf)
	{
		m_bIsValid = true;

 		// shape type
		m_type = CGeoShapeBuf::type(buf);
		m_general_type = GetGeneralType(m_type);
		m_nPointCount = CGeoShapeBuf::pointCount(buf, m_general_type);
		m_nPartCount = CGeoShapeBuf::partCount(buf, m_general_type);
		m_pPoints = CGeoShapeBuf::getXYs(buf, m_general_type, m_nPartCount);
	}

	bool CGeoShapeBuf::IsSuccinct() const
	{
		if (m_blob.empty())
			return false;
		return (m_blob[0]&eSuccinct) != 0;
	}


	void CGeoShapeBuf::WriteEncodeHeader(CommonLib::IWriteStream *pStream, shape_compress_params *pCompParams)
	{
		
	
		 

		/*if (genType == shape_type_general_polyline || genType == shape_type_polygon)
		{
			calcBB();
		}*/


	
	}

	void  CGeoShapeBuf::WriteCompParams(IWriteStream *pStream, eShapeType genType, shape_compress_params& comp_params)
	{
		if (genType == shape_type_null || genType == shape_type_general_point)
		{
			uint32 nCnt = getPointCnt();
			auto pXYs = getPoints();
			assert(nCnt > 1);
			comp_params.m_dOffsetX = pXYs[0].x;
			comp_params.m_dOffsetY = pXYs[0].y;
			for (uint32 i = 1; i < nCnt; ++i)
			{
				comp_params.m_dOffsetX = min(comp_params.m_dOffsetX, pXYs[i].x);
				comp_params.m_dOffsetY = min(comp_params.m_dOffsetY, pXYs[i].y);
			}
		}
		else
		{
			auto pbbox = getBBoxVals(genType);
			assert(pbbox !=nullptr);
			comp_params.m_dOffsetX = pbbox[0];
			comp_params.m_dOffsetY = pbbox[1];
		}

		pStream->write((byte)comp_params.m_PointType);
		pStream->write((byte)(comp_params.m_nScaleX | (comp_params.m_nScaleY << 4)));
		pStream->write(comp_params.m_dOffsetX);
		pStream->write(comp_params.m_dOffsetY);

	}

	void  CGeoShapeBuf::ReadCompParams(IReadStream *pStream, eShapeType shType, shape_compress_params& comp_params) const
	{
		comp_params.m_PointType = (eCompressDataType)pStream->readByte();
		byte  nScale = pStream->readByte();;

		comp_params.m_nScaleX = nScale & 0xf;
		comp_params.m_nScaleY = nScale >> 4;
		comp_params.m_dOffsetX = pStream->readDouble();
		comp_params.m_dOffsetY = pStream->readDouble();
	}

	void CGeoShapeBuf::calcBB()
	{
		if (IsSuccinct())
		{
			return;
		}

		eShapeType genType;
		eShapeType shType = type();
		bool has_z;
		bool has_m;
		bool has_curve;
		bool has_id;
		getTypeParams(shType, &genType, &has_z, &has_m, &has_curve, &has_id);
	 
		if (genType == shape_type_null || shType == shape_type_null || genType == shape_type_general_point)
			return;

		size_t npoints = pointCount();
		if (npoints == 0)
			return;

		double* xmin = NULL;
		double* xmax = NULL;
		double* ymin = NULL;
		double* ymax = NULL;
		double* zmin = NULL;
		double* zmax = NULL;
		double* mmin = NULL;
		double* mmax = NULL;

		GisXYPoint* xys = getPoints();
		double* xy_bounds = getBBoxVals(genType);
		xmin = xy_bounds + 0;
		ymin = xy_bounds + 1;
		xmax = xy_bounds + 2;
		ymax = xy_bounds + 3;

		*xmin = DBL_MAX;
		*ymin = DBL_MAX;
		*xmax = -DBL_MAX;
		*ymax = -DBL_MAX;

		for (size_t pt = 0; pt < npoints; ++pt)
		{
			*xmin = min(*xmin, xys[pt].x);
			*ymin = min(*ymin, xys[pt].y);
			*xmax = max(*xmax, xys[pt].x);
			*ymax = max(*ymax, xys[pt].y);
		}

		if (has_z)
		{
			double* zs = getZs();
			double* z_bounds = zs - 2;
			zmin = z_bounds + 0;
			zmax = z_bounds + 1;
			*zmin = DBL_MAX;
			*zmax = -DBL_MAX;
			for (size_t pt = 0; pt < npoints; ++pt)
			{
				*zmin = min(*zmin, zs[pt]);
				*zmax = max(*zmax, zs[pt]);
			}
		}

		if (has_m)
		{
			double* ms = getMs();
			double* m_bounds = ms - 2;
			mmin = m_bounds + 0;
			mmax = m_bounds + 1;
			*mmin = DBL_MAX;
			*mmax = -DBL_MAX;
			for (size_t pt = 0; pt < npoints; ++pt)
			{
				*mmin = min(*mmin, ms[pt]);
				*mmax = max(*mmax, ms[pt]);
			}
		}
		if (has_curve)
		{
			//TO DO
		}
	}

	 

	bbox CGeoShapeBuf::getBB() const
	{
		return bbox();
	}

	double *CGeoShapeBuf::getBBoxVals()
	{
		eShapeType genType;
		getTypeParams(type(), &genType);
		return getBBoxVals(genType);
	}
	const double *CGeoShapeBuf::getBBoxVals() const
	{
		eShapeType genType;
		getTypeParams(type(), &genType);
		return getBBoxVals(genType);
	}

	double *CGeoShapeBuf::getBBoxVals(eShapeType genType)
	{
		if (genType == shape_type_null || genType == shape_type_general_point)
			return nullptr;


		return  reinterpret_cast<double*>(m_blob.buffer() + 1 + 2);

	}
	const double *CGeoShapeBuf::getBBoxVals(eShapeType genType) const
	{
		if (genType == shape_type_null || genType == shape_type_general_point)
			return nullptr;

		return  reinterpret_cast<const double*>(m_blob.buffer() + 1 + 2);
	}

	void CGeoShapeBuf::InnerEncode(CWriteMemoryStream *pCacheStream, shape_compress_params *pCompParams)
	{
		if (IsSuccinct() || getPointCnt() < __minimum_point_)
			return;

		CWriteMemoryStream stream;
		CommonLib::CWriteMemoryStream *pStream = &stream;
		if (pCacheStream)
			pStream = pCacheStream;

		pStream->resize(size() * 2);
		pStream->seek(0, soFromBegin);

		eShapeType shType = type();
		eShapeType genType;
		bool has_z;
		bool has_m;
		bool has_curve;
		bool has_id;
		getTypeParams(shType, &genType, &has_z, &has_m, &has_curve, &has_id);


		bool bInnerParams = pCompParams ? false : true;

		pStream->write((byte)(eSuccinct | ((bInnerParams ? eInnerParams : eExternParams) << 1)));
		pStream->write((short)shType);
		const double *pbbox = getBBoxVals(genType);
		if (pbbox)
		{
			pStream->write(pbbox[0]);
			pStream->write(pbbox[1]);
			pStream->write(pbbox[2]);
			pStream->write(pbbox[3]);
			if (has_z)
			{
				pStream->write(pbbox[4]);
				pStream->write(pbbox[5]);
			}
			if (has_m)
			{
				pStream->write(pbbox[6]);
				pStream->write(pbbox[7]);
			}

		}
		
		if (pCompParams)
			m_comp_params = *pCompParams;
		else
		{
			WriteCompParams(pStream, genType, m_comp_params);
		}

		m_Encoder.Encode(this, pStream, &m_comp_params);
		m_blob.copy(pStream->buffer(), pStream->pos());
		m_params.reset();

	}


	bool CGeoShapeBuf::BeginReadSuccinct(shape_compress_params *pCompParams) const
	{
		if (!IsSuccinct())
			return false; //TO DO Error


		if (m_bInitSuccinct)
		{
			m_Encoder.ResetDecode(pCompParams ? pCompParams : &m_comp_params);
			return true;
		}
				

		eShapeType shType = type();
		eShapeType genType;
		bool has_z;
		bool has_m;
		bool has_curve;
		bool has_id;
		getTypeParams(shType, &genType, &has_z, &has_m, &has_curve, &has_id);

		uint32 nPos = 1 + 2;//flag + type

		if (genType != shape_type_null && genType != shape_type_general_point)
		{
			nPos += 4 * sizeof(double);
			if(has_z)
				nPos +=2 * sizeof(double);
			if (has_m)
				nPos += 2 * sizeof(double);
		}


		bool bInnerParam = ( (m_blob[0] >> 1) & eInnerParams);

		FxMemoryReadStream stream;

		stream.attachBuffer(m_blob.buffer(), m_blob.size());
		stream.seek(nPos, soFromBegin);

		if ((bInnerParam && pCompParams) || (!bInnerParam && !pCompParams))
			return false; //TO DO

		if (bInnerParam)
			ReadCompParams(&stream, type(), m_comp_params);

		m_Encoder.BeginDecode(&stream, pCompParams);

		m_bInitSuccinct = true;
	}
	void CGeoShapeBuf::EndReadSuccinct() const
	{
		if (!IsSuccinct())
			return;
 
	}
	uint32 CGeoShapeBuf::nextPart(uint32 nIdx) const
	{
		if (!IsSuccinct())
			return 0;

		return m_Encoder.GetNextPart(nIdx);
	}

	GisXYPoint CGeoShapeBuf::nextPoint(uint32 nIdx)
	{
		if (!IsSuccinct())
			return GisXYPoint();

		return m_Encoder.GetNextPoint(nIdx, &m_comp_params);
	}
}