#ifndef _LIB_COMMON_LOCK_OBJECT_H
#define _LIB_COMMON_LOCK_OBJECT_H

#include "CSSection.h"

namespace CommonLib
{

	class ILockObject
	{
		public:
			ILockObject(){}
			virtual ~ILockObject(){}

			virtual void lock() = 0;
			virtual void unlock() = 0;
			virtual bool try_lock() = 0;


			class scoped_lock
			{
			public:
				scoped_lock(ILockObject* pObj)
					: m_pObj(pObj)
				{
					assert(pObj);
					m_pObj->lock();
				}
				~scoped_lock()
				{
					unlock();
				}
				void unlock()
				{
					m_pObj->unlock();
				}
			private:
				ILockObject* m_pObj;
			};

			class scoped_try_lock
			{
			public:
				scoped_try_lock(ILockObject* pObj)
					: m_pObj(pObj)
				{
					m_bLocked = m_pObj->try_lock();		
				}
				~scoped_try_lock()
				{
					unlock();
				}
				void unlock()
				{
					if(m_bLocked)
					{
						m_pObj->unlock();
					}		
				}
				bool locked() const
				{
					return m_bLocked;
				}
			private:
				ILockObject* m_pObj;
				bool m_bLocked;
			};

	};



	 

	class CEmptyLockObject : public ILockObject
	{
		public:

			CEmptyLockObject(){}
			virtual ~CEmptyLockObject(){}

			virtual void lock() {}
			virtual void unlock() {}
			virtual bool try_lock() {return true;}

	};




	class CSLockObject : public ILockObject
	{
	public:

		CSLockObject() 
		{}
		virtual ~CSLockObject(){}

		virtual void lock() 
		{
			m_cs.lock();
		}
		virtual void unlock()
		{
			m_cs.release();

		}
		virtual bool try_lock() 
		{
			return m_cs.try_lock();
		}
	private:
		CSSection m_cs;
	};

}


#endif