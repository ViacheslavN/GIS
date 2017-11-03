#include "stdafx.h"
#include "../../EmbDB/DB/transactions/Loks/LockDBObject.h"
#include "../../EmbDB/DB/transactions/Loks/WaitForGraph.h"
std::mutex mtx;
std::condition_variable cv;
const int nThreadCounts = 10;
int nThreadResume = 0;

class CTable : public embDB::CLockDBObject
{
	public:
		CTable() : m_nRow(0)
		{}
		~CTable()
		{}

		void AddRow(uint64 nRows) { m_nRow = nRows; }
		uint64 GetRow() const { return m_nRow; }

private:
	uint64 m_nRow;
};



embDB::CWaitForGraph waitForGraph;


void TestLockObject()
{
	CTable m_Table;


	auto read_functor =
		[&]( CTable* pTable)
	{
		while (true)
		{
	 
			embDB::eWaitRes type = waitForGraph.TryToLockObject(embDB::wotTable, 1, ::GetCurrentThreadId(), embDB::wltReadLock);
			assert(type != embDB::wrDeadLock);

			pTable->Lock(embDB::wltReadLock);
			if (type != embDB::wrObjectCaptured)
				waitForGraph.LockObject(embDB::wotTable, 1, ::GetCurrentThreadId(), embDB::wltReadLock);


			::Sleep(100);
			uint64 nRow = pTable->GetRow();
			pTable->UnLock(embDB::wltReadLock);
			waitForGraph.FreeObject(embDB::wotTable, 1, ::GetCurrentThreadId(), embDB::wltReadLock);
			if (nRow > 10000)
			{
				std::unique_lock<std::mutex> lk(mtx);
				nThreadResume += 1;
				if (nThreadResume == 20)
				{
					cv.notify_one();
				}
				break;
			}
			::Sleep(1000);
		}
	
	};



	auto timer_functor =
		[&](CTable* pTable)
	{
		while (true)
		{
			uint32 nReadWaiter = pTable->GetCountOfWaiting(embDB::wltReadLock);
			uint32 nWriteWaiter = pTable->GetCountOfWaiting(embDB::wltWriteLock);
			uint32 nExclusiveWaiter = pTable->GetCountOfWaiting(embDB::wltExclusiveLock);

			std::cout << "ReadWaiter:" << nReadWaiter <<
				" WriteWaiter:" << nWriteWaiter <<
				" ExclusiveWaiter:" << nExclusiveWaiter << '\r';
			::Sleep(100);
		}

	};


	auto write_functor =
		[&](CTable* pTable)
	{
		while (true)
		{
			embDB::eWaitRes type = waitForGraph.TryToLockObject(embDB::wotTable, 1, ::GetCurrentThreadId(), embDB::wltWriteLock);
			assert(type != embDB::wrDeadLock);
			pTable->Lock(embDB::wltWriteLock);
			if (type != embDB::wrObjectCaptured)
				waitForGraph.LockObject(embDB::wotTable, 1, ::GetCurrentThreadId(), embDB::wltWriteLock);

			uint64 nRow = pTable->GetRow();
			if (nRow > 10000)
			{
				pTable->UnLock(embDB::wltWriteLock);
				waitForGraph.FreeObject(embDB::wotTable, 1, ::GetCurrentThreadId(), embDB::wltWriteLock);
				std::unique_lock<std::mutex> lk(mtx);
				nThreadResume += 1;
				if (nThreadResume == 20)
				{
					cv.notify_one();
				}
				break;
			}

			::Sleep(0);
			type = waitForGraph.TryToLockObject(embDB::wotTable, 1, ::GetCurrentThreadId(), embDB::wltExclusiveLock);
			assert(type != embDB::wrDeadLock);
			pTable->ChangeLock(embDB::wltWriteLock, embDB::wltExclusiveLock);

			if (type != embDB::wrObjectCaptured)
				waitForGraph.LockObject(embDB::wotTable, 1, ::GetCurrentThreadId(), embDB::wltExclusiveLock);
		
			pTable->AddRow(nRow + 100);
			pTable->UnLock(embDB::wltExclusiveLock);
			waitForGraph.FreeObject(embDB::wotTable, 1, ::GetCurrentThreadId(), embDB::wltExclusiveLock);
			::Sleep(100);
		}

	};


	std::thread read_threads[nThreadCounts];
	std::thread write_threads[nThreadCounts];

	for (size_t i = 0; i < nThreadCounts; ++i)
	{
		read_threads[i] = std::thread(read_functor,  &m_Table);
		write_threads[i] = std::thread(write_functor, &m_Table);
	}



	std::thread timerThread = std::thread(timer_functor, &m_Table);

	std::unique_lock<std::mutex> lck(mtx);
	cv.wait(lck);
 
	return;
}