#ifndef _LIB_COMMON_TIMER_
#define _LIB_COMMON_TIMER_
#include "general.h"
#include "delegate.h"
#include "CSSection.h"
#include <map>
namespace CommonLib
{

	class CTimer
	{
	public:
		CTimer(uint32 interval);
		~CTimer(void);
	public:
		void Start();
		void Stop();
		bool IsStarted();
		void Change(uint32 interval);
	public:
		CommonLib::Event1<CTimer*> OnTimer;
	private:
		uint32 m_nInterval;
#ifdef WIN32
		static void CALLBACK proc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
		static std::map<UINT_PTR, CTimer*> m_mapTimers;
		UINT_PTR m_nID;
		static CSSection m_cs;
#endif
	};
	
}

#endif