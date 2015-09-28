#pragma once
#include "ListViewCtrlEx.h"
#include "MapNamePage.h"
class CExtractDlg : public ATL::CDialogImpl<CExtractDlg>
{
public:
	enum { IDD = IDD_EXTRACT_DIALOG };


	BEGIN_MSG_MAP(CExtractDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOk)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)

		NOTIFY_CODE_HANDLER(LVN_ITEMCHANGED, OnListItemChanged)
		REFLECT_NOTIFICATIONS()
	//	CHAIN_MSG_MAP_ALT_MEMBER(m_ctlList, 1)
	END_MSG_MAP()

	CExtractDlg(const esriCore::IMxDocumentPtr& doc);
	~CExtractDlg();

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOk(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnListItemChanged(int wParam, LPNMHDR lParam, BOOL& bHandled);
private:
	CMapNamePage m_mapNamePage;
	 CListViewCtrlEx m_ctlList;
	  esriCore::IMxDocumentPtr m_pDoc;
};