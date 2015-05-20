#include "stdafx.h"
#include "GraphicsWinGDI.h"

namespace GisEngine
{
	namespace Display
	{
		CGraphicsWinGDI::CGraphicsWinGDI(HWND hWnd){}
		CGraphicsWinGDI::CGraphicsWinGDI(HDC hDC, bool dIsdeleteDC){}
		CGraphicsWinGDI::~CGraphicsWinGDI(){}



		 DeviceType  CGraphicsWinGDI::GetDeviceType() const{}
		 void        CGraphicsWinGDI::StartDrawing(){}
		 void        CGraphicsWinGDI:: EndDrawing(){}
		 GRect       CGraphicsWinGDI::GetClipRect() const {}
		 void        CGraphicsWinGDI::SetClipRect(const GRect& rect){}
		 void        CGraphicsWinGDI::SetClipRgn(const GPoint* lpPoints, const int *lpPolyCounts, int nCount){}
		 void        CGraphicsWinGDI::RemoveClip() {}
		 void        CGraphicsWinGDI::Erase(const Color& color, const GRect *rect) {}
		 IGraphics*   CGraphicsWinGDI::CreateCompatibleGraphics(GUnits width, GUnits height){}
		 void        CGraphicsWinGDI::Copy(IGraphics* src, const GPoint& srcPoint, const GRect& dstRect, bool bBlend){}

		 void CGraphicsWinGDI::DrawPoint(const CPen* pPen, const CBrush*  pBbrush, const GPoint& Pt){}
		 void CGraphicsWinGDI::DrawPoint(const CPen* pPen, const CBrush*  pBbrush, GUnits dX, GUnits dY){}
		 void CGraphicsWinGDI::DrawPixel(GUnits dX, GUnits dY, const Color &color){}

		 void CGraphicsWinGDI::DrawLineSeg(const CPen* pPen, const GPoint& P1, const GPoint& P2){}
		 void CGraphicsWinGDI::DrawLineSeg(const CPen* pPen, GUnits dX1, GUnits dY1, GUnits dX2, GUnits dY2){}

		 void CGraphicsWinGDI::DrawLine(const CPen* pPen, const GPoint* pPoints, int nNumPoints) {}

		 void CGraphicsWinGDI::DrawRoundRect(const CPen* pPen, const CBrush*  pBbrush, const GRect& Rect, GUnits radius){}
		 void CGraphicsWinGDI::DrawRect(const CPen* pPen, const CBrush*  pBbrush, const GRect& Rect){}
		 void CGraphicsWinGDI::DrawRect(CPen* pPen, CBrush*  pBbrush, const GPoint& LTPoint, const GPoint& RBPoint){}
		 void CGraphicsWinGDI::DrawRect(CPen* pPen, CBrush*  pBbrush, GUnits dLTX, GUnits dLTY, GUnits dRBX, GUnits dRBY) {}

		 void CGraphicsWinGDI::DrawRectEx(const CPen* pPen, const CBrush*  pBbrush, const GRect& Rect, const GPoint& originMin, const GPoint& originMax){}
		 void CGraphicsWinGDI::DrawEllipse(const CPen* pPen, const CBrush*  pBbrush, const GRect& Rect){}
		 void CGraphicsWinGDI::DrawEllipse(const CPen* pPen, const CBrush*  pBbrush, const GPoint& LTPoint, const GPoint& RBPoint){}
		 void CGraphicsWinGDI::DrawEllipse(const CPen* pPen, const CBrush*  pBbrush, GUnits dLTX, GUnits dLTY, GUnits dRBX, GUnits dRBY){}

		 void CGraphicsWinGDI::DrawPolygon(const CPen* pPen, const CBrush*  pBbrush, const GPoint* pPoints, int nNumPoints){}
		 void CGraphicsWinGDI::DrawPolyPolygon(const CPen* pPen, const CBrush*  pBbrush, const GPoint* lpPoints, const int *lpPolyCounts, int nCount){}

		 void CGraphicsWinGDI::DrawPolygonEx(const CPen* pPen, const CBrush*  pBbrush, const GPoint* pPoints, int nNumPoints, const GPoint& originMin, const GPoint& originMax){}
		 void CGraphicsWinGDI::DrawPolyPolygonEx(const CPen* pPen, const CBrush*  pBbrush, const GPoint* lpPoints, const int *lpPolyCounts, int nCount, const GPoint& originMin, const GPoint& originMax){}

		 void CGraphicsWinGDI::QueryTextMetrics(const CFont* pFont, GUnits* height, GUnits* baseLine, GUnits* lineSpacing) {}
		 void CGraphicsWinGDI::QueryTextMetrics(const CFont* pFont, const wchar_t* text, int len, GUnits* width, GUnits* height, GUnits* baseLine){}
		 void CGraphicsWinGDI::DrawText(const CFont* pFont, const wchar_t* text, int len, const GPoint& point, int drawFlags){}
		 void CGraphicsWinGDI::DrawTextByLine(const CFont* pFont, const wchar_t* text, int len, const GPoint* pPoints, int nNumPoints){}

		 void CGraphicsWinGDI::DrawBitmap(const CBitmap* bitmap, const GRect& dstRect, bool flip_y, unsigned char alpha ){}
		 void CGraphicsWinGDI::DrawRotatedBitmap(const CBitmap* bitmap, const GPoint& center, double angle, bool flip_y, unsigned char alpha, double scale_x, double scale_y, bool clip, bool onGrid ){}

		 Color CGraphicsWinGDI::GetPixel(GUnits x, GUnits y){}

		 GPoint CGraphicsWinGDI::GetViewportOrg() const{}
		 void   CGraphicsWinGDI::SetViewportOrg(const GPoint& org){}

		 GPoint CGraphicsWinGDI::GetBrushOrg() const {}
		 void   CGraphicsWinGDI::SetBrushOrg(const GPoint& org){}

		 const CBitmap& CGraphicsWinGDI::GetSurface() const{}
		 CBitmap& CGraphicsWinGDI::GetSurface(){}

		 size_t CGraphicsWinGDI::GetWidth() const{}
		 size_t CGraphicsWinGDI::GetHeight() const{} 

		 void CGraphicsWinGDI::Lock(){}
		 void CGraphicsWinGDI::Release(){}
		void   CGraphicsWinGDI::SelectPen(const CPen* pPen){}
		void   CGraphicsWinGDI::SelectBrush(const CBrush* pBrush){}
		HPEN   CGraphicsWinGDI::CreatePen(const CPen* pPen){}
		HBRUSH CGraphicsWinGDI::CreateBrush(const CBrush* pBrush){}
		void   CGraphicsWinGDI::CreateFont(const CFont* pFont){}
	}
}