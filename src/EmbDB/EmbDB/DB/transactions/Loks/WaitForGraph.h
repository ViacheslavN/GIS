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

			virtual bool TryToLockObject(eWaitObjectType type, int64 nObjecID, int64 nTranID, eLockType lockType);
			virtual bool LockObject(eWaitObjectType type, int64 nObjecID, int64 nTranID, eLockType lockType);
			virtual bool FreeObject(eWaitObjectType type, int64 nObjecID, int64 nTranID, eLockType lockType);
		private:

		 
			struct SResVertex
			{
				 
				std::map<int64, eLockType> m_setTranOwners;
				eWaitObjectType type;
				SResVertex()
				{}
			};

			struct STranVertex
			{		 

				std::vector<std::pair<eLockType, SResVertex*> > m_WaitVertex;
				STranVertex()
				{}
			};


			struct SVertex
			{
				eWaitObjectType m_type;
				std::list<SVertex*> m_vertex;

				SVertex(eWaitObjectType type) : m_type(type)
				{}
			};

			typedef std::map<std::pair<int32, int64>, SVertex*> TMapVertex;

			typedef std::map<int64, STranVertex*> TMapTranVertex;
			typedef std::map<std::pair<int32, int64>, SResVertex*> TMapResVertex;

	private:
		bool TryToLockObject(int64 nObjecID, int64 nTranID, STranVertex *pVertexTran, SResVertex *pVertexResObj, eLockType lockType);
		bool CanBeLocked(eLockType lockType, eLockType lockOwnerType) const;
	private:
		CommonLib::CSLockObject m_Lock;
		TMapVertex m_Graf;

		TMapTranVertex m_Transactions;
		TMapResVertex m_Resources;

	};
}

