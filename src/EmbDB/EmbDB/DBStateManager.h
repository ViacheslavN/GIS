#ifndef _EMBEDDED_DATABASE_I_DB_STATE_MANAGER_H_
#define _EMBEDDED_DATABASE_I_DB_STATE_MANAGER_H_
#include "storage.h"
#include "RBMap.h"
#include "embDBInternal.h"
namespace embDB
{

	struct sStorageState 
	{
		int64 m_nPageID;
		uint32 m_nState;
		int64 m_nTranID;

	};
	class CDBStateManager
	{
	public:
		CDBStateManager();
		~CDBStateManager();
		bool Init(int64 nPageID, IDBStorage* pDBStorage, bool bRead);
	private:
		//typedef RBMap<CStorage*, >
		IDBStorage* m_pDBStorage;
		int64 m_nPageID;
	};
}

#endif
