#include "stdafx.h"
#include "GraphicsOpenGL.h"
#include <gl/GL.h>
#include <gl/GLU.h>
namespace GisEngine
{
	namespace Display
	{
		CGraphicsOpenGL::CGraphicsOpenGL(int width, int height) : m_nWidth(width), m_nHeight(height)
		{
			m_DrawPolygon.Init(m_nWidth, m_nHeight);
		}
	

		CGraphicsOpenGL::~CGraphicsOpenGL()
		{
		 

		}



		eDeviceType  CGraphicsOpenGL::GetDeviceType() const
		{
			return DeviceTypeDisplay;
		}
		
		GRect  CGraphicsOpenGL::GetClipRect() const
		{
			GRect res;

		

			return res;
		}
		void   CGraphicsOpenGL::SetClipRect(const GRect& rect)
		{

		}
		void CGraphicsOpenGL::SetClipRgn(const GPoint* lpPoints, const int *lpPolyCounts, int nCount)
		{

		}
		void  CGraphicsOpenGL::RemoveClip()
		{

		}
		void  CGraphicsOpenGL::Erase(const Color& color, const GRect *pRect)
		{
			if (pRect == nullptr)
			{
				::glClearColor(color.GetR() / 255.0f, color.GetG() / 255.0f, color.GetB() / 255.0f, color.GetA() / 255.0f);

				GLfloat params[4];
				::glGetFloatv(GL_COLOR_CLEAR_VALUE, params);

				::glClear(GL_COLOR_BUFFER_BIT);
			}
		}
		IGraphics*   CGraphicsOpenGL::CreateCompatibleGraphics(GUnits width, GUnits height)
		{		
			return nullptr;

		}
		

		void CGraphicsOpenGL::DrawPoint(const CPen* pPen, const CBrush*  pBbrush, const GPoint& Pt)
		{
		
		}
		void CGraphicsOpenGL::DrawPoint(const CPen* pPen, const CBrush*  pBbrush, GUnits dX, GUnits dY)
		{
			
		}
		void CGraphicsOpenGL::DrawPixel(GUnits dX, GUnits dY, const Color &color)
		{
		}

		void CGraphicsOpenGL::DrawLineSeg(const CPen* pPen, const GPoint& P1, const GPoint& P2)
		{
		}
		void CGraphicsOpenGL::DrawLineSeg(const CPen* pPen, GUnits dX1, GUnits dY1, GUnits dX2, GUnits dY2)
		{

		}

		void CGraphicsOpenGL::DrawLine(const CPen* pPen, const GPoint* pPoints, int nNumPoints)
		{
			::glColor4f((GLfloat)(pPen->getColor().GetR() / 255.0), (GLfloat)(pPen->getColor().GetG() / 255.0), (GLfloat)(pPen->getColor().GetB() / 255.0), (GLfloat)(pPen->getColor().GetA() / 255.0));
			::glLineWidth(GLfloat(pPen->getWidth()));

			//::glEnable(GL_LINE_SMOOTH);
			::glBegin(GL_LINE_STRIP);
			for (int i = 0; i < nNumPoints; i++)
				::glVertex2f(pPoints[i].x + 0.5f, pPoints[i].y + 0.5f);
			::glEnd();
		}

		void CGraphicsOpenGL::DrawRoundRect(const CPen* pPen, const CBrush*  pBbrush, const GRect& Rect, GUnits radius)
		{

		}
		void CGraphicsOpenGL::DrawRect(const CPen* pPen, const CBrush*  pBbrush, const GRect& Rect)
		{

		}
		void CGraphicsOpenGL::DrawRect(CPen* pPen, CBrush*  pBbrush, const GPoint& LTPoint, const GPoint& RBPoint)
		{

		}
		void CGraphicsOpenGL::DrawRect(CPen* pPen, CBrush*  pBbrush, GUnits dLTX, GUnits dLTY, GUnits dRBX, GUnits dRBY)
		{
	
		}

