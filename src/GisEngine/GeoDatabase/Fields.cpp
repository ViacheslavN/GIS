#include "stdafx.h"
#include "Fields.h"

namespace GisEngine
{
	namespace GeoDatabase
	{
		
		CFields::CFields()
		{}

		CFields::~CFields()
		{}

		 
		// IFields
		int CFields::GetFieldCount() const
		{
			return (int)m_vecFields.size();
		}

		void CFields::SetFieldCount(int count)
		{
			m_vecFields.resize(count);
			m_mapFieldIndex.clear();
		}

		IFieldPtr CFields::GetField(int index) const
		{
			return m_vecFields[index];
		}
		IFieldPtr  CFields::GetField(const CommonLib::CString& name) const
		{
			int nIndex = FindField(name);
			if(nIndex == -1)
				return IFieldPtr();
			return GetField(nIndex);
		}
		void CFields::SetField(int index, IField* field)
		{
			m_vecFields[index] = field;
			m_mapFieldIndex.clear();
		}

		void CFields::AddField(IField* field)
		{
			m_vecFields.push_back(IFieldPtr(field));
			m_mapFieldIndex.clear();
		}

		void CFields::RemoveField(int index)
		{
			m_vecFields.erase(m_vecFields.begin() + index);
			m_mapFieldIndex.clear();
		}

		int CFields::FindField(const CommonLib::CString& sName) const
		{
			if(!m_mapFieldIndex.size())
				RebuildFieldIndexMap();

			TMapFieldIndex::iterator it = m_mapFieldIndex.find(sName);
			if(it == m_mapFieldIndex.end())
			{
				int n = sName.reverseFind(L'.');
				if(n >= 0)
					it = m_mapFieldIndex.find(sName.mid(n + 1));
				if(it == m_mapFieldIndex.end())
					return -1;
			}
			return (*it).second;
		}

		bool CFields::FieldExists(const CommonLib::CString& sName) const
		{
			if(!m_mapFieldIndex.size())
				RebuildFieldIndexMap();

			return m_mapFieldIndex.find(sName) != m_mapFieldIndex.end();
		}

		
		void CFields::RebuildFieldIndexMap() const
		{
			m_mapFieldIndex.clear();

			for(size_t i = 0; i < m_vecFields.size(); ++i)
			{
				IFieldPtr field = m_vecFields[i];
				if(!field.get())
					continue;
				m_mapFieldIndex.insert(TMapFieldIndex::value_type(field->GetName(), (int)i));
			}
		}
		void CFields::Clear()
		{
			m_vecFields.clear();
			m_mapFieldIndex.clear();
		}
		IFieldsPtr	CFields::clone() const
		{
			IFieldsPtr pFields(new CFields());
			for(size_t i = 0; i < m_vecFields.size(); ++i)
			{
				IFieldPtr pField = m_vecFields[i];
				pFields->AddField(pField->clone().get());
			}
			return pFields;
		}
	}
}