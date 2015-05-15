#ifndef GIS_ENGINE_DISPLAY_GRAPHICS_H_
#define GIS_ENGINE_DISPLAY_GRAPHICS_H_
#include "CommonLibrary/GeneralTypes.h"
#include "GraphTypes.h"
#include "Bitmap.h"
#include "Common/Common.h"
#include "Point.h"
#include "Rect.h"
#include "Bitmap.h"
#include "Pen.h"
#include "Font.h"
#include "Brush.h"

namespace GisEngine
{
	namespace Display
	{

		class IGraphics
		{
			public:
				IGraphics(){}
				virtual ~IGraphics(){}

				virtual DeviceType  GetDeviceType() const = 0;
				virtual void        StartDrawing() = 0;
				virtual void        EndDrawing() = 0;
				virtual GRect       GetClipRect() const = 0;
				virtual void        SetClipRect(const GRect& rect) = 0;
				virtual void        SetClipRgn(const GPoint* lpPoints, const int *lpPolyCounts, int nCount) = 0;
				virtual void        RemoveClip() = 0;
				virtual void        Erase(const Color& color, const GRect *rect = 0) = 0;
				virtual IGraphics*   CreateCompatibleGraphics(GUnits width, GUnits height) = 0;
				virtual void        Copy(IGraphics* src, const GPoint& srcPoint, const GRect& dstRect, bool bBlend = true) = 0;

				virtual void DrawPoint(const CPen* pPen, const CBrush*  pBbrush, const GPoint& Pt) = 0;
				virtual void DrawPoint(const CPen* pPen, const CBrush*  pBbrush, GUnits dX, GUnits dY) = 0;
				virtual void DrawPixel(GUnits dX, GUnits dY, const Color &color) = 0;

				virtual void DrawLineSeg(const CPen* pPen, const GPoint& P1, const GPoint& P2) = 0;
				virtual void DrawLineSeg(const CPen* pPen, GUnits dX1, GUnits dY1, GUnits dX2, GUnits dY2) = 0;

				virtual void DrawLine(const CPen* pPen, const GPoint* pPoints, int nNumPoints) = 0;

				virtual void DrawRoundRect(const CPen* pPen, const CBrush*  pBbrush, const GRect& Rect, GUnits radius) = 0;
				virtual void DrawRect(const CPen* pPen, const CBrush*  pBbrush, const GRect& Rect) = 0;
				virtual void DrawRect(CPen* pPen, CBrush*  pBbrush, const GPoint& LTPoint, const GPoint& RBPoint) = 0;
				virtual void DrawRect(CPen* pPen, CBrush*  pBbrush, GUnits dLTX, GUnits dLTY, GUnits dRBX, GUnits dRBY) = 0;

				virtual void DrawRectEx(const CPen* pPen, const CBrush*  pBbrush, const GRect& Rect, const GPoint& originMin, const GPoint& originMax);
				virtual void DrawEllipse(const CPen* pPen, const CBrush*  pBbrush, const GRect& Rect);
				virtual void DrawEllipse(const CPen* pPen, const CBrush*  pBbrush, const GPoint& LTPoint, const GPoint& RBPoint) = 0;
				virtual void DrawEllipse(const CPen* pPen, const CBrush*  pBbrush, GUnits dLTX, GUnits dLTY, GUnits dRBX, GUnits dRBY) = 0;

				virtual void DrawPolygon(const CPen* pPen, const CBrush*  pBbrush, const GPoint* pPoints, int nNumPoints) = 0;
				virtual void DrawPolyPolygon(const CPen* pPen, const CBrush*  pBbrush, const GPoint* lpPoints, const int *lpPolyCounts, int nCount) = 0;

				virtual void DrawPolygonEx(const CPen* pPen, const CBrush*  pBbrush, const GPoint* pPoints, int nNumPoints, const GPoint& originMin, const GPoint& originMax);
				virtual void DrawPolyPolygonEx(const CPen* pPen, const CBrush*  pBbrush, const GPoint* lpPoints, const int *lpPolyCounts, int nCount, const GPoint& originMin, const GPoint& originMax);

				virtual void QueryTextMetrics(const CFont* pFont, GUnits* height, GUnits* baseLine, GUnits* lineSpacing) = 0;
				virtual void QueryTextMetrics(const CFont* pFont, const wchar_t* text, int len, GUnits* width, GUnits* height, GUnits* baseLine) = 0;
				virtual void DrawText(const CFont* pFont, const wchar_t* text, int len, const GPoint& point, int drawFlags = TextDrawAll) = 0;
				virtual void DrawTextByLine(const CFont* pFont, const wchar_t* text, int len, const GPoint* pPoints, int nNumPoints) = 0;

				virtual void DrawBitmap(const CBitmap* bitmap, const GRect& dstRect, bool flip_y, unsigned char alpha = 255) = 0;
				virtual void DrawRotatedBitmap(const CBitmap* bitmap, const GPoint& center, double angle, bool flip_y = false, unsigned char alpha = 255, double scale_x = 1.0, double scale_y = 1.0, bool clip = true, bool onGrid = false) = 0;

				virtual Color GetPixel(GUnits x, GUnits y) = 0;

				virtual GPoint GetViewportOrg() = 0;
				virtual void   SetViewportOrg(const GPoint& org) = 0;

				virtual GPoint GetBrushOrg() = 0;
				virtual void   SetBrushOrg(const GPoint& org) = 0;

				virtual const CBitmap& GetSurface() const = 0;
				virtual CBitmap& GetSurface() = 0;

				virtual size_t GetWidth() const = 0;
				virtual size_t GetHeight() const= 0; 
 
				virtual void Lock() = 0;
				virtual void Release() = 0;


		};
	}
}

#endif