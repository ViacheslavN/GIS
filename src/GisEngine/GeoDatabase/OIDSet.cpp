#include "stdafx.h"
#include "OIDSet.h"

namespace GisEngine
{
	namespace GeoDatabase
	{ 

		COIDSet::COIDSet()
		{

			m_it = m_setIds.end();
		}

		COIDSet::~COIDSet()
		{}


		int COIDSet::GetCount() const
		{
			return (int)m_setIds.size();
		}

		bool COIDSet::Find(int id) const
		{
			return m_setIds.find(id) != m_setIds.end();
		}

		void COIDSet::Reset()
		{
			m_it = m_setIds.begin();
		}

		bool COIDSet::Next(int* id)
		{
			if(m_it == m_setIds.end())
				return false;

			*id = *(m_it++);
			return true;
		}

		void COIDSet::Add(int id)
		{
			m_setIds.insert(id);
		}

		void COIDSet::AddList(const int* id, int count)
		{
			m_setIds.insert(id, id + count);
		}

		void COIDSet::Remove(int id)
		{
			m_setIds.erase(id);
		}

		void COIDSet::RemoveList(const int* id, int count)
		{
			while (count--)
			{
				m_setIds.erase(*id++);
			}
		}

		void COIDSet::Clear()
		{
			m_setIds.clear();
		}

	}
	
}