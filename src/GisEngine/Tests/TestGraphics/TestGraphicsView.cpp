// TestGraphicsView.cpp : implementation of the CTestGraphicsView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "TestGraphicsView.h"

BOOL CTestGraphicsView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

LRESULT CTestGraphicsView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);
	GisEngine::Display::CPen pen;
	pen.setWidth(10);
	pen.setColor(GisEngine::Display::Color(0, 255, 0, 255));
	pen.setCapType(GisEngine::Display::CapTypeRound);
	pen.setJoinType(GisEngine::Display::JoinTypeRound);
	pen.setPenType(GisEngine::Display::PenTypeDashDotDot);
	if(m_pGraphicsAgg.get())
	{
		m_pGraphicsAgg->Erase(GisEngine::Display::Color(255, 0, 0, 255));
		m_pGraphicsAgg->DrawLineSeg(&pen, 0, 0, 200, 200);



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

	m_pGraphicsAgg.reset(new GisEngine::Display::CGraphicsAgg(nWidth, nHeight));
	return 0;
}