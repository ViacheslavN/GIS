// TestMapDrawView.cpp : implementation of the CTestMapDrawView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "MapView.h"
#include "../../GeoDatabase/GeoDatabase.h"
#include "../../GeoDatabase/ShapefileWorkspace.h"
#include "../../GeoDatabase/SQLiteWorkspace.h"
#include "../../../CommonLibrary/CommonLibrary/File.h"
#include "../../Cartography/FeatureLayer.h"
#include "../../Cartography/SimpleSymbolAssigner.h"
#include "../../Cartography/FeatureRenderer.h"
#include "../../Display/SimpleLineSymbol.h"
#include "../../Display/SimpleFillSymbol.h"
#include "../../Display/SimpleMarketSymbol.h"
#include "../../Display/Screen.h"
#include "../../Cartography/Map.h"
#include "../../GisFramework/MapDrawer.h"
#include "../../Display/GraphicsWinGDI.h"
#include "../../Display/GraphicsOpenGLWin.h"
#include "../../Common/GisEngineCommon.h"
#include "../../Common/XMLDoc.h"
#include "CommonLibrary/FileStream.h"
#include "../../GeoDatabase/ShapefileFeatureClass.h"
#include "../../GeoDatabase/EmbDBWorkspace.h"
class CTrackCancel : public GisEngine::GisCommon::ITrackCancel
{
public:
	virtual void Cancel(){}
	virtual bool Continue(){return true;}
};
CMapView::CMapView() : m_bLbDown(false), m_bPan(false)
{

	
	m_pMap = new GisEngine::Cartography::CMap();
	m_pMap->SetVerticalFlip(true);

	
	m_pMapDrawer = new GisEngine::GisFramework::CMapDrawer();
	//m_pMapDrawer->SetMap(m_pMap.get());
	m_pMapDrawer->SetOnFinishMapDrawing(CommonLib::Delegate(this ,&CMapView::OnFinishMapDrawing ), true);
	m_pMapDrawer->SetOnInvalidate(CommonLib::Delegate(this ,&CMapView::Update ), true);
}


CMapView::~CMapView()
{

	m_pMapDrawer->StopDraw();
}

BOOL CMapView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

LRESULT   CMapView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	/*CRect rect;
	GetClientRect(&rect);
	GisEngine::Display::GRect gRect(rect.left, rect.top, rect.right, rect.bottom);
	HDC dc = GetDC();
	double dpi = ::GetDeviceCaps(dc, LOGPIXELSX);
	ReleaseDC(dc);
	m_pDisplayTransformation = (GisEngine::Display::IDisplayTransformation*)new GisEngine::Display::CDisplayTransformation2D(dpi, m_pMap->GetMapUnits(), gRect);
	m_pDisplayTransformation->SetClipper(m_Clipper.get());
	
	m_pDisplayTransformation->SetVerticalFlip(true);
	m_pDisplay = new GisEngine::Display::CDisplay(m_pDisplayTransformation.get());*/
	return 0;
}
LRESULT CMapView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);
	GisEngine::Display::CGraphicsWinGDI graphics(dc);
	m_pMapDrawer->Update(&graphics, NULL, NULL);
	/*if(m_pGraphics.get())
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
	}*/
	

	return 0;
}
LRESULT  CMapView::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DWORD fwSizeType = wParam;     
	int nWidth = LOWORD(lParam);   
	int nHeight = HIWORD(lParam);  
	if(nWidth == 0 || nHeight == 0)
		return 0;


	m_pMapDrawer->SetSize(nWidth, nHeight);

/*	m_pGraphics = new GisEngine::Display::CGraphicsAgg(nWidth, nHeight, true);
	m_pGraphics->Erase(GisEngine::Display::Color(255, 255, 255, 255));

	if(m_pDisplayTransformation.get())
	{	
		CRect rect;
		GetClientRect(&rect);
		GisEngine::Display::GRect gRect(rect.left, rect.top, rect.right, rect.bottom);
		m_pDisplayTransformation->SetDeviceClipRect(gRect);
		m_pDisplayTransformation->SetClipRect(gRect);
		m_pDisplayTransformation->SetDeviceRect(gRect);
	}*/
	return 0;
}
LRESULT CMapView::OnMouseDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	m_LbDwnPt.x = LOWORD(lParam); 
	m_LbDwnPt.y = HIWORD(lParam);
	m_bLbDown = true;
	return 0;
}
LRESULT CMapView::OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (m_bLbDown)
		m_bPan = true;

	return 0;
}


