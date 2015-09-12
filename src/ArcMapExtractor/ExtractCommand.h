// ExtractCommand.h : Declaration of the CExtractCommand

#pragma once
#include "resource.h"       // main symbols



#include "ArcMapExtractor_i.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;


// CExtractCommand

class ATL_NO_VTABLE CExtractCommand :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CExtractCommand, &CLSID_ExtractCommand>,
	public IExtractCommand,
	public esriCore::IESRICommand
{
public:
	CExtractCommand()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_EXTRACTCOMMAND)


BEGIN_COM_MAP(CExtractCommand)
	COM_INTERFACE_ENTRY(IExtractCommand)
	COM_INTERFACE_ENTRY(esriCore::IESRICommand)
END_COM_MAP()


BEGIN_CATEGORY_MAP(CExtractCommand)
	IMPLEMENTED_CATEGORY(__uuidof(CATID_MxCommands))
END_CATEGORY_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		 m_hBitmap = (HBITMAP)::LoadImage(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDB_EXTRACT_CMD), IMAGE_BITMAP, 16, 16, LR_DEFAULTCOLOR);
		return S_OK;
	}

	void FinalRelease()
	{
		::DeleteObject(m_hBitmap);
	}

	STDMETHOD(get_Enabled)(VARIANT_BOOL * Enabled)
	{
		COM_CHECK_PTR(Enabled);

		if(getMap())
			*Enabled = VARIANT_TRUE;
		else
			*Enabled = VARIANT_FALSE;

		return S_OK;
	}
	STDMETHOD(get_Checked)(VARIANT_BOOL * Checked)
	{
		COM_CHECK_PTR(Checked);

		*Checked = VARIANT_TRUE;
		return S_OK;
	}
	STDMETHOD(get_Name)(BSTR * Name)
	{
		COM_CHECK_PTR(Name);
		*Name = ::SysAllocString(L"Extract Map");
		return S_OK;
	}
	STDMETHOD(get_Caption)(BSTR * Caption)
	{
		COM_CHECK_PTR(Caption);
		*Caption =::SysAllocString(L"Extract Map");
		return S_OK;
	}
	STDMETHOD(get_Tooltip)(BSTR * Tooltip)
	{
		COM_CHECK_PTR(Tooltip);
		*Tooltip = ::SysAllocString(L"Extract Map");
		return S_OK;
	}
	STDMETHOD(get_Message)(BSTR * Message)
	{
		COM_CHECK_PTR(Message);
		*Message =  ::SysAllocString(L"Get Message");
		return S_OK;
	}
	STDMETHOD(get_HelpFile)(BSTR * HelpFile)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(get_HelpContextID)(long * helpID)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(get_Bitmap)(esriSystem::OLE_HANDLE * Bitmap)
	{
		*Bitmap = (esriSystem::OLE_HANDLE)m_hBitmap;
		return S_OK;
	}
	STDMETHOD(get_Category)(BSTR * categoryName)
	{
		COM_CHECK_PTR(categoryName);
		*categoryName =  ::SysAllocString(L"Extract Map");
		return S_OK;
	}
	STDMETHOD(raw_OnCreate)(LPDISPATCH hook)
	{
		m_pApp = hook;
		return S_OK;
	}
	STDMETHOD(raw_OnClick)();

public:
private:
	esriCore::IMapPtr			getMap()
	{
		if(m_pApp)
			if(esriCore::IMxDocumentPtr pDoc = m_pApp->Document)
				if(esriCore::IMapPtr pMap = pDoc->FocusMap)
					if(pMap->LayerCount > 0)
						return pMap;
		return esriCore::IMapPtr();
	}

	esriCore::IApplicationPtr m_pApp;
	HBITMAP m_hBitmap;


};

OBJECT_ENTRY_AUTO(__uuidof(ExtractCommand), CExtractCommand)
