#ifndef _EMBEDDED_DATABASE_TRANSACTIONS_INSERT_CURSOR_H_
#define _EMBEDDED_DATABASE_TRANSACTIONS_INSERT_CURSOR_H_

#include "../../../../embDBInternal.h"
 
namespace embDB
{
	

	class CInsertCursor : public IInsertCursor
	{
		public:
			CInsertCursor(IDBTransaction* pTran, ITable* pTable,  IFieldSet *pFileds = 0);
			~CInsertCursor();
			virtual int64 insert(IRow* pRow);
			virtual IFieldSetPtr GetFieldSet() const;
			virtual IFieldsPtr   GetSourceFields() const;
			virtual IRowPtr createRow();
			bool init();
		private:
			bool AddField(const CommonLib::CString& sFieldName);
		private:
			IDBTransactionPtr m_pTran;
			IDBTablePtr m_pTable;
			IFieldSetPtr m_pFieldSet;
			IFieldsPtr   m_pSourceFields;
			typedef std::vector<IValueFieldPtr> TVecInsertFields;
	 
			TVecInsertFields m_vecInsertFields;
	};
}
#endif