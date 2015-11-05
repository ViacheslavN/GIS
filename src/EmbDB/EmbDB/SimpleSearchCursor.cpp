#include "stdafx.h"
#include "SimpleSearchCursor.h"
#include "FieldSet.h"
#include "Row.h"
#include "BaseRBTree.h"
namespace embDB
{
	SimpleSearchCursor::SimpleSearchCursor(IIndexIterator* pIndexIterator,  IDBTransaction* pTran, ITable* pTable, 
		IFieldSet *pFieldSet) : m_nCacheCount(10000), m_nCurrObj(0)
	{
		m_pTran = pTran;
		m_pIndexIterator = pIndexIterator;
		m_pTable = (IDBTable*)pTable;
		m_pFieldSet = pFieldSet;
		m_pFields = m_pTable->getFields();

		m_vecOIDs.reserve(m_nCacheCount); //TO DO set file vector
	}
	SimpleSearchCursor::~SimpleSearchCursor()
	{

	}

	bool SimpleSearchCursor::Init()
	{
		if(!m_pFieldSet.get())
			m_pFieldSet = new CFieldSet();
		if(m_pFieldSet->GetCount() == 0)
		 
		for (size_t i = 0, sz = m_pFields->GetFieldCount(); i < sz; ++i)
		{
			IFieldPtr pField = m_pFields->GetField(i);
			m_pFieldSet->Add(pField->getName());
		}

		m_pFieldSet->Reset();
		CommonLib::CString fieldName;
		while(m_pFieldSet->Next(&fieldName))
		{
			IFieldPtr pField = m_pTable->getField(fieldName);
			if(!pField.get())
			{
				m_pTran->error(L"not exist field %s", pField->getName().cwstr());
				return false;
			}
			IValueFieldPtr pValueField = m_pTran->GetField(m_pTable->getName().cwstr(), pField->getName().cwstr());

			SField field;
			field.m_pValueField = pValueField;
			field.m_pFieldIterator = IFieldIteratorPtr();

			m_vecFields.push_back(field);
		}


		SetCacheObj();
		m_pCacheRow = new CRow(m_pFields.get(), m_pFieldSet.get());
		return true;
	}
	void SimpleSearchCursor::SetCacheObj()
	{
		m_vecOIDs.clear();

		for (size_t i = 0; i < m_nCacheCount; ++i)
		{
			if(m_pIndexIterator->isNull())
				break;

			m_vecOIDs.push_back(m_pIndexIterator->getRowID());
			m_pIndexIterator->next();
		}

		if(m_vecOIDs.size())
		{
			m_vecOIDs.quick_sort(embDB::comp<uint64>());
		}
	}
	IFieldSetPtr SimpleSearchCursor::GetFieldSet() const
	{
		return m_pFieldSet;
	}
	IFieldsPtr   SimpleSearchCursor::GetSourceFields() const
	{
		return m_pFields;
	}
	bool         SimpleSearchCursor::IsFieldSelected(int index) const
	{
		return false;
	}
	bool SimpleSearchCursor::NextRow(IRowPtr* row)
	{
		if(m_vecOIDs.size() == m_nCurrObj)
		{
			SetCacheObj();
			if(m_vecOIDs.empty())
				return false;
		}


		bool bNext = false;
	

		uint64 nCurrID = m_vecOIDs[m_nCurrObj];
		if(m_nCurrObj != 0)
		{
			uint64 nPrevOID = m_vecOIDs[m_nCurrObj - 1];
			if((nCurrID - nPrevOID) == 1)
				bNext = true;
		}
		for (size_t i = 0, sz = m_vecFields.size(); i < sz; ++i)
		{
			 

			SField& field = m_vecFields[i];
			IFieldIteratorPtr pIterator;

			if(field.m_pFieldIterator.get())
			{
				if(bNext && !field.m_pFieldIterator->isNull())
				{
					field.m_pFieldIterator->next();
					if(field.m_pFieldIterator->isNull())
						field.m_pFieldIterator = field.m_pValueField->find(nCurrID, field.m_pFieldIterator.get());
					else
					{
						uint64 nOID = field.m_pFieldIterator->getRowID();
						if(nOID != nCurrID)
							field.m_pFieldIterator = field.m_pValueField->find(nCurrID, field.m_pFieldIterator.get());
					}
				}
				else
				{
					field.m_pFieldIterator = field.m_pValueField->find(nCurrID, field.m_pFieldIterator.get());
				}

				
				
			}
			else
				field.m_pFieldIterator = field.m_pValueField->find(nCurrID, field.m_pFieldIterator.get());
		
			if(!field.m_pFieldIterator->isNull())
				field.m_pFieldIterator->getVal(m_pCacheRow->value(i));
			else
				m_pCacheRow->set(CommonLib::CVariant(), i);

		}
		*row = m_pCacheRow.get();

		m_nCurrObj++;
		return true;
	}
}