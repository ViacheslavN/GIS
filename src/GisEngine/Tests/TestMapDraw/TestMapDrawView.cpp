// TestMapDrawView.cpp : implementation of the CTestMapDrawView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "TestMapDrawView.h"
#include "../../GeoDatabase/GeoDatabase.h"
#include "../../GeoDatabase/ShapefileWorkspace.h"
#include "../../../CommonLibrary/CommonLibrary/File.h"
#include "../../Cartography/FeatureLayer.h"
#include "../../Cartography/SimpleSymbolAssigner.h"
#include "../../Cartography/FeatureRenderer.h"
#include "../../Display/SimpleLineSymbol.h"
#include "../../Display/SimpleFillSymbol.h"
#include "../../Display/Screen.h"
#include "../../Cartography/Map.h"


class CTrackCancel : public GisEngine::GisCommon::ITrackCancel
{
public:
	virtual void Cancel(){}
	virtual bool Continue(){return true;}
};
CTestMapDrawView::CTestMapDrawView()
{

	m_Clipper = new GisEngine::Display::CRectClipper(&m_ClipAlloc);
	m_pMap = new GisEngine::Cartography::CMap();
}


CTestMapDrawView::~CTestMapDrawView()
{}

BOOL CTestMapDrawView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

LRESULT CTestMapDrawView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);

	if(m_pGraphics.get())
	{		
	BITMAPINFO bmi;
	::memset(&bmi, 0, sizeof(bmi));
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biHeight = (int)m_pGraphics->GetSurface().height();
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = m_pGraphics->GetSurface().width();
	::StretchDIBits(
		dc.m_hDC,           
		0, 0, m_pGraphics->GetWidth(), m_pGraphics->GetHeight(),
		0, 0,  m_pGraphics->GetWidth(), m_pGraphics->GetHeight(),
		m_pGraphics->GetSurface().bits(),
		&bmi,  
		DIB_RGB_COLORS, SRCCOPY) ;
	}
	

	return 0;
}
LRESULT  CTestMapDrawView::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DWORD fwSizeType = wParam;     
	int nWidth = LOWORD(lParam);   
	int nHeight = HIWORD(lParam);  
	if(nWidth == 0 || nHeight == 0)
		return 0;
	m_pGraphics = new GisEngine::Display::CGraphicsAgg(nWidth, nHeight, true);
	m_pGraphics->Erase(GisEngine::Display::Color(255, 255, 255, 255));
	return 0;
}

LRESULT CTestMapDrawView::OnRedrawMap(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(!m_pMap.get() || !m_pDisplay.get() || !m_pDisplayTransformation.get())
		return 0;

	m_pDisplay->StartDrawing(m_pGraphics.get());
	CTrackCancel tc;

	m_pDisplay->GetGraphics()->Erase(GisEngine::Display::Color(255, 255, 255, 255));
	m_pMap->Draw(m_pDisplay.get(), &tc);
	m_pDisplay->FinishDrawing();

	::InvalidateRect(m_hWnd, 0, FALSE);

	return 0;
}

void CTestMapDrawView::open(const wchar_t *pszFile)
{
	CommonLib::str_t sPath = CommonLib::FileSystem::FindFilePath(pszFile);
	CommonLib::str_t sFileName = CommonLib::FileSystem::FindOnlyFileName(pszFile);

	GisEngine::GeoDatabase::IWorkspacePtr pWks = GisEngine::GeoDatabase::CShapefileWorkspace::Open(L"", sPath.cwstr());
	if(!pWks.get())
		return;

	GisEngine::GeoDatabase::IFeatureClassPtr pFC = pWks->GetFeatureClass(sFileName);
	if(!pFC.get())
		return;
	GisEngine::Cartography::IFeatureLayerPtr pFeatureLayer (new GisEngine::Cartography::CFeatureLayer());
	CommonLib::eShapeType type = pFC->GetGeometryType();


	GisEngine::Cartography::CSimpleSymbolAssigner* pSimpleSymbolAssigner = new GisEngine::Cartography::CSimpleSymbolAssigner();
	GisEngine::Display::ISymbolPtr pSymbol;
	GisEngine::GisGeometry::ISpatialReferencePtr pSpRef = pFC->GetSpatialReference();

	switch(type)
	{
		case CommonLib::shape_type_polyline:
			pSymbol = new GisEngine::Display::CSimpleLineSymbol(GisEngine::Display::Color(0, 255, 0, 255), 0.5);
			break;
		case CommonLib::shape_type_polygon:
		case CommonLib::shape_type_multipatch:
			{
				GisEngine::Display::CSimpleFillSymbol* pFillSymbol = new GisEngine::Display::CSimpleFillSymbol();
				GisEngine::Display::CSimpleLineSymbol* pLineSymbol =new GisEngine::Display::CSimpleLineSymbol(GisEngine::Display::Color(0, 255, 0, 255), 0.5);
				pLineSymbol->SetScaleDependent(true);

				pFillSymbol->SetColor(GisEngine::Display::Color(255, 0, 0, 255));
				pFillSymbol->SetBackgroundColor(GisEngine::Display::Color(255, 0, 0, 255));
				pFillSymbol->SetOutlineSymbol(pLineSymbol);
				pSymbol = pFillSymbol;
			}
			break;
	}

	pSimpleSymbolAssigner->SetSymbol(pSymbol.get());
	GisEngine::Cartography::IFeatureRendererPtr pRenderer( (GisEngine::Cartography::IFeatureRenderer*)(new GisEngine::Cartography::CFeatureRenderer()));

	pRenderer->SetSymbolAssigner(pSimpleSymbolAssigner);
	pFeatureLayer->AddRenderer(pRenderer.get());
	pFeatureLayer->SetFeatureClass(pFC.get());
	pFeatureLayer->SetVisible(true);

	m_pMap->GetLayers()->AddLayer(pFeatureLayer.get());

	HDC dc = GetDC();
	double dpi = ::GetDeviceCaps(dc, LOGPIXELSX);
	ReleaseDC(dc);
	CRect rect;
	GetClientRect(&rect);
	 GisEngine::Display::GRect gRect(rect.left, rect.top, rect.right, rect.bottom);

	m_pDisplayTransformation = (GisEngine::Display::IDisplayTransformation*)new GisEngine::Display::CDisplayTransformation2D(dpi, pSpRef->GetUnits(), gRect);
	m_pDisplayTransformation->SetClipper(m_Clipper.get());
	m_pDisplayTransformation->SetDeviceClipRect(gRect);
	m_pDisplayTransformation->SetMapVisibleRect(m_pMap->GetFullExtent()->GetBoundingBox());
	m_pDisplayTransformation->SetVerticalFlip(true);

	m_pDisplay = new GisEngine::Display::CDisplay(m_pDisplayTransformation.get());
	m_pDisplay->StartDrawing(m_pGraphics.get());
	CTrackCancel tc;

	m_pDisplay->GetGraphics()->Erase(GisEngine::Display::Color(255, 255, 255, 255));
	m_pMap->Draw(m_pDisplay.get(), &tc);
	//m_pFeatureLayer->Draw(GisEngine::Cartography::DrawPhaseGeography, m_pDisplay.get(), &tc);

	

	m_pDisplay->FinishDrawing();

	 ::InvalidateRect(m_hWnd, 0, FALSE);

}