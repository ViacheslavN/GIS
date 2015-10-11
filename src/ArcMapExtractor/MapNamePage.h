#pragma once

class CMapNamePage  : public ATL::CDialogImpl<CMapNamePage>
{
public:
	enum { IDD = IDD_MAP_NAME_PAGE };


	BEGIN_MSG_MAP(CMapNamePage)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDC_BUTTON_NEXT, OnNext)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	CMapNamePage();
	~CMapNamePage();

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnNext(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
 
};