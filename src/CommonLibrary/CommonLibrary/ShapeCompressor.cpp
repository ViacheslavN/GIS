#include "stdafx.h"
#include "ShapeCompressor.h"


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
	1 - bit parts
	1 - bit parttype
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
		
		CGeoShape::eDataType pointType = CGeoShape::ptType64;
		double dOffsetX = 0.00000001;
		double dOffsetY = 0.00000001;
		double dScaleX = 0.00000001;
		double dScaleY = 0.00000001;
		CGeoShape::eDataType partType = CGeoShape::ptType8;
		bool bPart = true;
		bool bPartType = true;
		pStream->write((byte)pShp->m_type);

		if(pParams)
		{
			pointType = (CGeoShape::eDataType)pParams->m_PointType;
			dOffsetX = pParams->m_dOffsetX;
			dOffsetY = pParams->m_dOffsetY;
			dScaleX  = pParams->m_dScaleX;
			dScaleY  = pParams->m_dScaleY;


		}
		else
		{
			bbox bb = pShp->getBB();

			dOffsetX = bb.xMin;
			dOffsetY = bb.yMin;


			pStream->write(dOffsetX); 
			pStream->write(dOffsetY); 
			pStream->write(dScaleX); 
			pStream->write(dScaleY); 
		}

		

		uint32 nPartCount = pShp->getPartCount();
		if(nPartCount == 1)
		{
			bPart = false;
		}
		partType = GetType(nPartCount);
		if(partType != CGeoShape::ptType32)
		{
			for (uint32 i = 0; i < nPartCount; i++ )
			{
				uint32 nPart = pShp->getPart(i);
			}
		}
		

		return true;
	}


	CGeoShape::eDataType ShapeCompressor::GetType(uint32 nValue) const
	{
		if(nValue < 256)
			return CGeoShape::ptType8;
		else if(nValue < 65536)
			return CGeoShape::ptType16;
		 return CGeoShape::ptType32;
	}

	bool ShapeCompressor::decompress(CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IReadStream *pStream)
	{
		return true;
	}
}