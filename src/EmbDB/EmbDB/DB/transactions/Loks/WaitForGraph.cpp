#include "stdafx.h"
#include "WaitForGraph.h"

namespace embDB
{
	CWaitForGraph::CWaitForGraph()
	{

	}
	CWaitForGraph::~CWaitForGraph()
	{

	}

	eWaitRes CWaitForGraph::TryToLockObject(eWaitObjectType type, int64 nObjecID, int64 nTranID, eLockType lockType)
	{
		std::unique_lock<std::mutex> lk(m_mutex);
		STranVertex *pTranVertex = nullptr;
		SResVertex *pResObjVertex = nullptr;

		{
			auto it = m_Transactions.find(nTranID);
			if (it != m_Transactions.end())
				pTranVertex = it->second;
		}
		
		{
			auto it = m_Resources.find(std::make_pair(type, nObjecID));
			if (it != m_Resources.end())
				pResObjVertex = it->second;
		}	

		if (pTranVertex == nullptr)
		{
			pTranVertex = new STranVertex();
			m_Transactions.insert(std::make_pair(nTranID, pTranVertex));
		}

		if (pResObjVertex == nullptr)
		{
			pResObjVertex = new SResVertex();
			m_Resources.insert(std::make_pair(std::make_pair(type, nObjecID), pResObjVertex));
 		}

		return TryToLockObject(nObjecID, nTranID, pTranVertex, pResObjVertex, lockType);

	}
	eWaitRes CWaitForGraph::TryToLockObject(int64 nObjecID, int64 nTranID, STranVertex *pVertexTran, SResVertex *pVertexResObj, eLockType lockType)
	{

		if (pVertexResObj->m_setTranOwners.empty())
		{
			pVertexResObj->m_setTranOwners.insert(std::make_pair(nTranID, lockType));
			pVertexTran->m_nOwnResources += 1;
			return wrObjectCaptured;
		}



		{
			auto it = pVertexResObj->m_setTranOwners.find(nTranID);
			if (it != pVertexResObj->m_setTranOwners.end())
			{
				if (it->second == lockType || pVertexResObj->m_setTranOwners.size() == 1)
				{
					it->second = lockType;
					pVertexTran->m_nOwnResources += 1;
					return wrObjectCaptured;
				}
				pVertexResObj->m_setTranOwners.erase(it);
			}
			bool bCanBeLocked = true;
			for (auto tr_it = pVertexResObj->m_setTranOwners.begin(); tr_it != pVertexResObj->m_setTranOwners.end(); ++tr_it)
			{
				if (!CanBeLocked(lockType, tr_it->second))
				{
					bCanBeLocked = false;
					break;
				}
			}

			if (bCanBeLocked)
			{		 
				pVertexResObj->m_setTranOwners.insert(std::make_pair(nTranID, lockType));
				pVertexTran->m_nOwnResources += 1;
				return wrObjectCaptured;
			}

		}

		std::stack<SResVertex*> stVertex;
		stVertex.push(pVertexResObj);
		
		while (!stVertex.empty())
		{

			SResVertex* pVertex = stVertex.top();
			stVertex.pop();

			for (auto tr_it = pVertex->m_setTranOwners.begin(); tr_it != pVertex->m_setTranOwners.end(); ++tr_it)
			{
				auto tran_it = m_Transactions.find(tr_it->first);
				assert(tran_it != m_Transactions.end());
				auto pTran = tran_it->second;

				if (tran_it->first == nTranID)
				{
					//cycle
					return wrDeadLock;
				}
				for (auto tr_wait_it = pTran->m_WaitVertex.begin(); tr_wait_it != pTran->m_WaitVertex.end(); ++tr_wait_it)
				{

					stVertex.push(tr_wait_it->second);
				}



			}
		}
		pVertexResObj->m_nCountWaiting += 1;
		pVertexTran->m_WaitVertex.push_back(std::make_pair(lockType, pVertexResObj));
		return wrWaitingObject;
	}

	bool CWaitForGraph::CanBeLocked(eLockType lockType, eLockType lockOwnerType) const
	{
		switch (lockType)
		{
		case wltReadLock:
			return lockOwnerType != wltExclusiveLock;
			break;
		case wltWriteLock:
			return lockOwnerType != wltWriteLock && lockOwnerType != wltExclusiveLock;
			break;
		case wltExclusiveLock:
			return lockOwnerType != wltReadLock && lockOwnerType != wltWriteLock && lockOwnerType != wltExclusiveLock;
			break;
		}

		assert(false);
		return false;
	}

	bool CWaitForGraph::LockObject(eWaitObjectType type, int64 nObjecID, int64 nTranID, eLockType lockType)
	{
		std::unique_lock<std::mutex> lk(m_mutex);
		STranVertex *pTranVertex = nullptr;
		SResVertex *pResObjVertex = nullptr;

		{
			auto it = m_Transactions.find(nTranID);
			if (it != m_Transactions.end())
				pTranVertex = it->second;
		}

		{
			auto it = m_Resources.find(std::make_pair(type, nObjecID));
			if (it != m_Resources.end())
				pResObjVertex = it->second;
		}

		assert(pTranVertex != nullptr && pResObjVertex != nullptr);
		bool bFind = false;
		for (auto tr_wait_it = pTranVertex->m_WaitVertex.begin(); tr_wait_it != pTranVertex->m_WaitVertex.end(); ++tr_wait_it) //TO DO probably need a more efficient search 
		{

			if (tr_wait_it->second == pResObjVertex)
			{
				bFind = true;
				pResObjVertex->m_nCountWaiting -= 1;
				pTranVertex->m_WaitVertex.erase(tr_wait_it);
				pTranVertex->m_nOwnResources += 1;
				pResObjVertex->m_setTranOwners.insert(std::make_pair(nTranID, lockType));
				break;
			}
		}

		assert(bFind);
		return bFind;
	}

	bool CWaitForGraph::FreeObject(eWaitObjectType type, int64 nObjecID, int64 nTranID, eLockType lockType)
	{
		std::unique_lock<std::mutex> lk(m_mutex);
		STranVertex *pTranVertex = nullptr;
		SResVertex *pResObjVertex = nullptr;

		
		auto tran_it = m_Transactions.find(nTranID);
		if (tran_it != m_Transactions.end())
			pTranVertex = tran_it->second;
		

		
		auto res_it = m_Resources.find(std::make_pair(type, nObjecID));
		if (res_it != m_Resources.end())
			pResObjVertex = res_it->second;
		

		assert(pTranVertex != nullptr && pResObjVertex != nullptr);


		auto it = pResObjVertex->m_setTranOwners.find(nTranID);
		if (it == pResObjVertex->m_setTranOwners.end())
		{
			assert(false);
			return false;
		}

		pResObjVertex->m_setTranOwners.erase(it);
		if (pResObjVertex->m_setTranOwners.empty() && pResObjVertex->m_nCountWaiting == 0)
		{
			m_Resources.erase(res_it);
			delete pResObjVertex;
		}

		pTranVertex->m_nOwnResources -= 1;

		if (pTranVertex->m_WaitVertex.empty() && pTranVertex->m_nOwnResources == 0)
		{
			m_Transactions.erase(tran_it);
			delete pTranVertex;
		}
		return true;
	}
}