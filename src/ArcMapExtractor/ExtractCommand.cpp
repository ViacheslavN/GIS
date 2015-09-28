// ExtractCommand.cpp : Implementation of CExtractCommand

#include "stdafx.h"
#include "ExtractCommand.h"
#include "ExtractDlg.h"



STDMETHODIMP CExtractCommand::raw_OnClick()
{

	if(esriCore::IMapPtr map = getMap())
	{

		CExtractDlg(m_pApp->Document).DoModal();
		return S_OK;
	}
	return E_FAIL;
}

// CExtractCommand

