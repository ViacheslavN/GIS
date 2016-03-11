#include "stdafx.h"
#include "ShapeCompressor.h"
#include "compressutils.h"
#include "NumLenCompressor.h"
#include "PartCompressor.h"
#include "XYCompress.h"
#include "MemoryStream.h"
namespace CommonLib
{

	#define BIT_OFFSET_TYPE_PARTS 2
	#define BIT_OFFSET_COMPRESS_PARAMS 3
	#define BIT_OFFSET_PARTS_NULL 4
	#define BIT_OFFSET_PARTS_COMPRESS 5
	#define BIT_OFFSET_XY_COMPRESS 6

	ShapeCompressor::ShapeCompressor(CommonLib::alloc_t *pAlloc) : m_bWriteParams(true), m_bNullPart(false),
		m_bCompressPart(false), m_bCompressPoint(false), m_partType(dtType8), m_pAlloc(pAlloc)

	{
	}
	ShapeCompressor::~ShapeCompressor()
	{

	}

	/*
	-----------------
	shape type 1 byte
	-----------------
	compress flags
	bit nums		desc

	0			2 bit type coord
	2			2 bit type parts
	4			bit compress params
	5 			bit parts null
	6 			bit compress parts
	7 			bit compress points
	---------------------
	part Compress Header
	---------------------
	point Compress Header
	---------------------
	parts
	---------------------
	point
	---------------------
	Z
	---------------------
	M
	---------------------
	*/


