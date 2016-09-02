#ifndef _EMBEDDED_DATABASE_BASE_LOGGER_H_
#define _EMBEDDED_DATABASE_BASE_LOGGER_H_
#include "CommonLibrary/general.h"
#include  "embDBInternal.h"
#include "CommonLibrary/LockObject.h"
namespace embDB
{

	class LogBase : public ILogger
	{
		public:
			LogBase(bool bMultiThread): m_nLogLevel(0)
			{
				if(bMultiThread)
					m_pLockObj = &m_Lock;
				else
					m_pLockObj = &m_emptyLock;
			}

			virtual ~LogBase(){}


			virtual uint32 GetLogLevel() const
			{
				CommonLib::ILockObject::scoped_lock lock(m_pLockObj);
				return m_nLogLevel;
			}
			virtual void SetLogLevel(uint32 nLogLevel)
			{
				CommonLib::ILockObject::scoped_lock lock(m_pLockObj);
				m_nLogLevel = nLogLevel;
			}

		protected:
			uint32 m_nLogLevel;
			CommonLib::ILockObject *m_pLockObj;
			CommonLib::CEmptyLockObject m_emptyLock;
			CommonLib::CSLockObject m_Lock;
	};

}

#endif