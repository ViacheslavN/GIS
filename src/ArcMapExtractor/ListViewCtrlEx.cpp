#include "stdafx.h"
#include "ListViewCtrlEx.h"


LRESULT CListViewCtrlEx::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL  bHandled)
{ 
	HDC hdc = (HDC)wParam; 
	RECT rcClient ; 
	GetClientRect(&rcClient); 
	HBRUSH hBkBrush = CreateSolidBrush(RGB(255,255,255)); 
	FillRect(hdc, &rcClient, hBkBrush); return 1; 
} 
DWORD CListViewCtrlEx::OnPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw) 
{           return  CDRF_NOTIFYITEMDRAW; 
} 
DWORD CListViewCtrlEx::OnPostPaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/)
{
	 return  CDRF_NOTIFYITEMDRAW; 
}
DWORD CListViewCtrlEx::OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
{        
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( lpNMCustomDraw );     
	COLORREF crText = RGB(0,0,0); 
	int itemid = (pLVCD->nmcd).dwItemSpec; //this is item id  
	

	if(GetSelectedIndex() == itemid) 
	{ 
		crText = RGB(255,0,0);
		CDCHandle dc = pLVCD->nmcd.hdc;    
		CFontHandle cFont = dc.GetCurrentFont();
		RECT rcPaint = pLVCD->nmcd.rc;

		ATL::CString sText;
		GetItemText(itemid, 0, sText);

	 
		LOGFONT logFont;
		cFont.GetLogFont(logFont);
		logFont.lfWeight =  FW_BOLD; 

		HFONT font = ::CreateFontIndirect(&logFont);
		HFONT hOldfont =  dc.SelectFont(font);


		 dc.TextOutW(rcPaint.left + rcPaint.bottom - rcPaint.top + 3, rcPaint.top, sText);
		 ::DeleteObject(dc.SelectFont(hOldfont));
		return  CDRF_SKIPDEFAULT; 
	} 
	else 
	{ 
		crText = RGB(0,0,0);
	}       
	// Store the color back in the NMLVCUSTOMDRAW struct. 
	COLORREF crTextBk = RGB(255,255,255); 
	pLVCD->clrTextBk = crTextBk;
	pLVCD->clrText = crText;
	
	
	// Tell Windows to paint the control itself.       
	return CDRF_DODEFAULT;
} 
DWORD CListViewCtrlEx::OnItemPostPaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/)
{
	return CDRF_DODEFAULT;
}
void CListViewCtrlEx::ForceMeasureItemMessage()
{ // I just want to nudge it up a little.
	CRect window_rect; GetWindowRect(&window_rect); 
	CPoint pt = window_rect.TopLeft(); 
	::ScreenToClient(GetParent(), &pt); 
	window_rect.right = pt.x + window_rect.Width(); 
	window_rect.left = pt.x; 
	window_rect.bottom = pt.y + window_rect.Height(); 
	window_rect.top = pt.y + 1;
	MoveWindow(window_rect); // Alright now move it back. 
	window_rect.top = pt.y; MoveWindow(window_rect); 
}
void CListViewCtrlEx::DeleteItem(LPDELETEITEMSTRUCT /*lpDeleteItemStruct*/)
{
 
}; 
BOOL CListViewCtrlEx::DeleteItem(int nItem)
{ 
	ATLASSERT(::IsWindow(m_hWnd)); 
	return (BOOL)::SendMessage(m_hWnd, LVM_DELETEITEM, nItem, 0L);
} 
void CListViewCtrlEx::GetCellRect(int header_column, const CRect& item_rect, CRect& cell_rect)
{ 
	CHeaderCtrl header = GetHeader(); 
	CRect header_rect; 
	header.GetItemRect(header_column, &header_rect); 
	// If we don't do this, when we scroll to the right, we will be  // drawing as if we weren't and your cells won't line up with the // columns. 
	int x_offset = -GetScrollPos(SB_HORZ); 
	cell_rect.left = x_offset + header_rect.left; 
	cell_rect.right = x_offset + header_rect.right;
	cell_rect.top = item_rect.top; 
	cell_rect.bottom = item_rect.bottom;
}