#include "stdafx.h"
#include "Timer.h"
namespace CommonLib
{
#ifdef WIN32
	CSSection CTimer::m_cs;
	std::map<UINT_PTR, CTimer*> CTimer::m_mapTimers;
#endif

	CTimer::CTimer(uint32 interval)
		: m_nInterval(interval)
#ifdef WIN32
		, m_nID(0)
#endif
	{

	}

	CTimer::~CTimer(void)
	{
		Stop();

	}

	void CTimer::Start()
	{
		assert(!IsStarted());
#ifdef WIN32
		m_nID = ::SetTimer(0, 0, m_nInterval, &CTimer::proc);
		{
			CSSection::scoped_lock lock(m_cs);
			m_mapTimers.insert(std::make_pair(m_nID, this));
		}
#endif
	}

	void CTimer::Stop()
	{
#ifdef WIN32
		if(m_nID != 0)
		{
			::KillTimer(0, m_nID);
			{
				CSSection::scoped_lock lock(m_cs);
				m_mapTimers.erase(m_nID);
			}
			m_nID = 0;
		}
#endif
	}

	bool CTimer::IsStarted()
	{
#ifdef WIN32
		return m_nID != 0;
#endif
	}

	void CTimer::Change(uint32 interval)
	{
		bool restart = IsStarted();
		Stop();
		m_nInterval = interval;
		if(restart)
			Start();
	}

#ifdef WIN32

	void CALLBACK CTimer::proc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
	{
		CTimer* pThis = 0;
		{
			CSSection::scoped_lock lock(m_cs);
			std::map<UINT_PTR, CTimer*>::const_iterator it = m_mapTimers.find(idEvent);
			if(it != m_mapTimers.end())
				pThis = it->second;
		}
		if(pThis)
			pThis->OnTimer(pThis);
	}

#endif

}
