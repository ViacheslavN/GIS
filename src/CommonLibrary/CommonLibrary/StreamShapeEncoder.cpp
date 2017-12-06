#include "stdafx.h"
#include "StreamShapeEncoder.h"
#include "GeoShape.h"

namespace CommonLib
{
	namespace Private
	{
		CStreamShapeEncoder::CStreamShapeEncoder(CommonLib::alloc_t* pAlloc) : m_pAlloc(pAlloc)
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
		}

		bool CStreamShapeEncoder::Encode(const CGeoShape* pShape, IWriteStream *pStream, shape_compress_params *pParams)
		{
			assert(pShape != nullptr);
			clear();

#ifdef _PART_ENCODER_
			m_PartEncoder.Encode(pShape->getParts(), pShape->getPartCount(), pStream);
#else
			 stream.write(pShape->getPartCount());
			 for (uint32 i = 1, sz = pShape->getPartCount(); i < sz; ++i)
			 {
				 stream.write(pShape->getParts()[i]);
			 }
#endif
			 m_PointEncoder.encode(pShape->getPoints(), pShape->getPointCnt(), pParams, pStream);

			return true;
		} 

		bool CStreamShapeEncoder::BeginDecode(IReadStream *pStream, shape_compress_params *pParams)
		{

			m_PartEncoder.InitDecode(pStream);
			m_PointEncoder.InitDecode(pStream, pParams);
			return true;
		}

		void CStreamShapeEncoder::ResetDecode(shape_compress_params *pParams)
		{
			m_PartEncoder.Reset();
			m_PointEncoder.Reset();

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

		uint32 CStreamShapeEncoder::GetNextPart(int nIdx) const
		{
#ifdef _PART_ENCODER_
			return m_PartEncoder.GetNextPart(nIdx, m_PointEncoder.getPointCnts());
#else
			return m_nPartCnt;
#endif
		}
		GisXYPoint CStreamShapeEncoder::GetNextPoint(int nIdx, shape_compress_params *pParams) const
		{
			return m_PointEncoder.GetNextPoint(nIdx, pParams);
		}
		bool CStreamShapeEncoder::GetNextPoint(GisXYPoint& pt, int nIdx, shape_compress_params *pParams) const
		{
			return m_PointEncoder.GetNextPoint(pt, nIdx, pParams);
		}
	}
}