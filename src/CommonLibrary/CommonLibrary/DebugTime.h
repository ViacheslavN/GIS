#ifndef _LIB_COMMON_DEBUG_TIME_H_
#define _LIB_COMMON_DEBUG_TIME_H_
#include "general.h"
namespace CommonLib
{
	namespace TimeUtils
	{
		class CDebugTime
		{
		public:
			CDebugTime();
			~CDebugTime();

			void start();
			double stop();
		private:
			int64 m_nStartCount;
		};
	}
}

#endif