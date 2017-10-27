#include "stdafx.h"
#include "LockDBObject.h"

namespace embDB
{
	CLockDBObject::CLockDBObject() : m_nFlag(0)
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


	void CLockDBObject::LockForRead()
	{		
		std::unique_lock<std::mutex> lk(m_mutex);
		if (m_nFlag & eExclusive )
		{
			m_condition.wait(lk, [this]() {return !(m_nFlag & eExclusive); });
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
			m_condition.notify_one();
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
			m_condition.wait(lk, [this]() {return m_nFlag  == eFree; });

		m_nFlag = eExclusive;

	}
	void CLockDBObject::LockForWrite()
	{	
		std::unique_lock<std::mutex> lk(m_mutex);
		if(m_nFlag & (eExclusive|eWrite))
			m_condition.wait(lk, [this]() {return !(m_nFlag & (eExclusive | eWrite)); });

		m_nFlag |= eWrite;
	}
	void CLockDBObject::UnLockForWrite()
	{
		std::unique_lock<std::mutex> lk(m_mutex);
		assert(m_nFlag & eWrite);
		m_nFlag &= ~eWrite;
		m_condition.notify_one();
	}
}