#include "stdafx.h"
#include "ShapeCompressor.h"
#include "compressutils.h"
#include "NumLenCompressor.h"
#include "PartCompressor.h"
#include "XYCompress.h"
namespace CommonLib
{
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
	2 bit type coord
	2 bit type parts
	1 - bit parts null
	1 - bit compress parts
	1 - bit part type
	1 - bit compress points
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




	 
	bool ShapeCompressor::compress(CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IWriteStream *pStream)
	{
		

		 CGeoShape::compress_params CompressParams;
		CompressParams.m_PointType = dtType64;
		CompressParams.m_dOffsetX = 0.00000001;
		CompressParams.m_dOffsetY = 0.00000001;
		CompressParams.m_dScaleX = 0.00000001;
		CompressParams.m_dScaleY = 0.00000001;

		eDataType partType = dtType8;
		bool bPart = true;
		bool bPartNull = false;
		bool bPartType = true;
		pStream->write((byte)pShp->m_type);

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
			bPart = false;
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
		
		

		return true;
	}
	void ShapeCompressor::compressPart(eDataType nPartType, CGeoShape *pShp, CommonLib::IWriteStream *pStream)
	{

	}

	void ShapeCompressor::CompressXY(CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IWriteStream *pStream)
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