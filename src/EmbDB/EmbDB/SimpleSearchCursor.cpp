#include "stdafx.h"
#include "SimpleSearchCursor.h"

namespace embDB
{
	SimpleSearchCursor::SimpleSearchCursor(IIndexIterator* pIndexIterator,  IDBTransaction* pTran, ITable* pTable,  IFieldSet *pFileds)
	{

	}
	SimpleSearchCursor::~SimpleSearchCursor()
	{

	}

	bool SimpleSearchCursor::Init()
	{

	}

	IFieldSetPtr SimpleSearchCursor::GetFieldSet() const
	{
	}
	IFieldsPtr   SimpleSearchCursor::GetSourceFields() const
	{

	}
	bool         SimpleSearchCursor::IsFieldSelected(int index) const
	{

	}
	bool SimpleSearchCursor::NextRow(IRowPtr* row)
	{

	}
}