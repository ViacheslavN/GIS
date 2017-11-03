#include "stdafx.h"
#include "LockDBObject.h"

namespace embDB
{
	CLockDBObject::CLockDBObject() : m_nFlag(0), m_nReaders(0), m_nWaitingReaders(0), m_nWaitingWriters(0), m_nWaitingExclusive(0)
	{}
	CLockDBObject::~CLockDBObject()
	{

	}

	void CLockDBObject::Lock(eLockType type)
	{
		switch (type)
		{
		case embDB::wltReadLock:
			LockForRead();
			break;
		case embDB::wltWriteLock:
			LockForWrite();
			break;
		case embDB::wltExclusiveLock:
			LockForExclusive();
			break;
		}
	}
	void CLockDBObject::ChangeLock(eLockType typeFrom, eLockType typeTo)
	{
		assert(typeFrom == wltWriteLock && typeTo == wltExclusiveLock);//single case
		switch (typeFrom)
		{
		case embDB::wltWriteLock:
			FromWriteToExclusive();
			break;
		}
	}
	void  CLockDBObject::UnLock(eLockType type)
	{
		switch (type)
		{
		case embDB::wltReadLock:
			UnLockForRead();
			break;
		case embDB::wltWriteLock:
			UnLockForWrite();
			break;
		case embDB::wltExclusiveLock:
			UnLockForExclusive();
			break;
		}
	}

	bool CLockDBObject::IsTypeLock(eLockType type)
	{
		std::unique_lock<std::mutex> lk(m_mutex);
		return m_nFlag&type;
	}
	bool CLockDBObject::TryToLock(eLockType type)
	{
		return false;
	}

	void CLockDBObject::LockForRead()
	{		
		std::unique_lock<std::mutex> lk(m_mutex);
		if (m_nFlag & eExclusive )
		{
			AddCountOfWaiting(wltReadLock, true);
			m_condition.wait(lk, [this]()
			{
				return !(m_nFlag & eExclusive); 
			});
			AddCountOfWaiting(wltReadLock, false);
		}
		m_nReaders += 1;
		m_nFlag |= eRead;
	}
	void CLockDBObject::UnLockForRead()
	{
		std::unique_lock<std::mutex> lk(m_mutex);
		assert(m_nReaders != 0 && (m_nFlag & eRead));
		m_nReaders -= 1;
		if (m_nReaders == 0)
		{
			m_nFlag &= ~eRead;
			m_condition.notify_all();
		}
	}

	void CLockDBObject::UnLockForExclusive()
	{
		std::unique_lock<std::mutex> lk(m_mutex);
		assert(m_nFlag == eExclusive);
		m_nFlag = eFree;
		m_condition.notify_all();
	}
	void CLockDBObject::LockForExclusive()
	{
		std::unique_lock<std::mutex> lk(m_mutex);
		if(m_nFlag != eFree)
			m_condition.wait(lk, [this]()
		{
			return m_nFlag  == eFree;
		});

		m_nFlag = eExclusive;

	}
	void CLockDBObject::LockForWrite()
	{	
		std::unique_lock<std::mutex> lk(m_mutex);
		if (m_nFlag & (eExclusive | eWrite))
		{
			AddCountOfWaiting(wltWriteLock, true);
			m_condition.wait(lk, [this]()
			{
				return !(m_nFlag & (eExclusive | eWrite)); 
			});
			AddCountOfWaiting(wltWriteLock, false);
		}

		m_nFlag |= eWrite;
	}
	void CLockDBObject::UnLockForWrite()
	{
		std::unique_lock<std::mutex> lk(m_mutex);
		assert(m_nFlag & eWrite);
		m_nFlag &= ~eWrite;
		m_condition.notify_one();
	}

	void  CLockDBObject::FromWriteToExclusive()
	{
		std::unique_lock<std::mutex> lk(m_mutex);
		assert(m_nFlag & eWrite);  // TO Do needs the check by thread ID
		if (m_nFlag & (eRead))
		{
			AddCountOfWaiting(wltExclusiveLock, true);
			m_condition.wait(lk, [this]() 
			{
				return !(m_nFlag & (eRead)); 
			});
			AddCountOfWaiting(wltExclusiveLock, false);
		}

		m_nFlag = eExclusive;
	}
	uint32 CLockDBObject::GetCountOfWaiting(eLockType type)
	{
		std::shared_lock<std::shared_mutex> lock(m_shmutex);
		switch (type)
		{
		case embDB::wltReadLock:
			return m_nWaitingReaders;
			break;
		case embDB::wltWriteLock:
			return m_nWaitingWriters;
			break;
		case embDB::wltExclusiveLock:
			return m_nWaitingExclusive;
			break;
		}
		return 0;
	}
	void CLockDBObject::AddCountOfWaiting(eLockType type, bool bAdd)
	{
		std::unique_lock<std::shared_mutex> lock(m_shmutex);
		switch (type)
		{
		case embDB::wltReadLock:
			m_nWaitingReaders += bAdd ? 1 : -1;
			break;
		case embDB::wltWriteLock:
			m_nWaitingWriters += bAdd ? 1 : -1;
			break;
		case embDB::wltExclusiveLock:
			m_nWaitingExclusive += bAdd ? 1 : -1;
			break;
		}
	}
}