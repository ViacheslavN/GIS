#ifndef _EMBEDDED_DATABASE_TRANSACTIONS_DELETE_CURSOR_H_
#define _EMBEDDED_DATABASE_TRANSACTIONS_DELETE_CURSOR_H_

#include "embDBInternal.h"
#include "CommonLibrary/PodVector.h"
namespace embDB
{

	class CDeleteCursor : public IDeleteCursor
	{
		public:
			CDeleteCursor(IDBTransaction* pTran, ITable* pTable);
			virtual ~CDeleteCursor();

			bool Init();

			virtual bool remove(IRow* pRow);
			virtual bool remove(int64 nRowID);
		private:
			IDBTransactionPtr m_pTran;
			IDBTablePtr		  m_pTable;

			typedef std::vector<IValueFieldPtr> TVecValueField;
			TVecValueField m_vecFields;
	};
}

#endif