LRESULT CMapView::OnMouseUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(!m_pMap.get())
		return 0;

	if(m_bLbDown && !m_bPan)
	{
		GisEngine::Display::GPoint pt;
		GisXYPoint mapPt;
		pt.x = LOWORD(lParam); 
		pt.y = HIWORD(lParam);
		GisEngine::Display::IDisplayTransformationPtr pDisplayTransformation = m_pMapDrawer->GetCalcTransformation();

		pDisplayTransformation->DeviceToMap(&pt, &mapPt, 1);
		CommonLib::bbox bbox;
		bbox.type = CommonLib::bbox_type_normal;

		bbox.xMin = mapPt.x;
		bbox.xMax = mapPt.x;

		bbox.yMin = mapPt.y;
		bbox.yMax = mapPt.y;


		m_pMap->SelectFeatures(bbox, true);

	}

	if (m_bPan)
	{

 

		int x = m_LbDwnPt.x - LOWORD(lParam);
		int y = m_LbDwnPt.y - HIWORD(lParam);

		GisEngine::Display::IDisplayTransformationPtr pDisplayTransformation = m_pMapDrawer->GetCalcTransformation();

		double dMapX = pDisplayTransformation->DeviceToMapMeasure(x);
		double dMapY = pDisplayTransformation->DeviceToMapMeasure(y);

		CommonLib::GisXYPoint mapPos = pDisplayTransformation->GetMapPos();

		mapPos.x += dMapX;
		mapPos.y -= dMapY;

		pDisplayTransformation->SetMapPos(mapPos, pDisplayTransformation->GetScale());
		redraw();


		m_bPan = false;
	}
	
	
	m_bLbDown = false;
	return 0;
}
LRESULT CMapView::OnMouseWheel(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	GisEngine::Display::IDisplayTransformationPtr pDisplayTransformation = m_pMapDrawer->GetCalcTransformation();
	if(!pDisplayTransformation.get())
		return 0;
	double zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
	double mouseWheel = 0.4;

	GisEngine::Display::GRect devRect = pDisplayTransformation->GetDeviceRect();
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	GisEngine::Display::GPoint gpt((GisEngine::Display::GUnits)pt.x, (GisEngine::Display::GUnits)pt.y);
	if(!devRect.pointInRect(gpt))
		return 0;

	CommonLib::GisXYPoint location;
	pDisplayTransformation->DeviceToMap(&gpt, &location, 1);
	double mult = 0;
	if(zDelta < 0)
		mult = 1 - mouseWheel / 2.;
	else
		mult = 1. + mouseWheel / 2.;
	double newScale = pDisplayTransformation->GetScale() * mult;

	double scale = pDisplayTransformation->GetScale();
	GisEngine::GisBoundingBox curBox = pDisplayTransformation->GetFittedBounds();
	GisEngine::GisBoundingBox bigBox = m_pMap->GetFullExtent()->GetBoundingBox();


	double fullScaleX = scale / (curBox.xMax - curBox.xMin) * (bigBox.xMax - bigBox.xMin);
	double fullScaleY = scale / (curBox.yMax - curBox.yMin) * (bigBox.yMax - bigBox.yMin);
	double fullScale = fullScaleX > fullScaleY ? fullScaleX : fullScaleY;

	if(pDisplayTransformation->GetUnits() == GisEngine::GisCommon::UnitsUnknown)
	{
		//if(newScale < 1.)
		//  newScale = 1.;
	}
	else
	{
		if(newScale < 100.)
			newScale = 100.;
	}

	if(newScale > 7 * fullScale)
		newScale = 7 * fullScale;
	if((scale == 100. && newScale == 100.) || (scale == fullScale && newScale == fullScale))
		return 0;
	mult = newScale / pDisplayTransformation->GetScale();

	CommonLib::GisXYPoint mapPos = pDisplayTransformation->GetMapPos();
	CommonLib::GisXYPoint newPos;
	newPos.x = location.x + (mapPos.x - location.x) * mult;
	newPos.y = location.y + (mapPos.y - location.y) * mult;

	pDisplayTransformation->SetMapPos(newPos, newScale);
	redraw();
	return 0;
}

