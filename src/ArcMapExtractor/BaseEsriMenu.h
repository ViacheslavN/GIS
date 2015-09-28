#pragma once

class ATL_NO_VTABLE CBaseEsriMenu : public esriCore::IMenuDef 
{
public:
	CBaseEsriMenu(UINT idName, UINT idCaption);

public:
	STDMETHOD(get_ItemCount) (long * numItems );
	STDMETHOD(raw_GetItemInfo) (long pos, esriCore::IItemDef * itemDef );
	STDMETHOD(get_Name) (BSTR * Name );
	STDMETHOD(get_Caption) (BSTR * Name );
protected:
	void add_item(const GUID& guid, bool group = false);

private:
	std::vector<std::pair<GUID, bool> > m_vecItems;
	bstr_t m_name;
	bstr_t m_caption;
};