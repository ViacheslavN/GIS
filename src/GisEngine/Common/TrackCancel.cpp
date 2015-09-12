#include "stdafx.h"
#include "TrackCancel.h"

namespace GisEngine
{
	namespace GisCommon
	{
		CTrackCancel::CTrackCancel() : m_bContinue(true)
		{

		}
		CTrackCancel::~CTrackCancel()
		{

		}
		void CTrackCancel::Cancel()
		{
			m_bContinue = false;
		}
		bool CTrackCancel::Continue()
		{
			return m_bContinue;
		}
		void CTrackCancel::Reset()
		{
			m_bContinue = true;
		}
	}
}