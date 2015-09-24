// TestMapDrawView.h : interface of the CTestMapDrawView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "../../Display/GraphTypes.h"
#include "../../Display/GraphicsAgg.h"
#include "../../Display/DisplayTransformation2D.h"
#include "../../Cartography/Cartography.h"
#include "../../Display/RectClipper.h"
#include "../../Display/ClipRectAlloc.h"
#include "../../GisFramework/GisFramework.h"

class CMapView : public CWindowImpl<CMapView>
{
public:
	DECLARE_WND_CLASS(NULL)

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CMapView)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_MOUSEWHEEL,OnMouseWheel)
	ALT_MSG_MAP( 1 )	//	Forwarded by frame
		COMMAND_ID_HANDLER(ID_REDRAW_MAP, OnRedrawMap)
		COMMAND_ID_HANDLER(ID_FULL_ZOOM, OnFullZoom)
		COMMAND_ID_HANDLER(ID_ADD_SHAPE_FILE, OnOpenShapeFile)
		COMMAND_ID_HANDLER(ID_ADD_SQLITE_DB, OnSQLiteShapeFile)
	END_MSG_MAP()

	
	CMapView();
	~CMapView();

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnRedrawMap(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFullZoom(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);


	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseWheel(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnOpenShapeFile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSQLiteShapeFile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	void open(const wchar_t *pszFile);

	void redraw();

	void AddSQLite(const wchar_t *pszFile);
	void AddShapeFile(const wchar_t *pszFile);
	
private:
	void Update(const GisEngine::Display::GPoint* ,const GisEngine::Display::GRect*, bool bforce);
	void OnFinishMapDrawing(bool);

	void AddFeatureClass(GisEngine::GeoDatabase::IFeatureClass *pFC);

	GisEngine::Cartography::IMapPtr m_pMap;
	/*GisEngine::Display::IGraphicsPtr m_pGraphics;
	GisEngine::Display::IDisplayTransformationPtr	m_pDisplayTransformation;
	GisEngine::Display::IDisplayPtr m_pDisplay;
	GisEngine::Display::IClipPtr m_Clipper;
	GisEngine::Display::CClipRectAlloc m_ClipAlloc;*/

	GisEngine::GisFramework::IMapDrawerPtr m_pMapDrawer;


};
