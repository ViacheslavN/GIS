#ifndef _LIB_COMMON_MUTEX_H
#define _LIB_COMMON_MUTEX_H

#if (!defined(WIN32) && !defined(ANDROID))  // MD: not the best way :(
#include <Availability.h> // but <Availability.h> defines __IPHONE_3_1 macro
#endif

#ifdef __IPHONE_3_1
#include <pthread.h>
#endif

namespace CommonLib
{
	class CSSection
	{
#ifdef WIN32
	public:
		CSSection()
		{
			InitializeCriticalSection(&section_);
		}
#elif defined(__IPHONE_3_1) || defined (ANDROID)
	public:
		CSSection()
		{
			pthread_mutexattr_t attr;				
			pthread_mutexattr_init(&attr);
			pthread_mutexattr_settype(&attr,
				//PTHREAD_MUTEX_NORMAL);
				PTHREAD_MUTEX_RECURSIVE); // the recursive
			// thread allows us to aquire the same lock
			// again from the same thread
			// but still lock against another thread
			pthread_mutex_init(&mtx_, &attr);
			// MD: default initialization:
			//pthread_mutex_init(&mtx_, NULL);
			pthread_mutexattr_destroy(&attr);

		}
#endif
	public:			
		~CSSection()
		{
#ifdef WIN32
			DeleteCriticalSection(&section_);
#elif defined(__IPHONE_3_1)|| defined (ANDROID)
			pthread_mutex_destroy(&mtx_);
#endif	  
		}
	public:
		void lock()
		{
#ifdef WIN32
			EnterCriticalSection(&section_);
#elif defined(__IPHONE_3_1)|| defined (ANDROID)
			pthread_mutex_lock(&mtx_);
#endif
		}
		bool try_lock()
		{	  
#ifdef WIN32
			return ::TryEnterCriticalSection(&section_) != FALSE;
#elif defined(__IPHONE_3_1)|| defined (ANDROID)
			return (!pthread_mutex_trylock(&mtx_));
#endif
			return true;
		}
		void release()
		{
#ifdef WIN32
			LeaveCriticalSection(&section_);
#elif defined(__IPHONE_3_1)|| defined (ANDROID)
			pthread_mutex_unlock(&mtx_);
#endif	
		}
	private:
#ifdef WIN32
		CRITICAL_SECTION section_;
#elif defined(__IPHONE_3_1)|| defined (ANDROID)
		pthread_mutex_t mtx_;
#endif

	public:
		class scoped_lock
		{
		public:
			scoped_lock(CSSection& mutex)
				: mutex_(mutex)
			{
				mutex_.lock();
			}
			~scoped_lock()
			{
				unlock();
			}
			void unlock()
			{
				mutex_.release();	
			}
		private:
			CSSection& mutex_;
		};

		class scoped_try_lock
		{
		public:
			scoped_try_lock(CSSection& mutex)
				: mutex_(mutex)
			{
				locked_ = mutex_.try_lock();		
			}
			~scoped_try_lock()
			{
				unlock();
			}
			void unlock()
			{
				if(locked_)
				{
					mutex_.release();
				}		
			}
			bool locked() const
			{
				return locked_;
			}
		private:
			CSSection& mutex_;
			bool locked_;
		};
	};	
}

#endif