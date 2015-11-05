#ifndef _EMBEDDED_DATABASE_TRANSACTIONS_SIMPLE_SEARCH_CURSOR_H_
#define _EMBEDDED_DATABASE_TRANSACTIONS_SIMPLE_SEARCH_CURSOR_H_

#include "embDBInternal.h"
#include "CommonLibrary/PodVector.h"
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
		void SetCacheObj();
	private:
		IRowPtr m_pCacheRow;
		IDBTransactionPtr m_pTran;
		IIndexIteratorPtr m_pIndexIterator;
		IDBTablePtr		  m_pTable;
		IFieldSetPtr	  m_pFieldSet;
		IFieldsPtr		  m_pFields;
		typedef CommonLib::TPodVector<uint64> TVecOids;
		TVecOids m_vecOIDs;
		uint32 m_nCurrObj;
		uint32 m_nCacheCount;
		uint64 m_nPrevOID;


		struct SField
		{
			IFieldIteratorPtr m_pFieldIterator;
			IValueFieldPtr	  m_pValueField;
		};


		typedef std::vector<SField> TVecValueField;
		TVecValueField m_vecFields;
	};
}

#endif