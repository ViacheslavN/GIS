// ExtractMenu.h : Declaration of the CExtractMenu

#pragma once
#include "resource.h"       // main symbols


#include "CommonLibrary/guid.h"
#include "ArcMapExtractor_i.h"
#include "BaseEsriMenu.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;


// CExtractMenu

class ATL_NO_VTABLE CExtractMenu :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CExtractMenu, &CLSID_ExtractMenu>,
	public IExtractMenu,
	public CBaseEsriMenu
{
public:
	CExtractMenu() : CBaseEsriMenu(IDS_EXTRACT_CAPTION, IDS_EXTRACT_MAP_MENU)
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_EXTRACTMENU)


BEGIN_COM_MAP(CExtractMenu)
	COM_INTERFACE_ENTRY(IExtractMenu)
	COM_INTERFACE_ENTRY(esriCore::IMenuDef)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	
	void FinalRelease()
	{
	}

public:



};

OBJECT_ENTRY_AUTO(__uuidof(ExtractMenu), CExtractMenu)
