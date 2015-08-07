#include "stdafx.h"
#include "Envelope.h"
 

namespace GisEngine
{
	namespace Geometry
	{

		CEnvelope::CEnvelope()
		{}

		CEnvelope::CEnvelope(const GisBoundingBox& box, ISpatialReference* spatRef)
			: m_box(box)
			, m_pSpatialRef(spatRef)
		{}

		CEnvelope::~CEnvelope()
		{}

		

		// IEnvelope
		const GisBoundingBox& CEnvelope::GetBoundingBox() const
		{
			return m_box;
		}

		ISpatialReferencePtr CEnvelope::GetSpatialReference() const
		{
			return m_pSpatialRef;
		}
		GisBoundingBox& CEnvelope::GetBoundingBox()
		{
			return m_box;
		}

		void CEnvelope::SetBoundingBox(const GisBoundingBox& box)
		{
			m_box = box;
		}

		void CEnvelope::SetSpatialReference(ISpatialReference* spatRef)
		{
			m_pSpatialRef = spatRef;
		}

		void CEnvelope::Expand(IEnvelope* envelope)
		{
			if(!envelope)
				return;
			GisBoundingBox box = envelope->GetBoundingBox();
			ISpatialReferencePtr sr = envelope->GetSpatialReference();
			if(sr.get() && m_pSpatialRef.get() && sr.get() != m_pSpatialRef.get())
				sr->Project(m_pSpatialRef.get(), box);
			m_box.expand(box);
		}

		bool CEnvelope::Intersect(IEnvelope* envelope)
		{
			if(envelope)
			{
				GisBoundingBox box = envelope->GetBoundingBox();
				ISpatialReferencePtr sr = envelope->GetSpatialReference();
				if(sr.get() && m_pSpatialRef.get() && sr.get() != m_pSpatialRef.get())
					sr->Project(m_pSpatialRef.get(), box);

				// clipping
				m_box.xMin = max(m_box.xMin, box.xMin);
				m_box.xMax = min(m_box.xMax, box.xMax);
				m_box.yMin = max(m_box.yMin, box.yMin);
				m_box.yMax = min(m_box.yMax, box.yMax);
			}

			// Test for empty
			return !((m_box.xMin > m_box.xMax) || (m_box.yMin > m_box.yMax));
		}

		void CEnvelope::Project(ISpatialReference* spatRef)
		{
			if(!m_pSpatialRef.get() || !spatRef)
				return;

			m_pSpatialRef->Project(spatRef, m_box);
			m_pSpatialRef = spatRef;
		}
	}
}