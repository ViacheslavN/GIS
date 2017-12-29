// TestGraphicsView.h : interface of the CTestGraphicsView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "../../Display/GraphTypes.h"
#include "../../Display/GraphicsAgg.h"
#include "../../Display/GraphicsOpenGLWin.h"
#include "../../Display/GraphicsWinGDI.h"
#include <vector>
class CTestGraphicsView : public CWindowImpl<CTestGraphicsView>
{
public:
	DECLARE_WND_CLASS(NULL)

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CTestGraphicsView)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
	ALT_MSG_MAP( 1 )	//	Forwarded by frame
		COMMAND_ID_HANDLER(ID_3DTEST_WIRERENDER, OnWireRender)
		COMMAND_ID_HANDLER(ID_TRIANGLE_RENDER,  OnTriangleRender)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnWireRender(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnTriangleRender(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//void OnWireRender();
private:
	std::auto_ptr<GisEngine::Display::CGraphicsAgg> m_pGraphicsAgg;
	std::auto_ptr<GisEngine::Display::CGraphicsOpenGLWin> m_pGraphicsOpenGLWin;
};
