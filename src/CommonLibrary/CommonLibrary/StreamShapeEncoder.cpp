#include "stdafx.h"
#include "StreamShapeEncoder.h"
#include "GeoShapeBuf.h"

namespace CommonLib
{
	namespace Private
	{
		CStreamShapeEncoder::CStreamShapeEncoder(CommonLib::alloc_t* pAlloc) : m_bInit(false), m_pAlloc(pAlloc), m_Flag(0)
#ifndef _PART_ENCODER_
			,m_nPartCnt(0)
#endif
		{
			if (!m_pAlloc)
				m_pAlloc = &m_alloc;
		}
		CStreamShapeEncoder::~CStreamShapeEncoder()
		{

		}

		void CStreamShapeEncoder::clear()
		{
			m_PointEncoder.clear();
#ifdef _PART_ENCODER_
			m_PartEncoder.clear();
#else
			m_nPartCnt = 0;
#endif
			m_bInit = false;
		}

		bool CStreamShapeEncoder::Encode(const CGeoShapeBuf* pShape, IWriteStream *pStream, CGeoShape::compress_params *pParams)
		{
			assert(pShape != nullptr);
			clear();

#ifdef _PART_ENCODER_
			m_PartEncoder.Encode(pShape->getParts(), pShape->getPartCount(), pStream);
#else
			 stream.write(pShape->getPartCount());
			 for (uint32 i = 0, sz = pShape->getPartCount(); i < sz; ++i)
			 {
				 stream.write(pShape->getParts()[i]);
			 }
#endif
			 m_PointEncoder.encode(pShape->getPoints(), pShape->getPointCnt(), pParams, pStream);

			return true;
		} 

		bool CStreamShapeEncoder::BeginDecode(const CBlob& blob)
		{
			if (m_bInit)
				return true;

			m_bInit = true;
			return true;
		}

		uint32 CStreamShapeEncoder::cntParts() const
		{
#ifdef _PART_ENCODER_
			return m_PartEncoder.getPartCnt();
#else
			return m_nPartCnt;
#endif
		}
		uint32 CStreamShapeEncoder::cntPoints() const
		{
			return m_PointEncoder.getPointCnts();
		}

		uint32 CStreamShapeEncoder::GetNextPart(int nIdx)
		{
#ifdef _PART_ENCODER_
			return m_PartEncoder.GetNextPart(nIdx);
#else
			return m_nPartCnt;
#endif
		}
		GisXYPoint CStreamShapeEncoder::GetNextPoint(int nIdx, CGeoShape::compress_params *pParams)
		{
			return m_PointEncoder.GetNextPoint(nIdx, pParams);
		}
	}
}