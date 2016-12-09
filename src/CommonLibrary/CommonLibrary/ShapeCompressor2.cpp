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
		m_bCompressPart(false), m_bCompressPoint(false), m_partType(dtType8), m_pAlloc(pAlloc)

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
		/*
	

		uint32 nPartCount = pShp->getPartCount();
		const uint32 *pParts = pShp->getParts();
		m_partType = dtType8;
		for (uint32 i = 1; i < nPartCount; i++ )
		{
			uint32 nPart = pParts[i] - pParts[i - 1];
			eCompressDataType type  = GetCompressType(nPart);
			if(m_partType < type)
			{
				m_partType = type;
				if(m_partType == dtType32)
				{				 
					break;
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
			nCompressSize += (GetSizeTypeValue(m_partType) * (nPartCount));
		}
		else
		{
			m_bCompressPart = true;
			CreatePartCompressor(m_partType);
			//m_PartCompressor->PreCompress(pShp->getParts(), nPartCount);
			//uint32 nPart = m_PartCompressor->GetCompressSize();
			//nCompressSize += nPart;
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
		//	m_xyCompressor->PreCompress(pShp->getPoints(), pShp->getPointCnt());
		//	uint32 nCompSize = m_xyCompressor->GetCompressSize();
		//	nCompressSize += nCompSize;
		}
		//if(m_bWriteParams)
		//	nCompressSize += sizeof(m_CompressParams);


	/*	eShapeType genType;
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

		return pShp->getRowSize() * 2;*/
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
		uint32 nFlag = (uint32)m_partType;
		/*if(m_bWriteParams)
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
			nFlag |= (((uint32)1) << BIT_OFFSET_XY_COMPRESS);*/

		pCache->write(nFlag);

		
		if(m_bWriteParams)
		{
			pCache->write((byte)m_CompressParams.m_PointType); 
			pCache->write(m_CompressParams.m_dOffsetX); 
			pCache->write(m_CompressParams.m_dOffsetY); 
			pCache->write(m_CompressParams.m_nScaleX); 
			pCache->write(m_CompressParams.m_nScaleY); 
		}



		if(!m_bNullPart)
		{
			if(m_bCompressPart)
			{
				m_PartCompressor->WriteHeader(pCache);
			}
			else
			{
				WriteValue(pShp->getPartCount(), m_partType, pCache);
			}
		}

		if(m_bCompressPoint)
		{
			m_xyCompressor->WriteHeader(pCache);
		}
		else
		{
			pCache->write((byte)pShp->m_vecPoints.size());
		}
		if(m_bCompressPart)
		{
			m_PartCompressor->compress(pShp->getParts(), pShp->getPartCount(), pCache);
		}
		else if(!m_bNullPart)
		{
			uint32 nPartCount = pShp->getPartCount();
			//WriteValue(nPartCount, m_partType, pCache);
			const uint32 *pParts = pShp->getParts();
			for (uint32 i = 1; i < nPartCount; i++ )
			{
			 
				WriteValue(pParts[i] - pParts[i - 1], m_partType, pCache);
			}
		}
		if(m_bCompressPoint)
		{
			m_xyCompressor->compress(pShp->getPoints(), pShp->getPointCnt(), pCache);
		}
		else
		{
			if(m_CompressParams.m_PointType != dtType64)
			{
				double dScaleX = 1/pow(10., m_CompressParams.m_nScaleX);
				double dScaleY = 1/pow(10., m_CompressParams.m_nScaleY);
				for (uint32 i = 0, sz = pShp->m_vecPoints.size(); i < sz; ++i)
				{
					uint64 X = (uint64)((pShp->m_vecPoints[i].x + m_CompressParams.m_dOffsetX)/dScaleX);
					uint64 Y = (uint64)((pShp->m_vecPoints[i].y + m_CompressParams.m_dOffsetY)/dScaleY);

					WriteValue(X, m_CompressParams.m_PointType, pCache);
					WriteValue(Y, m_CompressParams.m_PointType, pCache);
				}
			}
			else
			{
				for (uint32 i = 0, sz = pShp->m_vecPoints.size(); i < sz; ++i)
				{
					pCache->write(pShp->m_vecPoints[i].x);
					pCache->write(pShp->m_vecPoints[i].y);
				}
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
			for (uint32 i = 0, sz = pShp->m_vecZs.size(); i < sz; ++i)
			{
				pCache->write(pShp->m_vecZs[i]);
			}
		
		}

		if(has_m)
		{
			 
			for (uint32 i = 0, sz = pShp->m_vecZs.size(); i < sz; ++i)
			{
				pCache->write(pShp->m_vecMs[i]);
			}
		}


		 
		pStream->write(pCache->buffer(), pCache->pos());
		return true;
	}


	void  ShapeCompressor2::CompressPart(const CGeoShape *pShp, CommonLib::IWriteStream *pStream)
	{

		uint32 nPartCount = pShp->getPartCount();
		const uint32 *pParts = pShp->getParts();
		m_partType = dtType8;
		for (uint32 i = 1; i < nPartCount; i++ )
		{
			uint32 nPart = pParts[i] - pParts[i - 1];
			eCompressDataType type  = GetCompressType(nPart);
			if(m_partType < type)
			{
				m_partType = type;
				if(m_partType == dtType32)
				{				 
					break;
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
			WriteValue(pShp->getPartCount(), m_partType, pStream);
			uint32 nPartCount = pShp->getPartCount();
			const uint32 *pParts = pShp->getParts();
			for (uint32 i = 1; i < nPartCount; i++ )
			{

				WriteValue(pParts[i] - pParts[i - 1], m_partType, pStream);
			}
		}
		else
		{
			m_bCompressPart = true;
			compressPart(m_partType,  pShp, pStream);
		}
	}

	void ShapeCompressor2::compressPart(eCompressDataType nPartType, const CGeoShape *pShp, CommonLib::IWriteStream *pStream)
	{
		CreatePartCompressor(m_partType);
	}
	void ShapeCompressor2::CreatePartCompressor(eCompressDataType nPartType)
	{


		if(m_xyCompressor.get())
		{
			if(m_PartCompressor->GetCompressDataType() == nPartType)
			{
				m_PartCompressor->clear();
				return;
			}
		}

		switch(nPartType)
		{
		case dtType8:
			{
				m_PartCompressor.reset(new TPartCompressor8(nPartType));
			}
			break;
		case dtType16:
			{
				m_PartCompressor.reset(new TPartCompressor16(nPartType));
			}
			break;
		case dtType32:
			{
				m_PartCompressor.reset(new TPartCompressor32(nPartType));
			}
			break;
		}
	}
	void ShapeCompressor2::CreateCompressXY( CGeoShape::compress_params *pParams)
	{

		if(m_xyCompressor.get())
		{
			if(m_xyCompressor->GetCompressDataType() == pParams->m_PointType)
			{
				m_xyCompressor->clear(pParams);
				return;
			}
		}


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
	

	bool ShapeCompressor2::decompress(CGeoShape *pShp, CGeoShape::compress_params *pParams, CommonLib::IReadStream *pStream)
	{

		  pShp->m_type = (eShapeType)pStream->readByte();
		 uint32 nFlag = pStream->readIntu32();

		 m_partType = (eCompressDataType)(nFlag & 3);
		 m_bWriteParams = ((nFlag >> BIT_OFFSET_COMPRESS_PARAMS) & 1) ? true : false;
		 m_bNullPart = ((nFlag >> BIT_OFFSET_PARTS_NULL) & 1) ? true : false;
		 m_bCompressPart = ((nFlag >> BIT_OFFSET_PARTS_COMPRESS) & 1) ? true : false;
		 m_bCompressPoint = ((nFlag >> BIT_OFFSET_XY_COMPRESS) & 1) ? true : false;

		 if(m_bWriteParams && pParams == NULL)
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

		 uint32 nParts = 0;
		 uint32 nPoints = 0;

		 eShapeType genType;
		 bool has_z;
		 bool has_m;
		 bool has_curve;
		 bool has_id;
		 pShp->getTypeParams(pShp->m_type, &genType, &has_z, &has_m, &has_curve, &has_id);

		 if(m_bCompressPart)
		 {
			 CreatePartCompressor(m_partType);
			 m_PartCompressor->ReadHeader(pStream);
			 nParts = m_PartCompressor->GetPartCount();
		 }
		 else if(m_bNullPart)
		 {
			 nParts = 1;
		 }
		 else
		 {
			 nParts = ReadValue<uint32>(m_partType, pStream);
		 }
		 if(m_bCompressPoint)
		 {
			 CreateCompressXY(&m_CompressParams);
			 m_xyCompressor->ReadHeader(pStream);
			 nPoints = m_xyCompressor->GetPointCount();
		 }
		 else
		 {
			 nPoints = (uint32)pStream->readByte();
		 }
		 pShp->m_vecParts.resize(nParts);
		 pShp->m_vecPoints.resize(nPoints);


		 if(has_z)
		 {
			 pShp->m_vecZs.resize(nPoints);
		 }

		 if(has_m)
		 {
			  pShp->m_vecMs.resize(nPoints);
		 }

		if(m_bNullPart)
		{
			pShp->getParts()[0] = 0;
		}
		else if(m_bCompressPart)
		{
			m_PartCompressor->decompress(pShp->getParts(), pShp->getPartCount(), pStream);
		}
		else
		{
			pShp->getParts()[0] = 0; 
			for (uint32 i = 1; i < nParts; ++i)
			{
				pShp->getParts()[i] = (uint32)ReadValue<uint32>(m_partType, pStream) + pShp->getParts()[i - 1];
			}
		}

		if(m_bCompressPoint)
		{
			m_xyCompressor->decompress(pShp->getPoints(), pShp->getPointCnt(), pStream);
		}
		else
		{
			if(m_CompressParams.m_PointType != dtType64)
			{
				for (uint32 i = 0, sz = pShp->m_vecPoints.size(); i < sz; ++i)
				{
					double dScaleX = 1/pow(10., m_CompressParams.m_nScaleX);
					double dScaleY = 1/pow(10., m_CompressParams.m_nScaleY);

					uint64 X = ReadValue<uint64>(m_CompressParams.m_PointType, pStream);
					uint64 Y = ReadValue<uint64>(m_CompressParams.m_PointType, pStream);
 
					pShp->getPoints()[i].x =  ((double)X *dScaleX) - m_CompressParams.m_dOffsetX;  
					pShp->getPoints()[i].y  =  ((double)Y *dScaleY) - m_CompressParams.m_dOffsetY;
				}
			}
			else
			{
				for (uint32 i = 0; i < nPoints; ++i)
				{
					pShp->getPoints()[i].x =  pStream->readDouble();
					pShp->getPoints()[i].y =  pStream->readDouble();
				}
			}
		}
		pShp->calcBB();
		return true;
	}


}