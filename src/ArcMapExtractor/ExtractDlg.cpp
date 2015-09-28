#include "stdafx.h"
#include "ExtractDlg.h"



CExtractDlg::CExtractDlg(const esriCore::IMxDocumentPtr& doc) : m_pDoc(doc)
{

}
CExtractDlg::~CExtractDlg()
{

}

LRESULT CExtractDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());

	m_ctlList.SubclassWindow( GetDlgItem( IDC_OUTTOOLBAR ) );
	m_ctlList.AddItem(0, 0, L"Page1");
	m_ctlList.AddItem(1, 0, L"Page2");
	m_ctlList.AddItem(2, 0, L"Page3");

	 
	m_ctlList.SelectItem(1);

	CWindow workSpase = GetDlgItem(IDC_WORK_SPACE);
	CRect rcClient;
	workSpase.GetWindowRect(rcClient);
	//workSpase.ShowWindow(SW_HIDE);
	m_mapNamePage.Create(m_hWnd, rcClient, NULL);
	//m_mapNamePage.MoveWindow(rcClient);
	//m_mapNamePage.ShowWindow(SW_SHOW);
	return TRUE;
}
LRESULT CExtractDlg::OnOk(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}
LRESULT CExtractDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

LRESULT CExtractDlg::OnListItemChanged(int wParam, LPNMHDR lParam, BOOL& bHandled)
{
	return 0;
}