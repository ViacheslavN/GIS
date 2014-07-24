#include "stdafx.h"
#include "delegate.h"
#include "thread.h"
//#include "Environment.h"
//#include "GisMutex.h"
//#include "Allocators.h"
//#include "CommonTypes.h"
//#include "GisThread.h"
#ifdef ANDROID
#include <android/log.h>
#include <errno.h>
#include <jni.h>
#endif  
namespace CommonLib
{
#if defined(WIN32) 

#ifndef _WIN32_WCE
  extern "C" {

    unsigned __stdcall thread_func(void* param)
    {

      delegate_t * pThread = reinterpret_cast<delegate_t*>(param);
      (*pThread)();
      return 0;
    }

  }

#else
  DWORD WINAPI thread_func(LPVOID lpParameter)
  {
    delegate_t * pThread = reinterpret_cast<delegate_t*>(lpParameter);
    (*pThread)();
    return 0;
  }

#endif

 ThreadWin32Impl:: ThreadWin32Impl() : function_(0), m_hThread(INVALID_HANDLE_VALUE)
 {
 }
  ThreadWin32Impl::ThreadWin32Impl(delegate_t * function):
  function_(function)
  {
#if (defined(_MT) || defined(_DLL)) && !defined(_WIN32_WCE)
	m_hThread = (HANDLE)(_beginthreadex(NULL, NULL, &thread_func,
      function_, NULL, &m_thread_id));
#else 
    m_hThread = ::CreateThread(NULL, NULL, &thread_func, function_, NULL, (DWORD*)&m_thread_id);
#endif
  }

  void ThreadWin32Impl::start(delegate_t *function){
	  assert(!function_);
	  assert(INVALID_HANDLE_VALUE == m_hThread);
	  function_ = function;
#if (defined(_MT) || defined(_DLL)) && !defined(_WIN32_WCE)
	  m_hThread = (HANDLE)(_beginthreadex(NULL, NULL, &thread_func,
		  function_, NULL, &m_thread_id));
#else 
	  m_hThread = ::CreateThread(NULL, NULL, &thread_func, function_, NULL, (DWORD*)&m_thread_id);
#endif
  }
  int32 ThreadWin32Impl::GetCallingThreadId()
  {
    return ::GetCurrentThreadId();
  }


  ThreadWin32Impl::~ThreadWin32Impl()
  {
    //assert(WAIT_TIMEOUT != ::WaitForSingleObject(m_hThread, 100));
    ::WaitForSingleObject(m_hThread, INFINITE);
    CloseHandle(m_hThread);
    if(function_)
      delete function_;
  }
  bool ThreadWin32Impl::Wait(const int Timeout) const
  {
    return WAIT_OBJECT_0 == ::WaitForSingleObject(m_hThread, Timeout);
  }

#elif defined(__IPHONE_3_1)	|| defined (ANDROID)
  // Regular non-realtime thread function
  static void * thread_func(void *param)
  {

    delegate_t * pThread = reinterpret_cast<delegate_t*>(param);
#ifdef  __IPHONE_3_1
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    (*pThread)();
    [pool release];
#else
    (*pThread)();
#endif
    return NULL;
  }


  GisThreadPosixImpl::GisThreadPosixImpl(delegate_t *function):
  function_(function)
  {				

    pthread_attr_t threadAttr;		
    // initialize the thread attribute
    if (pthread_attr_init(&threadAttr) == -1)
    {                                        
      perror("error in pthread_attr_init ***");                                     
      exit(1);                                                                  
    } 	

    /*
    // Set the stack size of the thread
    assert(!pthread_attr_setstacksize(&threadAttr,
    1024*1024*2));

    size_t stackSize;                                                   

    if (pthread_attr_getstacksize(&threadAttr, &stackSize) == -1)
    {                         
    perror("error in pthread_attr_getstackstate() ***");                           
    exit(2);                                                                   
    }                                                                            
    //printf("The stack size is %d. ***\n", (int) stackSize);
    */

    int ret = pthread_create(&tid_, //NULL instead of threadAttr
      &threadAttr,
      thread_func, function_);
    if(ret != 0){


    }

    if (pthread_attr_destroy(&threadAttr) == -1)
    {                                     
      perror("error in pthread_attr_destroy ***");                                  
      exit(3);
    }	
  }	

  GisThreadPosixImpl::~GisThreadPosixImpl()
  {
    // like m_thread.h/cpp
    //pthread_t pt=pthread_self();
    //pthread_detach(pt);

    // wait: "tid_" will be stopped
    int err = pthread_join(tid_, NULL);
    assert(!err);
#ifdef  __IPHONE_3_1
    err = pthread_cancel(tid_);
#endif
    //assert(!err);

    if(function_)
      delete function_;
  }
  bool GisThreadPosixImpl::Wait(const int Timeout) const
  {
    return (0 == pthread_join(tid_, NULL));
  }

  uint32 GisThreadPosixImpl::GetCallingThreadId()
  {
#ifdef  __IPHONE_3_1
    return reinterpret_cast<uint32>(pthread_self());
#else
    return pthread_self();
#endif
  }

/*#elif defined (ANDROID)

  GisThreadJavaImpl::GisThreadJavaImpl(delegate_t *function)
  {
    JNIEnv *env = (JNIEnv *)Environment::Instance().GetContext();
    jclass EnvironmentClass = env->FindClass("com/dataeast/ugliteapi/Environment");
    jmethodID createThread = env->GetStaticMethodID(EnvironmentClass, "createThread", "(I)V");
    env->CallStaticVoidMethod(EnvironmentClass, createThread, (int)function);
  }
  GisThreadJavaImpl::~GisThreadJavaImpl()
  {
  }*/


#endif //__IPHONE_3_1
}