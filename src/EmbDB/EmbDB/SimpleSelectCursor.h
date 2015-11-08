#ifndef _EMBEDDED_DATABASE_TRANSACTIONS_SIMPLE_SELECT_CURSOR_H_
#define _EMBEDDED_DATABASE_TRANSACTIONS_SIMPLE_SELECT_CURSOR_H_

#include "embDBInternal.h"
#include "CommonLibrary/PodVector.h"
namespace embDB
{



	class SimpleSelectCursor : public ICursor
	{
	public:
		SimpleSelectCursor(IDBTransaction* pTran, ITable* pTable,  IFieldSet *pFileds = 0);
		virtual ~SimpleSelectCursor();

		bool Init();

		virtual IFieldSetPtr GetFieldSet() const;
		virtual IFieldsPtr   GetSourceFields() const;
		virtual bool         IsFieldSelected(int index) const;
		virtual bool NextRow(IRowPtr* row);


	private:
		void SetCacheObj();
		bool AllNext(IRowPtr* row);
		bool NextByIter(IRowPtr* row);
	private:
		IRowPtr m_pCacheRow;
		IDBTransactionPtr m_pTran;
		IDBTablePtr		  m_pTable;
		IFieldSetPtr	  m_pFieldSet;
		IFieldsPtr		  m_pFields;

		IFieldIterator	*m_pIterField;
		int32 m_nIterIndex;

		typedef std::vector<IFieldIteratorPtr> TVecValueField;
		TVecValueField m_vecFields;
		bool m_bAllNext;
		
	};
}

#endif