#include "stdafx.h"
#include "ShapeCompressor.h"
#include "compressutils.h"
#include "NumLenCompressor.h"
#include "PartCompressor.h"
#include "XYCompress.h"
namespace CommonLib
{

	#define BIT_OFFSET_TYPE_PARTS 2
	#define BIT_OFFSET_COMPRESS_PARAMS 3
	#define BIT_OFFSET_PARTS_NULL 4
	#define BIT_OFFSET_PARTS_COMPRESS 5
	#define BIT_OFFSET_XY_COMPRESS 6

	ShapeCompressor::ShapeCompressor(CommonLib::alloc_t *pAlloc)
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
	-----------------
	type part count parts
	-----------------
	parts
	----------------
	type parts
	count
	---
	---
	----------------
	compress numlen points
	----------------
	not compressed
	Z
	M

	*/




	 
	bool ShapeCompressor::compress(const CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IWriteStream *pStream)
	{
		

		 CGeoShape::compress_params CompressParams;
		CompressParams.m_PointType = dtType64;
		CompressParams.m_dOffsetX = 0.00000001;
		CompressParams.m_dOffsetY = 0.00000001;
		CompressParams.m_dScaleX = 0.00000001;
		CompressParams.m_dScaleY = 0.00000001;

		eDataType partType = dtType8;
		bool bPartCompress = true;
		bool bPartNull = false;
		bool bPointCompress = false;

		pStream->write((byte)pShp->m_type);
		uint32 nFlagPos = pStream->pos();

		uint32 nFlag = 0;
		pStream->write(nFlag);

		if(pParams)
		{
			CompressParams  = *pParams;


		}
		else
		{
			bbox bb = pShp->getBB();


			if(bb.xMin < 0)
				CompressParams.m_dOffsetX = fabs(bb.xMin);
			else
				CompressParams.m_dOffsetX = -1 *bb.xMin;
			if(bb.yMin < 0)
				CompressParams.m_dOffsetX = fabs(bb.yMin);
			else
				CompressParams.m_dOffsetX = -1 *bb.yMin;

			pStream->write((byte)CompressParams.m_PointType); 
			pStream->write(CompressParams.m_dOffsetX); 
			pStream->write(CompressParams.m_dOffsetY); 
			pStream->write(CompressParams.m_dScaleX); 
			pStream->write(CompressParams.m_dScaleY); 
		}

		

		uint32 nPartCount = pShp->getPartCount();
		partType = GetType(nPartCount);
		if(partType != dtType32)
		{
			for (uint32 i = 0; i < nPartCount; i++ )
			{
				uint32 nPart = pShp->getPart(i);

				eDataType type = GetType(nPartCount);
				if(partType < type)
				{
					partType = type;
					if(partType == dtType32)
					{				 
						break;
					}
				}
			}
			

		}
		if(nPartCount < 50)
		{
			bPartCompress = false;
			if(nPartCount == 1 && pShp->getPart(0) == 0)
			{
				bPartNull = true;
			}
			else
			{
				WriteValue(nPartCount, partType, pStream);
				for (uint32 i = 0; i < nPartCount; i++ )
				{
					WriteValue(pShp->getPart(i), partType, pStream);
				}
			}
			
		}
		else
		{
			compressPart(partType, pShp, pStream);
		}
		if(pShp->getPointCnt() < 5)
		{
			pStream->write((byte)pShp->m_vecPoints.size());
			for (uint32 i = 0, sz = pShp->m_vecPoints.size(); i < sz; ++i)
			{
				pStream->write(pShp->m_vecPoints[i].x);
				pStream->write(pShp->m_vecPoints[i].y);
			}
		}
		else
		{
			bPointCompress = true;

			CompressXY(pShp, &CompressParams, pStream);
		}

		eShapeType genType;
		bool has_z;
		bool has_m;
		bool has_curve;
		bool has_id;
		pShp->getTypeParams(pShp->m_type, &genType, &has_z, &has_m, &has_curve, &has_id);

		if(has_z)
		{
			pStream->write(pShp->m_vecZs.size());
			for (uint32 i = 0, sz = pShp->m_vecZs.size(); i < sz; ++i)
			{
				pStream->write(pShp->m_vecZs[i]);
			}
		
		}

		if(has_m)
		{
			pStream->write(pShp->m_vecMs.size());
			for (uint32 i = 0, sz = pShp->m_vecZs.size(); i < sz; ++i)
			{
				pStream->write(pShp->m_vecMs[i]);
			}
		}


		uint32 endPos = pStream->pos();

		pStream->seek(nFlagPos, soFromBegin);
		nFlag = CompressParams.m_PointType;
		nFlag |= (((uint32)partType) << BIT_OFFSET_TYPE_PARTS);
		if(pParams)
			nFlag |= (((uint32)1) << BIT_OFFSET_COMPRESS_PARAMS);
		if(bPartNull)
		{
			nFlag |= (((uint32)1) << BIT_OFFSET_PARTS_NULL);
		}
		else if(bPartCompress)
		{
			nFlag |= (((uint32)1) << BIT_OFFSET_PARTS_COMPRESS);
		}

		if(bPointCompress)
			nFlag |= (((uint32)1) << BIT_OFFSET_XY_COMPRESS);
		
		pStream->write(nFlag);
		pStream->seek(endPos, soFromBegin);
		return true;
	}
	void ShapeCompressor::compressPart(eDataType nPartType, const CGeoShape *pShp, CommonLib::IWriteStream *pStream)
	{

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