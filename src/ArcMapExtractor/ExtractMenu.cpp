// ExtractMenu.cpp : Implementation of CExtractMenu

#include "stdafx.h"
#include "ExtractMenu.h"
#include "ExtractCommand.h"

HRESULT CExtractMenu::FinalConstruct()
{
	add_item(CommonLib::CGuid(CLSID_ExtractCommand));
	return S_OK;
}
// CExtractMenu

