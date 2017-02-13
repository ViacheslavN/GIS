#include "stdafx.h"
#include "ShapeCompressor2.h"
#include "compressutils.h"
#include "NumLenCompressor.h"
#include "PartCompressor.h"
#include "XYCompressDiff.h"
#include "MemoryStream.h"
//#include "XYCompressDiff2.h"
//#include "XYCompressZOrderDiff.h"
namespace CommonLib
{

	#define BIT_OFFSET_COMPRESS_PARAMS 2
	#define BIT_OFFSET_PARTS_NULL 3
	#define BIT_OFFSET_PARTS_COMPRESS 4
	#define BIT_OFFSET_XY_COMPRESS 5

	ShapeCompressor2::ShapeCompressor2(CommonLib::alloc_t *pAlloc) : m_bWriteParams(true), m_bNullPart(false),
		m_bCompressPart(false), m_bCompressPoint(false), m_partType(dtType8), m_pAlloc(pAlloc),
		m_PointCompressor(m_pAlloc)

	{
	}
	ShapeCompressor2::~ShapeCompressor2()
	{

	}

	/*
	-----------------
	shape type 1 byte
	-----------------
	compress flags
	bit nums		desc

	0			2 bit type parts
	2			bit compress params
	3 			bit parts null
	4 			bit compress parts
	5 			bit compress points
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


	uint32 ShapeCompressor2::CalcCompressSize(const CGeoShape *pShp, CGeoShape::compress_params *pParams)
	{
		return pShp->getRowSize() + 100;
	}


 
	bool ShapeCompressor2::compress(const CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IWriteStream *pStream, CommonLib::CWriteMemoryStream *pCacheStream)
	{
		


		CWriteMemoryStream streamCache(m_pAlloc);
		CommonLib::CWriteMemoryStream *pCache = &streamCache;
		if(pCacheStream) 
			pCache = pCacheStream;

		uint32 nCompressSize = sizeof(byte) + sizeof(uint32); //shape type + flags

		m_CompressParams.m_PointType = dtType64;
		m_CompressParams.m_dOffsetX = 0.00000001;
		m_CompressParams.m_dOffsetY = 0.00000001;
		m_CompressParams.m_nScaleX = 8;
		m_CompressParams.m_nScaleY = 8;

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

		nCompressSize += CalcCompressSize(pShp, pParams);
		pCache->seek(0, soFromBegin);
		pCache->resize(nCompressSize);

		pCache->write((byte)pShp->type());
		int nFlagPos = pCache->pos();
		uint32 nFlag = 0;

		if (m_bWriteParams)
			nFlag = WriteCompressParams;
	
		pCache->write(nFlag);

		
		if(m_bWriteParams)
		{
			pCache->write((byte)m_CompressParams.m_PointType); 
			pCache->write(m_CompressParams.m_dOffsetX); 
			pCache->write(m_CompressParams.m_dOffsetY); 
			pCache->write(m_CompressParams.m_nScaleX); 
			pCache->write(m_CompressParams.m_nScaleY); 
		}

		eShapeType genType;
		bool has_z;
		bool has_m;
		bool has_curve;
		bool has_id;
		pShp->getTypeParams(pShp->m_type, &genType, &has_z, &has_m, &has_curve, &has_id);

		m_PartCompressor.compress(pShp, pCache);
		m_PointCompressor.compress(pShp, &m_CompressParams, pCache);

		if (has_z)
		{
			for (uint32 i = 0, sz = pShp->m_vecZs.size(); i < sz; ++i)
			{
				pCache->write(pShp->m_vecZs[i]);
			}

		}

		if (has_m)
		{

			for (uint32 i = 0, sz = pShp->m_vecZs.size(); i < sz; ++i)
			{
				pCache->write(pShp->m_vecMs[i]);
			}
		}


		pStream->write(pCache->buffer(), pCache->pos());
		return true;
	}


	
	

	bool ShapeCompressor2::decompress(CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IReadStream *pStream)
	{

		  pShp->m_type = (eShapeType)pStream->readByte();
		 uint32 nFlag = pStream->readIntu32();

		 m_partType = (eCompressDataType)(nFlag & 3);
		 m_bWriteParams = (nFlag & WriteCompressParams) ? true : false;

		 if(!m_bWriteParams && pParams == NULL)
			 return false;

		 if(!m_bWriteParams)
			 m_CompressParams = *pParams;
		 else
		 {
			m_CompressParams.m_PointType =  (eCompressDataType)pStream->readByte(); 
			m_CompressParams.m_dOffsetX = pStream->readDouble();
			m_CompressParams.m_dOffsetY = pStream->readDouble();
			m_CompressParams.m_nScaleX = pStream->readByte();
			m_CompressParams.m_nScaleY = pStream->readByte();
		 }



		 eShapeType genType;
		 bool has_z;
		 bool has_m;
		 bool has_curve;
		 bool has_id;
		 pShp->getTypeParams(pShp->m_type, &genType, &has_z, &has_m, &has_curve, &has_id);


		 m_PartCompressor.decompress(pShp, pStream);
		 m_PointCompressor.decompress(pShp, &m_CompressParams, pStream);

		 if (has_z)
		 {
			 pShp->m_vecZs.resize(pShp->getPointCnt());

			 for (uint32 i = 0, sz = pShp->m_vecZs.size(); i < sz; ++i)
			 {
				 pShp->m_vecZs[i] = pStream->readDouble();
			 }
		 }

		 if (has_m)
		 {
			 pShp->m_vecMs.resize(pShp->getPointCnt());
			 for (uint32 i = 0, sz = pShp->m_vecMs.size(); i < sz; ++i)
			 {
				 pShp->m_vecMs[i] = pStream->readDouble();
			 }
		 }


		pShp->calcBB();
		return true;
	}


}