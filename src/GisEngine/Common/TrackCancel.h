#ifndef _LIB_GIS_ENGINE_COMMON_TRACK_CANCEL_H_
#define _LIB_GIS_ENGINE_COMMON_TRACK_CANCEL_H_

#include "GisEngineCommon.h"

namespace GisEngine
{
	namespace GisCommon
	{

		class CTrackCancel : public ITrackCancel
		{
		public:
			CTrackCancel();
			virtual ~CTrackCancel();
			virtual void Cancel();
			virtual bool Continue();
			virtual void Reset();
		private:
			volatile  bool   m_bContinue;
		};
	}
}

#endif