	uint32 ShapeCompressor::CalcCompressSize(const CGeoShape *pShp, CGeoShape::compress_params *pParams)
	{

		uint32 nCompressSize = sizeof(byte) + sizeof(uint32); //shape type + flags

		m_CompressParams.m_PointType = dtType64;
		m_CompressParams.m_dOffsetX = 0.00000001;
		m_CompressParams.m_dOffsetY = 0.00000001;
		m_CompressParams.m_dScaleX = 0.00000001;
		m_CompressParams.m_dScaleY = 0.00000001;

		if(pParams)
		{
			m_bWriteParams = false;
			m_CompressParams = *pParams;
		}
		else
		{
			bbox bb = pShp->getBB();
			m_bWriteParams = true;

			if(bb.xMin < 0)
				m_CompressParams.m_dOffsetX = fabs(bb.xMin);
			else
				m_CompressParams.m_dOffsetX = -1 *bb.xMin;
			if(bb.yMin < 0)
				m_CompressParams.m_dOffsetX = fabs(bb.yMin);
			else
				m_CompressParams.m_dOffsetX = -1 *bb.yMin;

		}
	

		uint32 nPartCount = pShp->getPartCount();
		m_partType = GetType(nPartCount);
		if(m_partType != dtType32)
		{
			for (uint32 i = 0; i < nPartCount; i++ )
			{
				uint32 nPart = pShp->getPart(i);
				eDataType type = GetType(nPartCount);
				if(m_partType < type)
				{
					m_partType = type;
					if(m_partType == dtType32)
					{				 
						break;
					}
				}
			}
		}
		if(nPartCount == 1)
		{
			m_bNullPart = true;
		}
		else if(nPartCount == 0 || nPartCount < 10)
		{

			m_bCompressPart = false;
			nCompressSize += (GetSizeTypeValue(m_partType) * (nPartCount + 1));
		}
		else
		{
			CreatePartCompressor(m_partType);
			m_PartCompressor->PreCompress(pShp->getParts(), nPartCount);
			nCompressSize += m_PartCompressor->GetCompressSize();
		}
		if(pShp->getPointCnt() < 5)
		{
			m_bCompressPoint = false;
			m_bWriteParams = false;
			nCompressSize += (pShp->getPointCnt() * (2 * sizeof(double)) + 1);
		}
		else
		{
			m_bCompressPoint = true;
			CreateCompressXY(&m_CompressParams);
			m_xyCompressor->PreCompress(pShp->getPoints(), pShp->getPointCnt());
			nCompressSize += m_xyCompressor->GetCompressSize();
		}
		if(m_bWriteParams)
			nCompressSize += sizeof(m_CompressParams);


		eShapeType genType;
		bool has_z;
		bool has_m;
		bool has_curve;
		bool has_id;
		pShp->getTypeParams(pShp->m_type, &genType, &has_z, &has_m, &has_curve, &has_id);

		if(has_z)
		{
			nCompressSize += (pShp->m_vecZs.size() * sizeof(double))  + sizeof(uint32);

		}

		if(has_m)
		{
			nCompressSize += (pShp->m_vecMs.size() * sizeof(double))  + sizeof(uint32);
		}

		return nCompressSize;
	}

	 
	bool ShapeCompressor::compress(const CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IWriteStream *pStream, CommonLib::CWriteMemoryStream *pCacheStream)
	{
		


		CWriteMemoryStream streamCache(m_pAlloc);
		CommonLib::CWriteMemoryStream *pCache = &streamCache;
		if(pCacheStream) 
			pCache = pCacheStream;

		uint32 nCompressSize = CalcCompressSize(pShp, pParams);
		pCache->seek(0, soFromBegin);
		pCache->resize(nCompressSize);

		pCache->write((byte)pShp->type());
		uint32 nFlag = m_CompressParams.m_PointType;
		nFlag |= (((uint32)m_partType) << BIT_OFFSET_TYPE_PARTS);
		if(m_bWriteParams)
			nFlag |= (((uint32)1) << BIT_OFFSET_COMPRESS_PARAMS);
		if(m_bNullPart)
		{
			nFlag |= (((uint32)1) << BIT_OFFSET_PARTS_NULL);
		}
		else if(m_bCompressPart)
		{
			nFlag |= (((uint32)1) << BIT_OFFSET_PARTS_COMPRESS);
		}

		if(m_bCompressPoint)
			nFlag |= (((uint32)1) << BIT_OFFSET_XY_COMPRESS);

		pCache->write(nFlag);

		
		if(m_bWriteParams)
		{
			pCache->write((byte)m_CompressParams.m_PointType); 
			pCache->write(m_CompressParams.m_dOffsetX); 
			pCache->write(m_CompressParams.m_dOffsetY); 
			pCache->write(m_CompressParams.m_dScaleX); 
			pCache->write(m_CompressParams.m_dScaleY); 
		}

		if(!m_bNullPart)
		{
			if(m_bCompressPart)
			{
				m_PartCompressor->WriteHeader(pCache);
			}
		}

		if(m_bCompressPoint)
		{
			m_xyCompressor->WriteHeader(pCache);
		}
		if(m_bCompressPart)
		{
			m_PartCompressor->compress(pShp->getParts(), pShp->getPartCount(), pCache);
		}
		else if(!m_bNullPart)
		{
			uint32 nPartCount = pShp->getPartCount();
			WriteValue(nPartCount, m_partType, pCache);
			for (uint32 i = 0; i < nPartCount; i++ )
			{
				WriteValue(pShp->getPart(i), m_partType, pCache);
			}
		}
		if(m_bCompressPoint)
		{
			m_xyCompressor->compress(pShp->getPoints(), pShp->getPointCnt(), pCache);
		}
		else
		{
			pCache->write((byte)pShp->m_vecPoints.size());
			for (uint32 i = 0, sz = pShp->m_vecPoints.size(); i < sz; ++i)
			{
				pCache->write(pShp->m_vecPoints[i].x);
				pCache->write(pShp->m_vecPoints[i].y);
			}
		}
		eShapeType genType;
		bool has_z;
		bool has_m;
		bool has_curve;
		bool has_id;
		pShp->getTypeParams(pShp->m_type, &genType, &has_z, &has_m, &has_curve, &has_id);

		if(has_z)
		{
			pCache->write(pShp->m_vecZs.size());
			for (uint32 i = 0, sz = pShp->m_vecZs.size(); i < sz; ++i)
			{
				pCache->write(pShp->m_vecZs[i]);
			}
		
		}

		if(has_m)
		{
			pCache->write(pShp->m_vecMs.size());
			for (uint32 i = 0, sz = pShp->m_vecZs.size(); i < sz; ++i)
			{
				pCache->write(pShp->m_vecMs[i]);
			}
		}


		 
		pStream->write(pCache->buffer(), pCache->pos());
		return true;
	}
	void ShapeCompressor::compressPart(eDataType nPartType, const CGeoShape *pShp, CommonLib::IWriteStream *pStream)
	{

	}
	void ShapeCompressor::CreatePartCompressor(eDataType nPartType)
	{

	}
	void ShapeCompressor::CreateCompressXY( CGeoShape::compress_params *pParams)
	{
		switch(pParams->m_PointType)
		{
		case dtType16:
			{
				m_xyCompressor.reset(new TXYCompressor16(*pParams));
			}
			break;
		case dtType32:
			{
				m_xyCompressor.reset(new TXYCompressor32(*pParams));
			}
			break;
		case dtType64:
			{
				m_xyCompressor.reset(new TXYCompressor64(*pParams));
			}
			break;
		}
		
	}
	void ShapeCompressor::CompressXY(const CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IWriteStream *pStream)
	{
		switch(pParams->m_PointType)
		{
			case dtType16:
				{
					TXYCompressor16 compressor(*pParams);
					compressor.compress(pShp->getPoints(), pShp->getPointCnt(), pStream);
				}
				break;
			case dtType32:
				{
					TXYCompressor32 compressor(*pParams);
					compressor.compress(pShp->getPoints(), pShp->getPointCnt(), pStream);
				}
				break;
			case dtType64:
				{
					TXYCompressor64 compressor(*pParams);
					compressor.compress(pShp->getPoints(), pShp->getPointCnt(), pStream);
				}
				break;
		}
	}

	bool ShapeCompressor::decompress(CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IReadStream *pStream)
	{
		return true;
	}


}