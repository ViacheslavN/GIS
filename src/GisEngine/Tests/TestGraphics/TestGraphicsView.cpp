// TestGraphicsView.cpp : implementation of the CTestGraphicsView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "TestGraphicsView.h"
#include "Model.h"
#include "3DDrawFunc.h"

class CTrackCancel : public GisEngine::GisCommon::ITrackCancel
{
public:
	virtual void Cancel(){}
	virtual bool Continue(){return true;}
};
BOOL CTestGraphicsView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

LRESULT CTestGraphicsView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);
	/*GisEngine::Display::CPen pen;
	pen.setWidth(10);
	pen.setColor(GisEngine::Display::Color(0, 255, 0, 255));
	pen.setCapType(GisEngine::Display::CapTypeRound);
	pen.setJoinType(GisEngine::Display::JoinTypeRound);
	pen.setPenType(GisEngine::Display::PenTypeDashDotDot);*/
	if(m_pGraphicsAgg.get())
	{
		//m_pGraphicsAgg->Erase(GisEngine::Display::Color(255, 0, 0, 255));
		//m_pGraphicsAgg->DrawLineSeg(&pen, 0, 0, 200, 200);



		 ::BitBlt(dc.m_hDC, 0, 0, m_pGraphicsAgg->GetWidth(), m_pGraphicsAgg->GetHeight(), m_pGraphicsAgg->GetDC(), 0, 0, SRCCOPY);


		/*BITMAPINFO bmi;
		::memset(&bmi, 0, sizeof(bmi));
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biHeight = (int)m_pGraphicsAgg->GetSurface().height();
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = m_pGraphicsAgg->GetSurface().width();
		::StretchDIBits(
			dc.m_hDC,           
			0, 0, m_pGraphicsAgg->GetWidth(), m_pGraphicsAgg->GetHeight(),
			0, 0,  m_pGraphicsAgg->GetWidth(), m_pGraphicsAgg->GetHeight(),
			m_pGraphicsAgg->GetSurface().bits(),
			&bmi,  
			DIB_RGB_COLORS, SRCCOPY) ;*/
	}
	

	//TODO: Add your drawing code here

	return 0;
}
LRESULT  CTestGraphicsView::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DWORD fwSizeType = wParam;     
	int nWidth = LOWORD(lParam);   
	int nHeight = HIWORD(lParam);  
	if(nWidth == 0 || nHeight == 0)
		return 0;

	m_pGraphicsAgg.reset(new GisEngine::Display::CGraphicsAgg(nWidth, nHeight, true));
	return 0;
}

LRESULT  CTestGraphicsView::OnWireRender(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{


	if(!m_pGraphicsAgg.get())
		return 0;

	Model model("d:\\work\\MyProject\\obj\\african_head\\african_head.obj");


	m_pGraphicsAgg->Erase(GisEngine::Display::Color(0, 0, 0, 255));
	int width = m_pGraphicsAgg->GetWidth();
	int height = m_pGraphicsAgg->GetHeight();

	GisEngine::Display::CPen pen;
	pen.setWidth(1);
	pen.setColor(GisEngine::Display::Color(255, 255, 255, 255));
	pen.setCapType(GisEngine::Display::CapTypeRound);
	pen.setJoinType(GisEngine::Display::JoinTypeRound);
	pen.setPenType(GisEngine::Display::PenTypeSolid);


	for (int i=0; i< model.nfaces(); i++) {
		std::vector<int> face = model.face(i);
		for (int j = 0; j < 3; j++) {
			Vec3f v0 = model.vert(face[j]);
			Vec3f v1 = model.vert(face[(j + 1)%3]);
			int x0 = (v0.x + 1.)*width/2.;
			int y0 = (v0.y + 1.)*height/2.;
			int x1 = (v1.x + 1.)*width/2.;
			int y1 = (v1.y + 1.)*height/2.;
			m_pGraphicsAgg->DrawLineSeg(&pen, x0, y0, x1, y1);
		}
	}

	CDC dc(GetDC());
	 ::BitBlt(dc.m_hDC, 0, 0, m_pGraphicsAgg->GetWidth(), m_pGraphicsAgg->GetHeight(), m_pGraphicsAgg->GetDC(), 0, 0, SRCCOPY);
	return 0;
}

LRESULT CTestGraphicsView::OnTriangleRender(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{

	if(!m_pGraphicsAgg.get())
		return 0;

	Model model("d:\\work\\MyProject\\obj\\african_head\\african_head.obj");


	m_pGraphicsAgg->Erase(GisEngine::Display::Color(0, 0, 0, 255));
	int width = m_pGraphicsAgg->GetWidth();
	int height = m_pGraphicsAgg->GetHeight();
	Vec3f light_dir(0,0,-1);
	for (int i=0; i<model.nfaces(); i++) {
		std::vector<int> face = model.face(i);
		Vec2i screen_coords[3];
		Vec3f world_coords[3];
		for (int j=0; j<3; j++) {
			Vec3f v = model.vert(face[j]);
			screen_coords[j] = Vec2i((v.x+1.)*width/2., (v.y+1.)*height/2.);
			world_coords[j]  = v;
		}
	//	Vec3f n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]);
		Vec3f vec1 = (world_coords[2]-world_coords[0]);
		Vec3f vec2 = (world_coords[1]-world_coords[0]);
		 
		 Vec3f n = vec1 ^ vec2;
		n.normalize();
		float intensity = n*light_dir;
		if (intensity > 0 )
			triangle(screen_coords[0], screen_coords[1], screen_coords[2], m_pGraphicsAgg.get(), GisEngine::Display::Color(intensity*255, intensity* 255, intensity *255, 255));
	}

 


	CDC dc(GetDC());
	::BitBlt(dc.m_hDC, 0, 0, m_pGraphicsAgg->GetWidth(), m_pGraphicsAgg->GetHeight(), m_pGraphicsAgg->GetDC(), 0, 0, SRCCOPY);
	return 0;
}