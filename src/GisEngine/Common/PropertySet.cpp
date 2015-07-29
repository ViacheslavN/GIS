#include "stdafx.h"
#include "PropertySet.h"

namespace GisEngine
{
	namespace Common
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
		CommonLib::IVariant* CPropertySet::GetProperty(const wchar_t *name) const
		{
			TMapProp::const_iterator c_it =  m_mapProp.find(name);
			return c_it != m_mapProp.end() ? c_it->second : nullptr;
		}
		void  CPropertySet::SetProperty(const wchar_t *name, CommonLib::IVariant* pValue)
		{
			TMapProp::iterator it =  m_mapProp.find(name);
			if(it == m_mapProp.end())
				m_mapProp.insert(std::make_pair(CommonLib::str_t(name), pValue->clone()));
			else
			{
				delete it->second;
				it->second = pValue->clone();
			}
		}
		void  CPropertySet::RemoveProperty(const wchar_t *name)
		{
			TMapProp::iterator it =  m_mapProp.find(name);
			if(it != m_mapProp.end())
			{
				delete it->second;
				m_mapProp.erase(it);
			}
		}
		void CPropertySet::clear()
		{
			TMapProp::iterator it =  m_mapProp.begin();
			TMapProp::iterator end = m_mapProp.end();
			for (; it != end; ++it)
			{
				delete it->second;
			}
			m_mapProp.clear();
		}
		void  CPropertySet::RemoveAllProperties()
		{
			clear();
		}
	}
}