void CMapView::redraw()
{
	/*if(!m_pMap.get() || !m_pDisplay.get() || !m_pDisplayTransformation.get())
		return;

	m_pDisplay->StartDrawing(m_pGraphics.get());
	CTrackCancel tc;
	m_pDisplay->GetGraphics()->Erase(GisEngine::Display::Color(255, 255, 255, 255));
	m_pMap->Draw(m_pDisplay.get(), &tc);
	m_pDisplay->FinishDrawing();

	::InvalidateRect(m_hWnd, 0, FALSE);*/

	m_pMapDrawer->Redraw();
}
LRESULT CMapView::OnRedrawMap(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	
	redraw();
	//m_pMapDrawer->Redraw();
	return 0;
}
LRESULT CMapView::OnFullZoom(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	//m_pDisplayTransformation->SetMapVisibleRect(m_pMap->GetFullExtent()->GetBoundingBox());
	m_pMapDrawer->GetCalcTransformation()->SetMapVisibleRect(m_pMap->GetFullExtent()->GetBoundingBox());
	redraw();
	return 0;
}

void CMapView::Update(const GisEngine::Display::GPoint*pPt , const GisEngine::Display::GRect* pRect, bool bforce)
{
	if(!bforce)
	{
		Invalidate(FALSE);
	}
	else 
	{

		HDC hDC = GetDC();
		GisEngine::Display::CGraphicsWinGDI graphics(hDC);


		CRect rect;
		GetClientRect(&rect);


		GisEngine::Display::GRect gRect;
		GisEngine::Display::GPoint Pt(0, 0);
		CRect client;
		GetClientRect(client);
		if(pRect)
		{
			gRect.set(pRect->xMin, pRect->yMin, pRect->xMax, pRect->yMax);
		}
		else 
		{
			gRect.set((GisEngine::Display::GUnits)client.left, (GisEngine::Display::GUnits)client.top, (GisEngine::Display::GUnits)client.right, (GisEngine::Display::GUnits)client.bottom);
		}
		if(pPt)
		{
			Pt.x = pPt->x;
			Pt.y = pPt->y;
		}

		 GisEngine::Display::GPoint pt(gRect.xMin, gRect.yMin);
		m_pMapDrawer->Update(&graphics ,&pt, &gRect);
		

		ReleaseDC(hDC);

	}
}
void CMapView::OnFinishMapDrawing(bool)
{

}