		void CGraphicsOpenGL::DrawRectEx(const CPen* pPen, const CBrush*  pBbrush, const GRect& Rect, const GPoint& originMin, const GPoint& originMax) {}
		void CGraphicsOpenGL::DrawEllipse(const CPen* pPen, const CBrush*  pBbrush, const GRect& Rect)
		{

		}
		void CGraphicsOpenGL::DrawEllipse(const CPen* pPen, const CBrush*  pBbrush, const GPoint& LTPoint, const GPoint& RBPoint)
		{
		}
		void CGraphicsOpenGL::DrawEllipse(const CPen* pPen, const CBrush*  pBbrush, GUnits dLTX, GUnits dLTY, GUnits dRBX, GUnits dRBY)
		{

		}

		void CGraphicsOpenGL::DrawPolygon(const CPen* pPen, const CBrush*  pBbrush, const GPoint* pPoints, int nNumPoints)
		{
			::glColor4f((GLfloat)(pBbrush->GetColor().GetR() / 255.0), (GLfloat)(pBbrush->GetColor().GetG() / 255.0), (GLfloat)(pBbrush->GetColor().GetB() / 255.0), (GLfloat)(pBbrush->GetColor().GetA() / 255.0));
			
			m_DrawPolygon.DrawPolygon(pPoints, nNumPoints);


		}
		void CGraphicsOpenGL::DrawPolyPolygon(const CPen* pPen, const CBrush*  pBbrush, const GPoint* lpPoints, const int *lpPolyCounts, int nCount)
		{
			::glColor4f((GLfloat)(pBbrush->GetColor().GetR() / 255.0), (GLfloat)(pBbrush->GetColor().GetG() / 255.0), (GLfloat)(pBbrush->GetColor().GetB() / 255.0), (GLfloat)(pBbrush->GetColor().GetA() / 255.0));
			m_DrawPolygon.DrawPolyPolygon(lpPoints, lpPolyCounts, nCount);
		
		}

		void CGraphicsOpenGL::DrawPolygonEx(const CPen* pPen, const CBrush*  pBbrush, const GPoint* pPoints, int nNumPoints, const GPoint& originMin, const GPoint& originMax) {}
		void CGraphicsOpenGL::DrawPolyPolygonEx(const CPen* pPen, const CBrush*  pBbrush, const GPoint* lpPoints, const int *lpPolyCounts, int nCount, const GPoint& originMin, const GPoint& originMax) {}

		void CGraphicsOpenGL::QueryTextMetrics(const CFont* pFont, GUnits* height, GUnits* baseLine, GUnits* lineSpacing)
		{

		}
		void CGraphicsOpenGL::QueryTextMetrics(const CFont* pFont, const wchar_t* text, int len, GUnits* width, GUnits* height, GUnits* baseLine)
		{
		
		}
		void CGraphicsOpenGL::DrawText(const CFont* pFont, const wchar_t* text, int len, const GPoint& point, int drawFlags)
		{
		
		}
		void CGraphicsOpenGL::DrawTextByLine(const CFont* pFont, const wchar_t* text, int len, const GPoint* pPoints, int nNumPoints) {}

		void CGraphicsOpenGL::DrawBitmap(const CBitmap* bitmap, const GRect& dstRect, bool flip_y, unsigned char alpha) {}
		void CGraphicsOpenGL::DrawRotatedBitmap(const CBitmap* bitmap, const GPoint& center, double angle, bool flip_y, unsigned char alpha, double scale_x, double scale_y, bool clip, bool onGrid) {}

		Color CGraphicsOpenGL::GetPixel(GUnits x, GUnits y)
		{
			COLORREF ref;
			return Color(ref);
		}

		GPoint CGraphicsOpenGL::GetViewportOrg() const
		{
			POINT point;

			return GPoint((GUnits)point.x, (GUnits)point.y);
		}
		void   CGraphicsOpenGL::SetViewportOrg(const GPoint& org)
		{

		}

		GPoint CGraphicsOpenGL::GetBrushOrg() const
		{
			POINT point;
	
			return GPoint((GUnits)point.x, (GUnits)point.y);
		}
		void   CGraphicsOpenGL::SetBrushOrg(const GPoint& org)
		{

		}

	
		size_t CGraphicsOpenGL::GetWidth() const
		{
			return (size_t)m_nWidth;
		}
		size_t CGraphicsOpenGL::GetHeight() const
		{
			return (size_t)m_nHeight;
		}

		void CGraphicsOpenGL::Lock() {}
		void CGraphicsOpenGL::UnLock() {}
	
	}
}