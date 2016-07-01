#include "stdafx.h"
#include "SimpleSelectOpCursor.h"
#include "FieldSet.h"
#include "Row.h"
#include "BaseRBTree.h"
#include <set>
namespace embDB
{





	SimpleSelectOpCursor::IndexSearchHandler::IndexSearchHandler (const CommonLib::CVariant& value, OpType opType, IndexFiled* pIndex) :
		ISearhHandler(value, opType), m_bEnd(false)
	{
		m_pIndex = pIndex;
	}
	SimpleSelectOpCursor::IndexSearchHandler::~IndexSearchHandler()
	{

	}
	void SimpleSelectOpCursor::IndexSearchHandler::reset()
	{
		m_bEnd = false;
		if(m_optType == OpEqual || m_optType == OpLess ||  m_optType == OpLessOrEqual )
		{
			if(m_optType == OpEqual && m_pIndex->GetType() == itUnique)
				m_pIndexIterator = m_pIndex->find(&m_value);
			else
				m_pIndexIterator = m_pIndex->lower_bound(&m_value);
		}
		else
		{
			m_pIndexIterator = m_pIndex->lower_bound(&m_value);
		}
		m_bIterEnd = m_pIndexIterator->isNull();
		FillChache();



	}

	void SimpleSelectOpCursor::IndexSearchHandler::FillChache()
	{

		m_vecROWIDs.clear();
		m_nCurrRowID = 0;
		if(!m_pIndexIterator.get())
		{
			m_bEnd = true;
			return;
		}
		if(m_bIterEnd || m_bEnd)
			return;

		CommonLib::CVariant key;
		for (uint32 i = 0; i < m_nCacheCount; ++i)
		{
			if(m_pIndexIterator->isNull())
			{
				m_bIterEnd = true;
				break;
			}

			m_pIndexIterator->getKey(&key);

			if(!m_pValueValidator->IsConditions(m_value, key))
			{
				m_bIterEnd = true;
				break;
			}

			m_vecROWIDs.push_back(m_pIndexIterator->getRowID());
			m_pIndexIterator->next();
		}
		if(m_vecROWIDs.size())
		{
			embDB::comp<uint64> comp;
			m_vecROWIDs.quick_sort(comp);
		}
	}

	SimpleSelectOpCursor::FieldSearchHandler::FieldSearchHandler(const CommonLib::CVariant& value,
		OpType opType, IValueField* pField) : 	ISearhHandler(value, opType), m_bEnd(false)
	{
		m_pField = pField;
	}
    SimpleSelectOpCursor::FieldSearchHandler::~FieldSearchHandler()
	{

	}
	void SimpleSelectOpCursor::FieldSearchHandler::reset()
	{
		m_bEnd = true;
		m_pFieldIterator = m_pField->begin();
		FillChache();
	}

	void SimpleSelectOpCursor::FieldSearchHandler::FillChache()
	{
		m_vecROWIDs.clear();
		if(m_bEnd)
			return;
		if(m_pFieldIterator->isNull())
		{
			m_bEnd = true;
			return;
		}
		CommonLib::CVariant value;
		while(!m_pFieldIterator->isNull())
		{
			m_pFieldIterator->getVal(&value);


			if(m_pValueValidator->IsConditions(m_value, value))
			{
				m_vecROWIDs.push_back(m_pFieldIterator->getRowID());
				if(m_vecROWIDs.size() == m_nCacheCount)
					break;
			}

			m_pFieldIterator->next();
		}
	}

	SimpleSelectOpCursor::SimpleSelectOpCursor(IDBTransaction* pTran, ITable* pTable, 
		IFieldSet *pFieldSet, const wchar_t *pszField, const CommonLib::CVariant& var, OpType opType):
		m_pSearhHandler(NULL), m_nPrevROWID(-1), m_nCurrROWID(-1), m_bEnd(false)
	{
		m_pTran = pTran;
		m_pTable = (IDBTable*)pTable;
		m_pFieldSet = pFieldSet;
		m_pFields = m_pTable->getFields();
		m_sField = pszField;
		m_value = var;
		m_nOpType = opType;
	}
	SimpleSelectOpCursor::~SimpleSelectOpCursor()
	{
		if(m_pSearhHandler)
		{
			delete m_pSearhHandler;
			m_pSearhHandler = NULL;
		}
	}

