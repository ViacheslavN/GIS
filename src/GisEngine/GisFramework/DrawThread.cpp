#include "stdafx.h"
#include "DrawThread.h"
#include "MapDrawer.h"
namespace GisEngine
{
	namespace GisFramework
	{
		CDrawThread::CDrawThread(CMapDrawer* pDrawer):
	m_pDrawer(pDrawer)
		,m_bStop(false)
		,m_bDraw(false)
		,m_pTtask(NULL)
		,m_Thread(CommonLib::Delegate(this, &CDrawThread::ThreadProc))
	{
		assert(m_pDrawer);
	}
	CDrawThread::~CDrawThread()
	{
		StopThread();
	}
	void CDrawThread::SetTask(IDrawTask* task, bool bDraw)
	{
		{
			CommonLib::CSSection::scoped_lock lock(m_cs);
			m_pTtask = task;
		}
		if(bDraw)
			StartDraw();

	}
	void CDrawThread::StopDraw(bool bClearTask, bool bWait)
	{
		m_bDraw = false;
		if(m_pTtask)
		{
			m_pTtask->StopDraw(bWait);
		}
		if(bWait)
		{
			CommonLib::CSSection::scoped_lock lock(m_cs);
			if(bClearTask)
				m_pTtask = 0;
		}



	}
	void CDrawThread::StopThread()
	{
		StopDraw();
		m_bStop = true;
		m_WorkEvent.set(true);
	}
	void CDrawThread::StartDraw()
	{
		m_bDraw = true;
		m_WorkEvent.set(true);
	}

	bool CDrawThread::IsStop()
	{
		CommonLib::CSSection::scoped_lock lock(m_cs);
		return m_bStop;
	}
	bool CDrawThread::IsDraw()
	{
		CommonLib::CSSection::scoped_lock lock(m_cs);
		return m_bDraw;
	}
	void CDrawThread::ThreadProc()
	{
		while (true)
		{
			m_WorkEvent.wait();
			if(IsStop())
				break;
			{
				CommonLib::CSSection::scoped_lock lock(m_cs);
				if(m_pTtask)
				{
					m_pTtask->Draw();
				}
			}
			m_bDraw = false;
			if(IsStop())
			{
				break;
			}
		}
	}
	}
}
