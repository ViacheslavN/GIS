#include "stdafx.h"
#include "SimpleSearchCursor.h"
#include "../../../../FieldSet.h"
#include "../../../../Row.h"
#include "../../../../BaseRBTree.h"
namespace embDB
{
	SimpleSearchCursor::SimpleSearchCursor(IIndexIterator* pIndexIterator,  IDBTransaction* pTran, ITable* pTable, 
		IFieldSet *pFieldSet) : m_nCacheCount(10000), m_nCurrObj(0), m_bEnd(false)
	{

		m_pTran = pTran;
		m_pIndexIterator = pIndexIterator;
		m_pTable = (IDBTable*)pTable;
		m_pFieldSet = pFieldSet;
		m_pFields = m_pTable->getFields();

		m_vecOIDs.reserve(m_nCacheCount);
	}
	SimpleSearchCursor::~SimpleSearchCursor()
	{

	}

	bool SimpleSearchCursor::Init()
	{
		if(!m_pFieldSet.get())
			m_pFieldSet = new CFieldSet();
		if(m_pFieldSet->GetCount() == 0)
		 
		for (uint32 i = 0, sz = m_pFields->GetFieldCount(); i < sz; ++i)
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
				m_pTran->error(L"not exist field %s", fieldName.cwstr());
				return false;
			}
			IValueFieldPtr pValueField = m_pTran->GetField(m_pTable->getName().cwstr(), pField->getName().cwstr());

			SField field;
			field.m_pValueField = pValueField;
			field.m_pFieldIterator = IFieldIteratorPtr();

			m_vecFields.push_back(field);
		}

		m_pCacheRow = new CRow(m_pFields.get(), m_pFieldSet.get());
		return true;

		//SetCacheObj();
		//m_pCacheRow = new CRow(m_pFields.get(), m_pFieldSet.get());
		//return !m_vecOIDs.empty();
	}
	void SimpleSearchCursor::SetCacheObj()
	{
		m_vecOIDs.clear();

		for (uint32 i = 0; i < m_nCacheCount; ++i)
		{
			if(m_pIndexIterator->isNull())
				break;

			m_vecOIDs.push_back(m_pIndexIterator->getRowID());
			m_pIndexIterator->next();
		}

		if(m_vecOIDs.size())
		{
			embDB::comp<uint64> comp;
			m_vecOIDs.quick_sort(comp);
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

	bool  SimpleSearchCursor::value(CommonLib::CVariant* pValue, int32 nNum)
	{
		if(m_bEnd)
			return false;
		if(nNum < (int32)m_vecFields.size())
		{
			const SField& field = m_vecFields[nNum];
			if(!field.m_pFieldIterator.get())
				SetNext(m_vecOIDs[0], false);

			if(!field.m_pFieldIterator->isNull())
				return field.m_pFieldIterator->getVal(pValue);

			*pValue = CommonLib::CVariant();
			return true;
		}
		return false;
	}
	int64 SimpleSearchCursor::GetRowID() const
	{
		if(!m_bEnd)
			return m_vecOIDs[m_nCurrObj];
		return -1;
	}
	bool SimpleSearchCursor::NextRow(IRowPtr* pRow)
	{
		if(m_bEnd)
			return false;


		if(m_vecOIDs.size() == m_nCurrObj)
		{
			SetCacheObj();
			if(m_vecOIDs.empty())
			{
				m_bEnd = true;
				return false;
			}

			m_nCurrObj = 0;
		}


		bool bNext = false;
	

		int64 nCurrID = m_vecOIDs[m_nCurrObj];
		if(m_nCurrObj != 0)
		{
			int64 nPrevOID = m_vecOIDs[m_nCurrObj - 1];
			if((nCurrID - nPrevOID) == 1)
				bNext = true;
		}
		for (uint32 i = 0, sz = (uint32)m_vecFields.size(); i < sz; ++i)
		{
			 

			SField& field = m_vecFields[i];

			if(field.m_pFieldIterator.get())
			{
				if(bNext && !field.m_pFieldIterator->isNull())
				{
					field.m_pFieldIterator->next();
					if(field.m_pFieldIterator->isNull())
						field.m_pValueField->find(nCurrID,  field.m_pFieldIterator, field.m_pFieldIterator.get());//field.m_pFieldIterator = field.m_pValueField->find(nCurrID, field.m_pFieldIterator.get());
					else
					{
						int64 nOID = field.m_pFieldIterator->getRowID();
						if(nOID != nCurrID)
							field.m_pValueField->find(nCurrID, field.m_pFieldIterator, field.m_pFieldIterator.get());
					}
				}
				else
				{
					field.m_pValueField->find(nCurrID, field.m_pFieldIterator, field.m_pFieldIterator.get());
				}

				
				
			}
			else
				field.m_pValueField->find(nCurrID, field.m_pFieldIterator, field.m_pFieldIterator.get());
		
			if(pRow)
			{
				if(!field.m_pFieldIterator->isNull())
					field.m_pFieldIterator->getVal(m_pCacheRow->value(i));
				else
				{	
					CommonLib::CVariant var;
					m_pCacheRow->set(var, i);
				}

				m_pCacheRow->SetRow(m_nCurrObj);
				*pRow = m_pCacheRow.get();
			}

		}
	

		m_nCurrObj++;
		return true;
	}

	void SimpleSearchCursor::SetNext(int64 nOID, bool bNext)
	{
		for (uint32 i = 0, sz = (uint32)m_vecFields.size(); i < sz; ++i)
		{
			SField& field = m_vecFields[i];
			IFieldIteratorPtr pIterator;

			if(field.m_pFieldIterator.get())
			{
				if(bNext && !field.m_pFieldIterator->isNull())
				{
					field.m_pFieldIterator->next();
					if(field.m_pFieldIterator->isNull())
						field.m_pValueField->find(nOID,  field.m_pFieldIterator, field.m_pFieldIterator.get());
					else
					{
						int64 nOID = field.m_pFieldIterator->getRowID();
						if(nOID != nOID)
							field.m_pValueField->find(nOID,  field.m_pFieldIterator, field.m_pFieldIterator.get());
					}
				}
				else
				{
					field.m_pValueField->find(nOID,  field.m_pFieldIterator, field.m_pFieldIterator.get());
				}



			}
			else
				field.m_pValueField->find(nOID, field.m_pFieldIterator, field.m_pFieldIterator.get());
		}
	}
}