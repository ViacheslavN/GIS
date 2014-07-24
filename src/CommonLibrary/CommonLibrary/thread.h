#ifndef _LIB_COMMON_THREAD_H
#define _LIB_COMMON_THREAD_H

#include "general.h"
#include "delegate.h"

namespace CommonLib
{
#ifdef WIN32

class ThreadWin32Impl
{
		public:
			ThreadWin32Impl();
			ThreadWin32Impl(delegate_t * function);
			~ThreadWin32Impl();

			bool Wait(const int Timeout = 0xFFFFFFFF) const;
			static int32 GetCallingThreadId();
			void start(delegate_t *function);
		private:
			void* m_hThread;
			unsigned int  m_thread_id;
			delegate_t * function_;
};



typedef ThreadWin32Impl CThread;
	
#elif defined(__IPHONE_3_1) || defined (ANDROID)
	
	class GisThreadPosixImpl
	{
	public:
		GisThreadPosixImpl();
		GisThreadPosixImpl(delegate_t *function);
		~GisThreadPosixImpl();
		
		bool Wait(const int Timeout = 0xFFFFFFFF) const;
		static uint32 GetCallingThreadId();
		
	private:
		pthread_t tid_;
		unsigned int  m_thread_id;//???
		delegate_t *function_;
	};
	
typedef GisThreadPosixImpl CThread;
/*#elif  defined (ANDROID)	
	class GisThreadJavaImpl
	{
	public:
		
		GisThreadJavaImpl(delegate_t *function);
		~GisThreadJavaImpl();
		
		//bool Wait(const int Timeout = 0xFFFFFFFF) const;
    static uint32 GetCallingThreadId() {return 0;}
	private:
	};
  typedef GisThreadJavaImpl GisThread;*/
#endif
}

#endif