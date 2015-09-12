// ExtractCommand.cpp : Implementation of CExtractCommand

#include "stdafx.h"
#include "ExtractCommand.h"




STDMETHODIMP CExtractCommand::raw_OnClick()
{

	if(esriCore::IMapPtr map = getMap())
	{

		return S_OK;
	}
	return E_FAIL;
}

// CExtractCommand

