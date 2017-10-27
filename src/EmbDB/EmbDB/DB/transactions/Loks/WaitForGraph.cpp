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

	bool CWaitForGraph::TryToLockObject(eWaitObjectType type, int64 nObjecID, int64 nTranID, eLockType lockType)
	{

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

		if (pTranVertex == nullptr)
		{
			pResObjVertex = new SResVertex();
			m_Resources.insert(std::make_pair(std::make_pair(type, nObjecID), pResObjVertex));
 		}

		return TryToLockObject(nObjecID, nTranID, pTranVertex, pResObjVertex, lockType);

	}
	bool CWaitForGraph::TryToLockObject(int64 nObjecID, int64 nTranID, STranVertex *pVertexTran, SResVertex *pVertexResObj, eLockType lockType)
	{

		if (pVertexResObj->m_setTranOwners.empty())
		{
			pVertexResObj->m_setTranOwners.insert(std::make_pair(nTranID, lockType));
			return true;
		}

		bool bOwnYet = false;
		eLockType lockTypeYet;
		{
			auto it = pVertexResObj->m_setTranOwners.find(nTranID);
			if (it != pVertexResObj->m_setTranOwners.end())
			{
				bOwnYet = true;

				if (it->second == lockType)
					return true;
				if (pVertexResObj->m_setTranOwners.size() == 1)
				{
					it->second = lockType;
					return true;
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
				pVertexTran->m_WaitVertex.push_back(std::make_pair(lockType, pVertexResObj));
				return true;
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
				assert(tran_it == m_Transactions.end());
				auto pTran = tran_it->second;

				if (tran_it->first == nTranID)
				{
					//cycle
					return false;
				}
				for (auto tr_wait_it = pTran->m_WaitVertex.begin(); tr_wait_it != pTran->m_WaitVertex.end(); ++tr_wait_it)
				{

					stVertex.push(tr_wait_it->second);
				}



			}
		}

		pVertexTran->m_WaitVertex.push_back(std::make_pair(lockType, pVertexResObj));
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
		return true;
	}

	bool CWaitForGraph::FreeObject(eWaitObjectType type, int64 nObjecID, int64 nTranID, eLockType lockType)
	{
		return true;
	}
}