void CMapView::AddSQLite(const wchar_t *pszFile)
{
	CommonLib::CString sPath = CommonLib::FileSystem::FindFilePath(pszFile);
	CommonLib::CString sFileName = CommonLib::FileSystem::FindFileName(pszFile);

	
	GisEngine::GeoDatabase::IWorkspacePtr pWks = GisEngine::GeoDatabase::CSQLiteWorkspace::Open(sFileName.cwstr(), sPath.cwstr(), true, true);

	if(!pWks.get())
		return;

	for (size_t i = 0, sz =pWks->GetDatasetCount(); i < sz; ++i)
	{
		GisEngine::GeoDatabase::IDatasetPtr pDataset = pWks->GetDataset(i);
		if(pDataset->GetDatasetType() ==  GisEngine::GeoDatabase::dtFeatureClass)
		{
			AddFeatureClass((GisEngine::GeoDatabase::IFeatureClass*)pDataset.get());
		}
	}

}
void CMapView::AddShapeFile(const wchar_t *pszFile)
{
	CommonLib::CString sPath = CommonLib::FileSystem::FindFilePath(pszFile);
	//	CommonLib::CString sFileName = CommonLib::FileSystem::FindOnlyFileName(pszFile);
	CommonLib::CString sFileName = CommonLib::FileSystem::FindFileName(pszFile);

	GisEngine::GeoDatabase::IWorkspacePtr pWks = GisEngine::GeoDatabase::CShapefileWorkspace::Open(L"", sPath.cwstr());
	
	if(!pWks.get())
		return;

	GisEngine::GeoDatabase::IFeatureClassPtr pFC = pWks->GetFeatureClass(sFileName);
	if(!pFC.get())
	{
		 pFC = pWks->OpenFeatureClass(sFileName);
	}


	if(!pFC.get())
		return;

	GisEngine::GeoDatabase::CShapefileFeatureClass *pShapeFC = (GisEngine::GeoDatabase::CShapefileFeatureClass*)pFC.get();
	if(!pShapeFC->GetShapeIndex().get())
	{
		int nRet = ::MessageBox(m_hWnd, L"File not found with a spatial index. Create it?", L"Info", MB_YESNO);
		if(nRet == IDYES)
		{
			pShapeFC->CreateShapeIndex();
		}
	}

	AddFeatureClass(pFC.get());
}
void CMapView::AddEmbDB(const wchar_t *pszFile)
{
	CommonLib::CString sPath = CommonLib::FileSystem::FindFilePath(pszFile);
	CommonLib::CString sFileName = CommonLib::FileSystem::FindFileName(pszFile);


	GisEngine::GeoDatabase::IWorkspacePtr pWks = GisEngine::GeoDatabase::CEmbDBWorkspace::Open(sFileName.cwstr(), sPath.cwstr(), true, true);

	if(!pWks.get())
		return;

	for (size_t i = 0, sz =pWks->GetDatasetCount(); i < sz; ++i)
	{
		GisEngine::GeoDatabase::IDatasetPtr pDataset = pWks->GetDataset(i);
		if(pDataset->GetDatasetType() ==  GisEngine::GeoDatabase::dtFeatureClass)
		{
			AddFeatureClass((GisEngine::GeoDatabase::IFeatureClass*)pDataset.get());
		}
	}

}
void CMapView::AddFeatureClass(GisEngine::GeoDatabase::IFeatureClass *pFC)
{
	if(!pFC)
		return;

	GisEngine::Cartography::IFeatureLayerPtr pFeatureLayer (new GisEngine::Cartography::CFeatureLayer());
	CommonLib::eShapeType type = pFC->GetGeometryType();


	GisEngine::Cartography::CSimpleSymbolAssigner* pSimpleSymbolAssigner = new GisEngine::Cartography::CSimpleSymbolAssigner();
	GisEngine::Display::ISymbolPtr pSymbol;
	GisEngine::GisGeometry::ISpatialReferencePtr pSpRef = pFC->GetSpatialReference();

	switch(type)
	{
	case CommonLib::shape_type_point:
	case CommonLib::shape_type_multipoint:
		{
			GisEngine::Display::CSimpleMarketSymbol* pMarketSymbol = new GisEngine::Display::CSimpleMarketSymbol();
			pMarketSymbol->SetOutline(true);
			pMarketSymbol->SetColor(GisEngine::Display::Color(255, 0, 0, 255));
			pMarketSymbol->SetOutlineColor(GisEngine::Display::Color(123,123, 123, 255));
			pMarketSymbol->SetOutlineSize(0.1);
			pMarketSymbol->SetSize(1.0);
			pSymbol = pMarketSymbol;
		}
		
		break;
	case CommonLib::shape_type_polyline:
		pSymbol = new GisEngine::Display::CSimpleLineSymbol(GisEngine::Display::Color(0, 255, 255, 255), 0.5);
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
	pFeatureLayer->SetFeatureClass(pFC);
	pFeatureLayer->SetVisible(true);


	m_pMapDrawer->StopDraw();
	m_pMap->GetLayers()->AddLayer(pFeatureLayer.get());

	if(m_pMap->GetLayers()->GetLayerCount() == 1)
	{
		m_pMap->SetSpatialReference(pFC->GetSpatialReference().get());
		//m_pDisplayTransformation->SetSpatialReference(pFC->GetSpatialReference().get());
		//m_pDisplayTransformation->SetUnits(m_pMap->GetMapUnits());
		m_pMapDrawer->SetMap(m_pMap.get());

	}
	//m_pDisplayTransformation->SetMapVisibleRect(m_pMap->GetFullExtent()->GetBoundingBox());
	m_pMapDrawer->GetCalcTransformation()->SetMapVisibleRect(m_pMap->GetFullExtent()->GetBoundingBox());
	m_pMapDrawer->Redraw();
}


LRESULT CMapView::OnOpenShapeFile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CFileDialog fileDlg(TRUE, _T("shp"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("ESRI shape Files (*.shp)\0*.shp\0All Files (*.*)\0*.*\0"), m_hWnd);
	if ( fileDlg.DoModal() != IDOK )
		return 0;
	AddShapeFile(fileDlg.m_szFileName);

	return 1;
}
LRESULT CMapView::OnSQLiteShapeFile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CFileDialog fileDlg(TRUE, _T("sqlite"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Sqlite DB (*.sqlite)\0*.sqlite\0All Files (*.*)\0*.*\0"), m_hWnd);
	if ( fileDlg.DoModal() != IDOK )
		return 0;
	AddSQLite(fileDlg.m_szFileName);

	return 1;
}

LRESULT CMapView::OnEmbDBShapeFile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CFileDialog fileDlg(TRUE, _T("embDB"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("EmbDB (*.embdb)\0*.embdb\0All Files (*.*)\0*.*\0"), m_hWnd);
	if ( fileDlg.DoModal() != IDOK )
		return 0;
	AddEmbDB(fileDlg.m_szFileName);

	return 1;
}


void CMapView::open(const wchar_t *pszFile)
{
	m_pMapDrawer->StopDraw();


	CommonLib::CString sExt = CommonLib::FileSystem::FindFileExtension(pszFile);
	if(sExt == L"xml")
	{
		GisEngine::GisCommon::CXMLDoc xmlDoc;
		if(xmlDoc.Open(pszFile))
		{
			GisEngine::GisCommon::IXMLNodePtr pNodes = xmlDoc.GetNodes();
			GisEngine::GisCommon::IXMLNodePtr pMapNode = pNodes->GetChild(L"Map");
			if(pMapNode.get())
			{

				GisEngine::Cartography::IMapPtr pMap(new GisEngine::Cartography::CMap());
				pMap->SetVerticalFlip(true);
				if(((GisEngine::GisCommon::IXMLSerialize*)pMap.get())->load(pMapNode.get()))
				{
					m_pMap = pMap;
					m_pMapDrawer->SetMap(pMap.get());
				}
			}
		}
	}
	else
	{
		CommonLib::CReadFileStream fStream;
		if(fStream.open(pszFile, CommonLib::ofmOpenExisting, CommonLib::arRead, CommonLib::smNoMode))
		{
			GisEngine::Cartography::IMapPtr pMap(new GisEngine::Cartography::CMap());
			pMap->SetVerticalFlip(true);
			if(((GisEngine::GisCommon::IStreamSerialize*)pMap.get())->load(&fStream))
			{
				m_pMap = pMap;
				m_pMapDrawer->SetMap(pMap.get());
			}
		}
	}

	redraw();

}

void CMapView::save(const wchar_t *pszFile)
{
	if(!m_pMap.get())
		return;

	CommonLib::CString sExt = CommonLib::FileSystem::FindFileExtension(pszFile);
	if(sExt == L"xml")
	{
		GisEngine::GisCommon::CXMLDoc xmlDoc;
		GisEngine::GisCommon::IXMLNodePtr pNodes = xmlDoc.GetNodes();
		if(m_pMap->saveXML(pNodes.get()))
		{
			xmlDoc.Save(pszFile);
		}
	}
	else
	{
		CommonLib::CWriteFileStream fStream;
		if(fStream.open(pszFile, CommonLib::ofmCreateAlways, CommonLib::arWrite, CommonLib::smNoMode))
		{
			m_pMap->save(&fStream);
		}
	}
}