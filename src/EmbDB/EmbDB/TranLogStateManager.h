#ifndef _EMBEDDED_DATABASE_TRANSACTIONS_LOG_STATE_MANAGER_H_
#define _EMBEDDED_DATABASE_TRANSACTIONS_LOG_STATE_MANAGER_H_
#include "CommonLibrary/GeneralTypes.h"
namespace embDB
{
	enum eTransactionsState
	{
		eTS_EMPTY = 0,
		eTS_BEGIN = 1,
		eTS_SAVE_DB_UNDO_PAGE = 2,
		eTS_SAVE_DB_REDO_PAGE = 3,
		eTS_BEGIN_COPY_TO_DB = 4,
		eTS_FINISH_COPY_TO_DB = 5
	};
	class CTranStorage;
	class CTranLogStateManager
	{
	public:
		CTranLogStateManager(CTranStorage *pStorage);
		~CTranLogStateManager();
		bool Init(int64 nPageAddr, bool bRead);
		bool setState(eTransactionsState eTS);
		void setDBSize(uint64 nSize);
		uint64 getDBSize() const {return m_nDbSize;}
		uint32 getState();
		bool save();
	private:
		CTranStorage *m_pStorage;
		int64 m_nPageAddr;
		int64 m_nDbSize;
		uint32 m_nState;
	};
}

#endif