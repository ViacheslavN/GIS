#pragma once

#include "../../../embDBInternal.h"
#include "CommonLibrary/alloc_t.h"
#include "CommonLibrary/LockObject.h"
#include "CommonLibrary/HashKey.h"
namespace embDB
{
	
	class CWaitForGraph : public IWaitForGraph
	{
		public:
			CWaitForGraph();
			~CWaitForGraph();

			virtual eWaitRes TryToLockObject(eWaitObjectType type, int64 nObjecID, int64 nTranID, eLockType lockType);
			virtual bool LockObject(eWaitObjectType type, int64 nObjecID, int64 nTranID, eLockType lockType);
			virtual bool FreeObject(eWaitObjectType type, int64 nObjecID, int64 nTranID, eLockType lockType);
		private:

		 
			struct SResVertex
			{
				 
				std::map<int64, eLockType> m_setTranOwners;
				eWaitObjectType type;
				uint32 m_nCountWaiting;
				SResVertex(): m_nCountWaiting(0)
				{}
			};

			struct SWatingInfo
			{
				eLockType m_type;
				SResVertex* m_pResObj;
				uint32 mDate;
				uint32 m_nTime;
			};

			struct STranVertex
			{		 
				uint32 m_nOwnResources;
				std::vector<std::pair<eLockType, SResVertex*> > m_WaitVertex;
				STranVertex() : m_nOwnResources(0)
				{}
			};


		 
 

			typedef std::map<int64, STranVertex*> TMapTranVertex;
			typedef std::map<std::pair<int32, int64>, SResVertex*> TMapResVertex;

	private:
		eWaitRes TryToLockObject(int64 nObjecID, int64 nTranID, STranVertex *pVertexTran, SResVertex *pVertexResObj, eLockType lockType);
		bool CanBeLocked(eLockType lockType, eLockType lockOwnerType) const;
	private:
		std::mutex m_mutex;
 

		TMapTranVertex m_Transactions;
		TMapResVertex m_Resources;
 	};
}

