// MapToolBar.cpp : Implementation of CMapToolBar

#include "stdafx.h"
#include "MapToolBar.h"
#include "ExtractCommand.h"

HRESULT CMapToolBar::FinalConstruct()
{
	m_vecItems.push_back(std::make_pair(CommonLib::CGuid(CLSID_ExtractCommand), false));
	return S_OK;
}

STDMETHODIMP CMapToolBar::get_ItemCount(long * numItems )
{
	COM_CHECK_PTR(numItems);
	*numItems = 1;
	return S_OK;
}

STDMETHODIMP CMapToolBar::raw_GetItemInfo(long pos, esriCore::IItemDef * itemDef )
{
	COM_CHECK_PTR(itemDef);
	COM_CHECK_PTR(itemDef);
	if(pos >= (int)m_vecItems.size())
		return E_INVALIDARG;

	itemDef->Group = m_vecItems[pos].second ? VARIANT_TRUE : VARIANT_FALSE;
	itemDef->ID = m_vecItems[pos].first.toString().bstr();

	
	return S_OK;
}

STDMETHODIMP CMapToolBar::get_Name(BSTR * Name )
{
	COM_CHECK_PTR(Name);
	*Name = ::SysAllocString(L"Extract Tools");
	return S_OK;
}

STDMETHODIMP CMapToolBar::get_Caption(BSTR * Name )
{
	COM_CHECK_PTR(Name);
	*Name = ::SysAllocString(L"Extract Tools");
	return S_OK;
}
// CMapToolBar