	bool SimpleSelectOpCursor::Init()
	{

		IValueFieldPtr pFindField = m_pTran->GetField(m_pTable->getName().cwstr(), m_sField.cwstr());
		if(!pFindField.get())
		{
			m_pTran->error(L"not exist field %s", m_sField.cwstr());
			return false;
		}


		IndexFiledPtr pIndex = pFindField->GetIndex();
		if(pIndex.get())
		{
			m_pSearhHandler = new IndexSearchHandler(m_value, m_nOpType, pIndex.get());
		}
		else
		{
			m_pSearhHandler = new FieldSearchHandler(m_value, m_nOpType, pFindField.get());
		}
		m_pSearhHandler->reset();
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
	}

	IFieldSetPtr SimpleSelectOpCursor::GetFieldSet() const
	{
			return m_pFieldSet;
	}
	IFieldsPtr   SimpleSelectOpCursor::GetSourceFields() const
	{
		return m_pFields;
	}
	bool   SimpleSelectOpCursor::IsFieldSelected(int index) const
	{
		return false;
	}
	bool SimpleSelectOpCursor::NextRow(IRowPtr* pRow)
	{
		if(m_bEnd)
			return false;

		m_nPrevROWID = m_nCurrROWID;
		m_nCurrROWID = m_pSearhHandler->nextRowID();
		if(m_nCurrROWID == -1)
		{
			m_bEnd = false;
			return false;
		}

		bool bNext = false;
		if(m_nPrevROWID != -1 && (m_nCurrROWID - m_nPrevROWID) == 1)
			bNext = true;

		SetNext(m_nCurrROWID, bNext);
		m_pCacheRow->SetRow(m_nCurrROWID);
		for (uint32 i = 0, sz = (uint32)m_vecFields.size(); i < sz; ++i)
		{
			SField& field = m_vecFields[i];

			if(!field.m_pFieldIterator->isNull())
				field.m_pFieldIterator->getVal(m_pCacheRow->value(i));
			else
			{	
				CommonLib::CVariant var;
				m_pCacheRow->set(var, i);
			}
		}

		if(pRow)
			*pRow = m_pCacheRow.get();
		return false;
	}
	bool  SimpleSelectOpCursor::value(CommonLib::CVariant* pValue, int32 nNum)
	{

		if(m_bEnd)
			return false;
		if(m_nCurrROWID == -1)
		{
			m_nCurrROWID = m_pSearhHandler->nextRowID();
			if(m_nCurrROWID == -1)
			{
				m_bEnd = false;
				return false;
			}
			SetNext(m_nCurrROWID, false);
		}


		if(nNum < (int32)m_vecFields.size())
		{
			const SField& field = m_vecFields[nNum];
			if(!field.m_pFieldIterator->isNull())
				return field.m_pFieldIterator->getVal(pValue);

			*pValue = CommonLib::CVariant();
			return true;
		}


		return false;
	}
	int64 SimpleSelectOpCursor::GetRowID() const
	{

		if(m_bEnd)
			return -1;
		return m_nCurrROWID;
	}


	void SimpleSelectOpCursor::SetNext(int64 nROWID, bool bNext)
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
						field.m_pValueField->find(nROWID,  field.m_pFieldIterator, field.m_pFieldIterator.get());
					else
					{
						int64 nNextROWID = field.m_pFieldIterator->getRowID();
						if(nNextROWID != nROWID)
							field.m_pValueField->find(nROWID,  field.m_pFieldIterator, field.m_pFieldIterator.get());
					}
				}
				else
				{
					field.m_pValueField->find(nROWID,  field.m_pFieldIterator, field.m_pFieldIterator.get());
				}



			}
			else
				field.m_pValueField->find(nROWID, field.m_pFieldIterator, field.m_pFieldIterator.get());
		}
	}
}