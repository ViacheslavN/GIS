#include "stdafx.h"
#include "InsertCursor.h"
#include "Row.h"
#include "FieldSet.h"
namespace embDB
{
	CInsertCursor::CInsertCursor(IDBTransaction* pTran, ITable* pTable,  IFieldSet *pFileds)
	{
		m_pTran = pTran;
		m_pTable = (IDBTable*)pTable;
		m_pFieldSet = pFileds;
	}
	CInsertCursor::~CInsertCursor()
	{}

	bool CInsertCursor::init()
	{
		//TO DO check not null fields
		m_vecInsertFields.clear();

		if(!m_pFieldSet.get())
			m_pFieldSet = new CFieldSet();
		if(m_pFieldSet->GetCount() == 0)
		{
			IFieldsPtr pFields = m_pTable->getFields();
			for (size_t i = 0, sz = pFields->GetFieldCount(); i < sz; ++i)
			{
				IFieldPtr pField = pFields->GetField(i);
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
			m_vecInsertFields.push_back(pValueField);
		}

		return true;
	}
	 
	int64 CInsertCursor::insert(IRow* pRow)
	{

		if(pRow->count() < m_vecInsertFields.size())
			return -1;
		int64 nRowID = m_pTable->GetNextOID();

		for (size_t i = 0, sz = m_vecInsertFields.size(); i < sz; ++i)
		{
			CommonLib::CVariant *pValue = pRow->value(i);
			IValueFieldPtr pValueField = m_vecInsertFields[i];
			if(!pValue)
			{
				if(pValueField->getFieldInfoType()->m_nFieldDataType & dteIsNotEmpty)
				{
					m_pTran->error(L"field %s is not null ", pValueField->getFieldInfoType()->m_sFieldName.cwstr());
					return -1;
				}
				continue;
			}

			bool bRet = pValueField->insert(nRowID, pValue);
			if(!bRet) 
				return -1;
		}
		return nRowID;
	}

	IFieldSetPtr CInsertCursor::GetFieldSet() const
	{
		return m_pFieldSet;
	}
	IFieldsPtr   CInsertCursor::GetSourceFields() const
	{
		return m_pTable->getFields();
	}
	IRowPtr CInsertCursor::createRow()
	{
		return IRowPtr(new CRow(GetSourceFields().get(), GetFieldSet().get()));
	}
}