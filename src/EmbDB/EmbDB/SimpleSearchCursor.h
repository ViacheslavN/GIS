#ifndef _EMBEDDED_DATABASE_TRANSACTIONS_SIMPLE_SEARCH_CURSOR_H_
#define _EMBEDDED_DATABASE_TRANSACTIONS_SIMPLE_SEARCH_CURSOR_H_

#include "embDBInternal.h"

namespace embDB
{



	class SimpleSearchCursor : public ICursor
	{
		public:
			SimpleSearchCursor(IIndexIterator* pIndexIterator,  IDBTransaction* pTran, ITable* pTable,  IFieldSet *pFileds = 0);
			virtual ~SimpleSearchCursor();
			
			bool Init();

			virtual IFieldSetPtr GetFieldSet() const;
			virtual IFieldsPtr   GetSourceFields() const;
			virtual bool         IsFieldSelected(int index) const;
			virtual bool NextRow(IRowPtr* row);


		
	private:
		IRowPtr m_pCacheRow;
		IIndexIteratorPtr m_pIndexIterator;
		IDBTablePtr		  m_pTable;
		IFieldSetPtr	  m_pFieldSet;
		IFieldsPtr		  m_pFields;

	};
}

#endif