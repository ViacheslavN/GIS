#pragma once
#include "stdafx.h"
class CListViewCtrlEx: public  ATL::CWindowImpl<CListViewCtrlEx, CListViewCtrl>, public CCustomDraw<CListViewCtrlEx>  
{
public: 
	BEGIN_MSG_MAP(CListViewCtrlEx)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd) 
		CHAIN_MSG_MAP_ALT(CCustomDraw<CListViewCtrlEx>, 1) 
		DEFAULT_REFLECTION_HANDLER() 
	END_MSG_MAP() 
	
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL  bHandled); 
	DWORD OnPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw);  
	DWORD OnPostPaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/);
	DWORD OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw);   
	DWORD OnItemPostPaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/);

	void ForceMeasureItemMessage(); void DeleteItem(LPDELETEITEMSTRUCT /*lpDeleteItemStruct*/); 
	BOOL DeleteItem(int nItem); 
	void GetCellRect(int header_column, const CRect& item_rect, CRect& cell_rect); 
}; 
	