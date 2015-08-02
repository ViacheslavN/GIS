#include "stdafx.h"
#include "FieldSet.h"

namespace GisEngine
{
	namespace GeoDatabase
	{
		CFieldSet::CFieldSet()
		{}

		CFieldSet::~CFieldSet()
		{}

		// IFieldSet
		int CFieldSet::GetCount() const
		{
			return (int)m_vecFields.size();
		}

		bool CFieldSet::Find(const CommonLib::str_t& field) const
		{
			return m_mapIndexes.find(field) != m_mapIndexes.end();
		}

		void CFieldSet::Reset()
		{
			m_it = m_vecFields.begin();
		}

		bool CFieldSet::Next(CommonLib::str_t* field)
		{
			if(m_it == m_vecFields.end())
				return false;

			*field = *(m_it++);
			return true;

		}

		void CFieldSet::Add(const CommonLib::str_t& field)
		{
			TMapIndexes::iterator it = m_mapIndexes.find(field);
			if(it == m_mapIndexes.end())
			{
				m_mapIndexes.insert(TMapIndexes::value_type(field, (int)m_vecFields.size()));
				m_vecFields.push_back(field);
			}
		}

		void CFieldSet::Remove(const CommonLib::str_t& field)
		{
			TMapIndexes::iterator it = m_mapIndexes.find(field);
			if(it != m_mapIndexes.end())
			{
				int index = (*it).second;
				m_mapIndexes.erase(it);
				m_vecFields.erase(m_vecFields.begin() + index);
				for(TMapIndexes::iterator it = m_mapIndexes.begin(); it != m_mapIndexes.end(); ++it)
				{
					if((*it).second > index)
						--(*it).second;
				}
			}
		}

		void CFieldSet::Clear()
		{
			m_vecFields.clear();
			m_mapIndexes.clear();
		}
	}
}
