#ifndef _EMBEDDED_DATABASE_TRANSACTIONS_INSERT_CURSOR_H_
#define _EMBEDDED_DATABASE_TRANSACTIONS_INSERT_CURSOR_H_

#include "embDBInternal.h"
 
namespace embDB
{
	

	class CInsertCursor : public IInsertCursor
	{
		public:
			CInsertCursor(IDBTransaction* pTran, ITable* pTable,  IFieldSet *pFileds = 0);
			~CInsertCursor();
			virtual int64 insert(IRow* pRow) = 0;
			virtual IFieldSetPtr GetFieldSet() const = 0;
			virtual IFieldsPtr   GetSourceFields() const = 0;
		private:
			IDBTransactionPtr m_pTran;
			IFieldSetPtr m_pFieldSet;
			IFieldsPtr   m_pSourceFields;
	};
}
#endif