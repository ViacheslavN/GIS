#pragma once

#include "Graphics.h"
namespace GisEngine
{
	namespace Display
	{
		class CGraphicsOpenGL : public IGraphics
		{
		public:

#ifdef _WIN32
			CGraphicsOpenGL(int width, int height);
			~CGraphicsOpenGL();

#endif

			virtual eDeviceType  GetDeviceType() const;
			virtual GRect       GetClipRect() const;
			virtual void        SetClipRect(const GRect& rect);
			virtual void        SetClipRgn(const GPoint* lpPoints, const int *lpPolyCounts, int nCount);
			virtual void        RemoveClip();
			virtual void        Erase(const Color& color, const GRect *rect = 0);
			virtual IGraphics*   CreateCompatibleGraphics(GUnits width, GUnits height);


			virtual void DrawPoint(const CPen* pPen, const CBrush*  pBbrush, const GPoint& Pt);
			virtual void DrawPoint(const CPen* pPen, const CBrush*  pBbrush, GUnits dX, GUnits dY);
			virtual void DrawPixel(GUnits dX, GUnits dY, const Color &color);

			virtual void DrawLineSeg(const CPen* pPen, const GPoint& P1, const GPoint& P2);
			virtual void DrawLineSeg(const CPen* pPen, GUnits dX1, GUnits dY1, GUnits dX2, GUnits dY2);

			virtual void DrawLine(const CPen* pPen, const GPoint* pPoints, int nNumPoints);

			virtual void DrawRoundRect(const CPen* pPen, const CBrush*  pBbrush, const GRect& Rect, GUnits radius);
			virtual void DrawRect(const CPen* pPen, const CBrush*  pBbrush, const GRect& Rect);
			virtual void DrawRect(CPen* pPen, CBrush*  pBbrush, const GPoint& LTPoint, const GPoint& RBPoint);
			virtual void DrawRect(CPen* pPen, CBrush*  pBbrush, GUnits dLTX, GUnits dLTY, GUnits dRBX, GUnits dRBY);

			virtual void DrawRectEx(const CPen* pPen, const CBrush*  pBbrush, const GRect& Rect, const GPoint& originMin, const GPoint& originMax);
			virtual void DrawEllipse(const CPen* pPen, const CBrush*  pBbrush, const GRect& Rect);
			virtual void DrawEllipse(const CPen* pPen, const CBrush*  pBbrush, const GPoint& LTPoint, const GPoint& RBPoint);
			virtual void DrawEllipse(const CPen* pPen, const CBrush*  pBbrush, GUnits dLTX, GUnits dLTY, GUnits dRBX, GUnits dRBY);

			virtual void DrawPolygon(const CPen* pPen, const CBrush*  pBbrush, const GPoint* pPoints, int nNumPoints);
			virtual void DrawPolyPolygon(const CPen* pPen, const CBrush*  pBbrush, const GPoint* lpPoints, const int *lpPolyCounts, int nCount);

			virtual void DrawPolygonEx(const CPen* pPen, const CBrush*  pBbrush, const GPoint* pPoints, int nNumPoints, const GPoint& originMin, const GPoint& originMax);
			virtual void DrawPolyPolygonEx(const CPen* pPen, const CBrush*  pBbrush, const GPoint* lpPoints, const int *lpPolyCounts, int nCount, const GPoint& originMin, const GPoint& originMax);

			virtual void QueryTextMetrics(const CFont* pFont, GUnits* height, GUnits* baseLine, GUnits* lineSpacing);
			virtual void QueryTextMetrics(const CFont* pFont, const wchar_t* text, int len, GUnits* width, GUnits* height, GUnits* baseLine);
			virtual void DrawText(const CFont* pFont, const wchar_t* text, int len, const GPoint& point, int drawFlags = TextDrawAll);
			virtual void DrawTextByLine(const CFont* pFont, const wchar_t* text, int len, const GPoint* pPoints, int nNumPoints);

			virtual void DrawBitmap(const CBitmap* bitmap, const GRect& dstRect, bool flip_y, unsigned char alpha = 255);
			virtual void DrawRotatedBitmap(const CBitmap* bitmap, const GPoint& center, double angle, bool flip_y = false, unsigned char alpha = 255, double scale_x = 1.0, double scale_y = 1.0, bool clip = true, bool onGrid = false);

			virtual Color GetPixel(GUnits x, GUnits y);

			virtual GPoint GetViewportOrg() const;
			virtual void   SetViewportOrg(const GPoint& org);

			virtual GPoint GetBrushOrg() const;
			virtual void   SetBrushOrg(const GPoint& org);


			virtual size_t GetWidth() const;
			virtual size_t GetHeight() const;

			virtual void Lock();
			virtual void UnLock();
		protected:
			GUnits m_nWidth;
			GUnits m_nHeight;
			
		//	CDrawPolygonOpenGL m_DrawPolygon;
		};

	}
}
