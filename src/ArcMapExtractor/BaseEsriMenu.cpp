#include "stdafx.h"
#include "BaseEsriMenu.h"

CBaseEsriMenu::CBaseEsriMenu(UINT idName, UINT idCaption)
{
	wchar_t buf[1000];
	if(idName != 0)
	{
		::LoadStringW(ATL::_AtlBaseModule.GetResourceInstance(), idName, buf, 999);
		m_name = buf;
	}
	if(idCaption != 0)
	{
		::LoadStringW(ATL::_AtlBaseModule.GetResourceInstance(), idCaption, buf, 999);
		m_caption = buf;
	}
}

STDMETHODIMP CBaseEsriMenu::get_ItemCount (long * numItems )
{
	if(numItems == 0)
		return E_POINTER;
	*numItems = (long)m_vecItems.size();
	return S_OK;
}

STDMETHODIMP CBaseEsriMenu::raw_GetItemInfo (long pos, esriCore::IItemDef * itemDef)
{
	if((size_t)pos >= m_vecItems.size())
		return E_INVALIDARG;
	if(itemDef == 0)
		return E_POINTER;

	wchar_t str[40];

	StringFromGUID2(m_vecItems[pos].first, str, 40);
	itemDef->ID = str;
	itemDef->Group = (m_vecItems[pos].second) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

STDMETHODIMP CBaseEsriMenu::get_Name (BSTR * Name )
{
	if(Name == 0)
		return E_POINTER;

	*Name = m_name.copy();
	return S_OK;
}

STDMETHODIMP CBaseEsriMenu::get_Caption (BSTR * Name )
{
	if(Name == 0)
		return E_POINTER;

	*Name = m_caption.copy();
	return S_OK;
}


void CBaseEsriMenu::add_item(const GUID& guid, bool group)
{
	m_vecItems.push_back(std::make_pair(guid, group));
}