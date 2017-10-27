#pragma once

#include "../../../embDBInternal.h"
#include "CommonLibrary/alloc_t.h"
#include "CommonLibrary/LockObject.h"
#include "CommonLibrary/HashKey.h"
namespace embDB
{
	class CLockDBObject
	{
	public:
		CLockDBObject();
		~CLockDBObject();

		virtual void Lock(eLockType type);
		virtual void UnLock(eLockType type);
		virtual bool IsTypeLock(eLockType type);
		virtual bool TryToLock(eLockType type);

 

	private:
		void LockForRead();
		void LockForWrite();
		void LockForExclusive();

		void UnLockForRead();
		void UnLockForWrite();
		void UnLockForExclusive();
	private:

		enum eFlag
		{
			eFree = 0,
			eRead = 1,
			eWrite = 2,
			eExclusive = 4

		};

		uint32 m_nReaders;
		std::mutex m_mutex;
		uint32 m_nFlag;
		std::condition_variable m_condition;

	};
}
