#include "stdafx.h"
#include "PropertySet.h"

namespace GisEngine
{
	namespace GisCommon
	{
		CPropertySet::CPropertySet()
		{
			
		}
		CPropertySet::~CPropertySet()
		{
			clear();
		}
		int  CPropertySet::count() const
		{
			return m_mapProp.size();
		}
		bool  CPropertySet::PropertyExists(const wchar_t *name) const
		{
			return m_mapProp.find(name) != m_mapProp.end();
		}
		const CommonLib::CVariant* CPropertySet::GetProperty(const wchar_t *name) const
		{
			TMapProp::const_iterator c_it =  m_mapProp.find(name);
			return c_it != m_mapProp.end() ? &c_it->second : nullptr;
		}
		void  CPropertySet::SetProperty(const wchar_t *name, const CommonLib::CVariant& variant)
		{
			m_mapProp[name] = variant;
			
		}
		void  CPropertySet::RemoveProperty(const wchar_t *name)
		{
			TMapProp::iterator it =  m_mapProp.find(name);
			if(it != m_mapProp.end())
			{
				m_mapProp.erase(it);
			}
		}
		void CPropertySet::clear()
		{
			m_mapProp.clear();
		}
		void  CPropertySet::RemoveAllProperties()
		{
			clear();
		}
	}
}