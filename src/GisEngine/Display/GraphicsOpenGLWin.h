#pragma once
#include "GraphicsOpenGL.h"

namespace GisEngine
{
	namespace Display
	{

		class CGraphicsOpenGLWin : public CGraphicsOpenGL
		{
		public:
			CGraphicsOpenGLWin(HDC hDC, int width, int height);
			~CGraphicsOpenGLWin();

			virtual void        StartDrawing(IDisplayTransformation *pDT = nullptr);
			virtual void        EndDrawing();


			virtual void        Copy(IGraphics* src, const GPoint& srcPoint, const GRect& dstRect, bool bBlend = true);
			virtual const CBitmap& GetSurface() const;
			virtual CBitmap& GetSurface();
			virtual HDC GetDC() const { return m_hMemDC; }
		private:
			void init();
		private:

			HWND    m_hWnd;
			//HDC m_hDC;
			HDC m_hMemDC;

			HGLRC  m_hglrc;
			bool m_bIsReleaseDC;

			HBITMAP m_oldBitmap;
			CBitmap m_surface;

		};
	}
}



