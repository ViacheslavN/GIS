#ifndef _LIB_COMMON_EVENT_H
#define _LIB_COMMON_EVENT_H

namespace CommonLib
{

class CEvent
{
public:
  CEvent(bool autoReset = true)
  {
#ifdef WIN32
    handle_ = ::CreateEvent(0, autoReset?FALSE: TRUE, FALSE, 0);
#elif defined(__IPHONE_3_1) || defined (ANDROID)
    auto_reset_ = autoReset;
		ready_to_go_ = false;
	  int err = pthread_mutex_init(&mutex_, NULL);
	  assert(!err);
	  err = pthread_cond_init(&cond_, NULL);	  
	  assert(!err);
#endif
  }
  ~CEvent()
  {
#ifdef WIN32
    ::CloseHandle(handle_);
#elif defined(__IPHONE_3_1) || defined (ANDROID)
	  int err = pthread_mutex_lock(&mutex_);
	  assert(!err);
	  err = pthread_cond_destroy(&cond_);
	  assert(!err);
	  err = pthread_mutex_unlock(&mutex_);
	  assert(!err);
	  err = pthread_mutex_destroy(&mutex_);
	  assert(!err);
#endif
  }
public:
  void pulse()
  {
#ifdef WIN32
    ::PulseEvent(handle_);
#elif defined(__IPHONE_3_1) || defined (ANDROID)
	assert(!"NO ANALOG OF ::PulseEvent IN PTHREAD FUNCTIONS!");
#endif	  
  }
  void wait(uint32 milliseconds = 0xFFFFFFFF)
  {
#ifdef WIN32
    ::WaitForSingleObject(handle_, milliseconds);
#elif defined(__IPHONE_3_1)	|| defined (ANDROID)   
	  // Lock the mutex.
	  assert(!pthread_mutex_lock(&mutex_));
	  // If the predicate is already set,
	  // then the while loop is bypassed;
	  // otherwise, the thread sleeps until the predicate is set.
	  
	  if(milliseconds == 0xFFFFFFFF)
		{
		  if(!ready_to_go_)
			  assert(!pthread_cond_wait(&cond_, &mutex_));
	  }
		else
		{
			assert(!"MAKE REALIZATION FOR IOS!");
  		//while(!ready_to_go_)
	    //{
	    //	timespec ts;
	  	// ts.tv_sec = milliseconds / 1000;
	  	//	ts.tv_nsec = (milliseconds - ts.tv_sec*1000) * 1000000;		
	    //	assert(!pthread_cond_timedwait_relative_np(&cond_, &mutex_, &ts));
      //}
		}
	  
		// Reset the predicate and release the mutex.
		if(auto_reset_)
      ready_to_go_ = false;

    assert(!pthread_mutex_unlock(&mutex_));
#endif
  }
	
  void set(bool flag)
  {
#ifdef WIN32
    if(flag)
      ::SetEvent(handle_);
    else
      ::ResetEvent(handle_);
#elif defined(__IPHONE_3_1) || defined (ANDROID)
    if (flag)
	  {
		  assert(!pthread_mutex_lock(&mutex_));
		  ready_to_go_ = true;
		  assert(!pthread_cond_signal(&cond_));
		  assert(!pthread_mutex_unlock(&mutex_));
	  }
	  else
	  {
		  assert(!pthread_mutex_lock(&mutex_));
		  ready_to_go_ = false;
		  assert(!pthread_mutex_unlock(&mutex_));
	  }
#endif
  }
private:
#ifdef WIN32
  HANDLE handle_;
#elif defined(__IPHONE_3_1) || defined (ANDROID)
  bool auto_reset_;
	pthread_cond_t cond_;
  pthread_mutex_t mutex_;
  bool ready_to_go_;
#endif
};

}

#endif