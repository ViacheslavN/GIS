#include "stdafx.h"
#include "DeleteCursor.h"

namespace embDB
{
	CDeleteCursor::CDeleteCursor(IDBTransaction* pTran, ITable* pTable)
	{
		m_pTran = pTran;
		m_pTable = (IDBTable*)pTable;
	}
	CDeleteCursor::~CDeleteCursor()
	{

	}

	bool CDeleteCursor::Init()
	{

		for (size_t i = 0, sz = m_pTable->getFieldCnt(); i < sz; ++i)
		{
			embDB::IFieldPtr pField = m_pTable->getField(i);
			IValueFieldPtr pValueField = m_pTran->GetField(m_pTable->getName().cwstr(), pField->getName().cwstr());
			m_vecFields.push_back(pValueField);
		}

		return true;
	
	}

	bool CDeleteCursor::remove(IRow* pRow)
	{
		return remove(pRow->GetRowID());
	}
	bool CDeleteCursor::remove(int64 nRowID)
	{
		for (size_t i = 0, sz = m_vecFields.size(); i < sz; ++i)
		{
			m_vecFields[i]->remove(nRowID);
		}

		return !m_pTran->isError();
	}
}