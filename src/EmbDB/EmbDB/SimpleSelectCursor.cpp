#include "stdafx.h"
#include "SimpleSelectCursor.h"
#include "FieldSet.h"
#include "Row.h"
#include "BaseRBTree.h"
#include <set>
namespace embDB
{
	SimpleSelectCursor::SimpleSelectCursor(IDBTransaction* pTran, ITable* pTable, 
		IFieldSet *pFieldSet): m_bAllNext(true), m_nIterIndex(-1), m_pIterField(NULL), m_bEnd(false)
	{
		m_pTran = pTran;
		m_pTable = (IDBTable*)pTable;
		m_pFieldSet = pFieldSet;
		m_pFields = m_pTable->getFields();

	}
	SimpleSelectCursor::~SimpleSelectCursor()
	{

	}

	bool SimpleSelectCursor::Init()
	{
		if(!m_pFieldSet.get())
			m_pFieldSet = new CFieldSet();
		if(m_pFieldSet->GetCount() == 0)
		{
			for (uint32 i = 0, sz = m_pFields->GetFieldCount(); i < sz; ++i)
			{
				IFieldPtr pField = m_pFields->GetField(i);
				m_pFieldSet->Add(pField->getName());
			}
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
		 
			IFieldIteratorPtr pFieldIterator = pValueField->begin();
			IDBFieldHandlerPtr pFieldHandler = pValueField->GetFieldHandler();

			if(!m_pIterField && pFieldHandler->GetIsNotNull())
			{
				m_pIterField = pFieldIterator.get();
				m_nIterIndex = m_vecFields.size();
			}


			if(!pFieldHandler->GetIsNotNull())
				m_bAllNext = false;
			 

			m_vecFields.push_back(pFieldIterator);
		}
		m_pCacheRow = new CRow(m_pFields.get(), m_pFieldSet.get());
		return true;
	}
 
	IFieldSetPtr SimpleSelectCursor::GetFieldSet() const
	{
		return m_pFieldSet;
	}
	IFieldsPtr   SimpleSelectCursor::GetSourceFields() const
	{
		return m_pFields;
	}
	bool         SimpleSelectCursor::IsFieldSelected(int index) const
	{
		return false;
	}
	bool  SimpleSelectCursor::value(CommonLib::CVariant* pValue, int32 nNum)
	{
		if(m_bEnd)
			return false;

		if(nNum < (int)m_vecFields.size())
		{
			IFieldIteratorPtr& iter = m_vecFields[nNum];
			if(!iter->isNull())
				return iter->getVal(pValue);
			*pValue = CommonLib::CVariant();
			return true;

		}
		return false;
	}
	int64 SimpleSelectCursor::GetRowID() const
	{
		if(m_bEnd)
			return -1;
		return m_pCacheRow->GetRowID();
	}
	bool SimpleSelectCursor::NextRow(IRowPtr* pRow)
	{
		if(m_bAllNext)
			return AllNext(pRow);

		if(m_pIterField)
		 return NextByIter(pRow);


		uint64 nMinOID = 0;
		for (uint32 i = 0, sz = m_vecFields.size(); i < sz; ++i)
		{
			IFieldIteratorPtr& iter = m_vecFields[i];
			if(iter->isNull())
				continue;

			uint64 nRowID = iter->getRowID();
			if(i == 0)
				nMinOID = nRowID;
			else if(nRowID < nMinOID)
				nMinOID = nRowID;

		}
		if(nMinOID == 0)
		{
			m_bEnd = true;
			return false;
		}
		if(pRow)
			m_pCacheRow->SetRow(nMinOID);
		for (uint32 i = 0, sz = m_vecFields.size(); i < sz; ++i)
		{
			IFieldIteratorPtr& iter = m_vecFields[i];
			
			uint64 nRowID = iter->isNull() ? 0 : iter->getRowID();
			if(nRowID == nMinOID)
			{
				if(pRow)
					iter->getVal(m_pCacheRow->value(i));
				iter->next();
			}
			else
			{
				if(pRow)
				{
					CommonLib::CVariant var;
					m_pCacheRow->set(var, i);
				}
			}
		}
		if(pRow)
			*pRow = m_pCacheRow;
		return true;
	}

	bool SimpleSelectCursor::AllNext(IRowPtr* pRow)
	{
		if(m_vecFields[0]->isNull())
		{
			m_bEnd = true;
			return false;
		}

		m_pCacheRow->SetRow(m_vecFields[0]->getRowID());
		for (uint32 i = 0, sz = m_vecFields.size(); i < sz; ++i)
		{
			IFieldIteratorPtr& iter = m_vecFields[i];
			if(pRow)
				iter->getVal(m_pCacheRow->value(i));
			iter->next();
		}
		if(pRow)
			*pRow = m_pCacheRow;
		return true;
	}
	bool SimpleSelectCursor::NextByIter(IRowPtr* pRow)
	{
		if(m_pIterField->isNull())
		{
			m_bEnd = true;
			return false;
		}

		uint64 nCurrRowID = m_pIterField->getRowID();
		if(pRow)
			m_pCacheRow->SetRow(nCurrRowID);
		for (uint32 i = 0, sz = m_vecFields.size(); i < sz; ++i)
		{
			IFieldIteratorPtr& iter = m_vecFields[i];
			uint64 nRowID = iter->isNull() ? 0 : iter->getRowID();
			if(nRowID == nCurrRowID)
			{
				if(pRow)
					iter->getVal(m_pCacheRow->value(i));
				iter->next();
			}
			else
			{
				if(pRow)
				{
					CommonLib::CVariant var;
					m_pCacheRow->set(var, i);
				}
			}
		
		}
		if(pRow)
			*pRow = m_pCacheRow;
		return true;
	}
}