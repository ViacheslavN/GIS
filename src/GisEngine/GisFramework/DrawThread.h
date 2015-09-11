#ifndef GIS_ENGINE_GIS_FRAEMWORK_DRAW_THREAD_H_
#define GIS_ENGINE_GIS_FRAEMWORK_DRAW_THREAD_H_

#include "DrawTask.h"
#include "CommonLibrary/thread.h"
#include "CommonLibrary/CSSection.h"
#include "CommonLibrary/Event.h"

namespace GisEngine
{
	namespace GisFramework
	{
		class CMapDrawer;
		class CDrawThread
		{
		public:
			CDrawThread(CMapDrawer* pDrawer);
			~CDrawThread();
			void SetTask(IDrawTask* task, bool bDraw = true);
			void StopDraw(bool bClearTask = true, bool bWait = true);
			void StopThread();
			void StartDraw();

		private:
			void ThreadProc();

			bool IsStop();
			bool IsDraw();
		private:


			CMapDrawer* m_pDrawer;
			bool m_bStop;
			bool m_bDraw;
			IDrawTask* m_pTtask;
			CommonLib::CEvent m_WorkEvent;
			CommonLib::CSSection m_cs;
			CommonLib::CThread m_Thread;

		};



	}
}

#endif 