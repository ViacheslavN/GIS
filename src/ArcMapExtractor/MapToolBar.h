// MapToolBar.h : Declaration of the CMapToolBar

#pragma once
#include "resource.h"       // main symbols

#include "CommonLibrary/guid.h"

#include "ArcMapExtractor_i.h"
#include "ExtractMenu.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;


// CMapToolBar

class ATL_NO_VTABLE CMapToolBar :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMapToolBar, &CLSID_MapToolBar>,
	public IMapToolBar,
	public esriCore::IToolBarDef
{
public:
	CMapToolBar()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_MAPTOOLBAR)


BEGIN_COM_MAP(CMapToolBar)
	COM_INTERFACE_ENTRY(IMapToolBar)
	COM_INTERFACE_ENTRY(esriCore::IToolBarDef)
END_COM_MAP()


BEGIN_CATEGORY_MAP(CMapToolBar)
	IMPLEMENTED_CATEGORY(__uuidof(CATID_MxCommandBars))
END_CATEGORY_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	

	void FinalRelease()
	{
	}

	// IToolBarDef
	STDMETHOD(get_ItemCount)(long * numItems );
	STDMETHOD(raw_GetItemInfo)(long pos, esriCore::IItemDef * itemDef );
	STDMETHOD(get_Name)(BSTR * Name );
	STDMETHOD(get_Caption)(BSTR * Name );

public:
private:
	 std::vector<std::pair<CommonLib::CGuid, bool> > m_vecItems;


};

OBJECT_ENTRY_AUTO(__uuidof(MapToolBar), CMapToolBar)
