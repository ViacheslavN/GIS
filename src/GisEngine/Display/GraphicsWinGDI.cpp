#include "stdafx.h"
#include "GraphicsWinGDI.h"

namespace GisEngine
{
	namespace Display
	{
		CGraphicsWinGDI::CGraphicsWinGDI(HWND hWnd){}
		CGraphicsWinGDI::CGraphicsWinGDI(HDC hDC, bool dIsdeleteDC){}
		CGraphicsWinGDI::~CGraphicsWinGDI(){}



		 DeviceType  CGraphicsWinGDI::GetDeviceType() const
		 {
			 switch(::GetDeviceCaps(m_hDC, TECHNOLOGY))
			 {
			 case DT_RASCAMERA:
			 case DT_RASDISPLAY: 
				 return DeviceTypeDisplay;
			 case DT_RASPRINTER: 
				 return DeviceTypePrinter ;
			 case DT_PLOTTER: 
				 return DeviceTypePlotter;
			 case DT_CHARSTREAM:
			 case DT_METAFILE:
			 case DT_DISPFILE: 
				 return DeviceTypeMetafile;
			 }
			 return DeviceTypeDisplay;
		 }
		 void        CGraphicsWinGDI::StartDrawing(){}
		 void        CGraphicsWinGDI:: EndDrawing(){}
		 GRect       CGraphicsWinGDI::GetClipRect() const 
		 {
			 GRect res;

			 RECT rect;
			 HRGN hrgn = ::CreateRectRgn(0, 0, 0, 0);
			 if(ERROR != GetClipRgn(m_hDC, hrgn))
			 {
				 if(ERROR != GetRgnBox(hrgn, &rect))
					 res.set(GUnits(rect.left), GUnits(rect.top), GUnits(rect.right), GUnits(rect.bottom));
			 }
			 else
			 {
				 res.set(0, 0, GUnits(GetDeviceCaps(m_hDC, HORZRES)), GUnits(GetDeviceCaps(m_hDC, VERTRES)));
			 }
			 ::DeleteObject(hrgn);

			 return res;
		 }
		 void        CGraphicsWinGDI::SetClipRect(const GRect& rect){}
		 void        CGraphicsWinGDI::SetClipRgn(const GPoint* lpPoints, const int *lpPolyCounts, int nCount){}
		 void        CGraphicsWinGDI::RemoveClip() {}
		 void        CGraphicsWinGDI::Erase(const Color& color, const GRect *rect) {}
		 IGraphics*   CGraphicsWinGDI::CreateCompatibleGraphics(GUnits width, GUnits height)
		 {
			 HDC hDC = ::CreateCompatibleDC(m_hDC);
			 HBITMAP hBitmap = ::CreateCompatibleBitmap(m_hDC, (int)width, (int)height);
			 DeleteObject(::SelectObject(hDC, hBitmap));
			 return new CGraphicsWinGDI(hDC, true);

		 }
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

		 Color CGraphicsWinGDI::GetPixel(GUnits x, GUnits y)
		 {
			 COLORREF ref = ::GetPixel(m_hDC, (int)x, (int)y);
			 return Color(ref);
		 }

		 GPoint CGraphicsWinGDI::GetViewportOrg() const
		 {
			 POINT point;
			 ::SetViewportOrgEx(m_hDC, 0, 0, &point);
			 ::SetViewportOrgEx(m_hDC, point.x, point.y, 0);

			 return GPoint((GUnits)point.x, (GUnits)point.y);
		 }
		 void   CGraphicsWinGDI::SetViewportOrg(const GPoint& org){}

		 GPoint CGraphicsWinGDI::GetBrushOrg() const 
		 {
			 POINT point;

			 ::SetBrushOrgEx(m_hDC, 0, 0, &point);
			 ::SetBrushOrgEx(m_hDC, point.x, point.y, 0);

			 return GPoint((GUnits)point.x, (GUnits)point.y);
		 }
		 void   CGraphicsWinGDI::SetBrushOrg(const GPoint& org){}

		 const CBitmap& CGraphicsWinGDI::GetSurface() const
		 {
			 return m_Surface;
		 }
		 CBitmap& CGraphicsWinGDI::GetSurface()
		 {
			  return m_Surface;
		 }

		 size_t CGraphicsWinGDI::GetWidth() const
		 {
			 return (size_t)m_nWidth;
		 }
		 size_t CGraphicsWinGDI::GetHeight() const
		 {
			  return (size_t)m_nHeight;
		 } 

		 void CGraphicsWinGDI::Lock(){}
		 void CGraphicsWinGDI::Release(){}
		void   CGraphicsWinGDI::SelectPen(const CPen* pPen){}
		void   CGraphicsWinGDI::SelectBrush(const CBrush* pBrush){}
		HPEN   CGraphicsWinGDI::CreatePen(const CPen* pPen)
		{
			LOGPEN logpen;
			logpen.lopnColor = pPen->getColor().GetRGB();
			logpen.lopnStyle = pPen->getPenType();
			logpen.lopnWidth.x = (LONG)pPen->getWidth();
			return ::CreatePenIndirect(&logpen);
		}
		HBRUSH CGraphicsWinGDI::CreateBrush(const CBrush* pBrush)
		{
			LOGBRUSH logbrush;
			logbrush.lbColor = pBrush->GetColor().GetRGB();
			logbrush.lbStyle = pBrush->GetType();
			//switch(brush->htype_)
			//{
			//case BrushHTypeBdiagonal:
			//  logbrush.lbHatch = HS_BDIAGONAL;
			//  break;
			//case BrushHTypeCross:
			//  logbrush.lbHatch = HS_CROSS;
			//  break;
			//case BrushHTypeDiagcross:
			//  logbrush.lbHatch = HS_DIAGCROSS;
			//  break;
			//case BrushHTypeFdiagonal:
			//  logbrush.lbHatch = HS_FDIAGONAL;
			//  break;
			//case BrushHTypeHorizontal:
			//  logbrush.lbHatch = HS_HORIZONTAL;
			//  break;
			//case BrushHTypeVertical:
			//  logbrush.lbHatch = HS_VERTICAL;
			//  break;
			//}
			return ::CreateBrushIndirect(&logbrush);
		}
		void   CGraphicsWinGDI::CreateFont(const CFont* pFont){}
	}
}