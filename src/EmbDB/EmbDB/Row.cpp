#include "stdafx.h"
#include "Row.h"
#include "FieldSet.h"
namespace embDB
{
	CRow::CRow(IFields* pFields, IFieldSet* pFieldSet)
	{
		m_pFields = pFields;
		m_pFieldsSet = pFieldSet;
		int fieldCount = m_pFields->GetFieldCount();
		assert(m_pFields.get());
		m_vecFieldMap.resize(fieldCount, -1);

		if(!m_pFieldsSet.get())
			m_pFieldsSet = new CFieldSet();

		if( m_pFieldsSet->GetCount() == 0)
		{
			for(int i = 0; i < fieldCount; ++i)
				m_pFieldsSet->Add(m_pFields->GetField(i)->getName());
		}
		int nFieldsSet = m_pFieldsSet->GetCount();
		m_vecValues.resize(nFieldsSet);
		m_pFieldsSet->Reset();
		CommonLib::CString fieldName;
		int i = 0;
		while(m_pFieldsSet->Next(&fieldName))
		{
			int fieldIndex = m_pFields->FindField(fieldName);
			m_vecFieldMap[fieldIndex] = i;
			m_vecValues[i] = CommonLib::CVariant();
			++i;
		}
	

	}
	 CRow::~CRow()
	{

	}
	 int32 CRow::count() const
	 {
		 return m_vecValues.size();
	 }
	 CommonLib::CVariant* CRow::value(int32 nNum)
	 {
		 return &m_vecValues[nNum];
	 }
	 const CommonLib::CVariant* CRow::value(int32 nNum) const
	 {
		 return &m_vecValues[nNum];
	 }
	 bool CRow::set(CommonLib::CVariant& pValue, int32 nNum)
	 {
		 //TO DO check type
		 m_vecValues[nNum] = pValue;
		 return true; 
	 }

	 bool CRow::IsFieldSelected(int index) const
	 {
		 return m_vecFieldMap[index] >= 0;
	 }
}