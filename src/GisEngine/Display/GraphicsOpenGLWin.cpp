#include "stdafx.h"
#include "GraphicsOpenGLWin.h"
#include <gl/GL.h>
namespace GisEngine
{
	namespace Display
	{


		CGraphicsOpenGLWin::CGraphicsOpenGLWin(HDC hDC, int width, int height) : CGraphicsOpenGL(width, height),
			/*m_hDC(0),*/  m_bIsReleaseDC(false), m_hMemDC(0)
		{
		
		/*	if (m_hDC != 0)
			{
				PIXELFORMATDESCRIPTOR pfd;
				::DescribePixelFormat(m_hDC, 1, sizeof(pfd), &pfd);
				int idx = ChoosePixelFormat(m_hDC, &pfd);
				BOOL res = ::SetPixelFormat(m_hDC, idx, &pfd);
			}*/

			size_t line_size = 4 * (((size_t)m_nWidth * 32 + 31) / 32);
			size_t img_size = line_size* (int)m_nHeight;
			size_t rgb_size = 0;
			size_t full_size = sizeof(BITMAPINFOHEADER);// + rgb_size + img_size;


			BITMAPINFO bitmapInfo;
			bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bitmapInfo.bmiHeader.biWidth = (LONG)m_nWidth;
			bitmapInfo.bmiHeader.biHeight = (LONG)m_nHeight;
			bitmapInfo.bmiHeader.biPlanes = 1;
			bitmapInfo.bmiHeader.biBitCount = 32;
			bitmapInfo.bmiHeader.biCompression = BI_RGB;
			bitmapInfo.bmiHeader.biSizeImage = img_size;
			bitmapInfo.bmiHeader.biXPelsPerMeter = 0;
			bitmapInfo.bmiHeader.biYPelsPerMeter = 0;
			bitmapInfo.bmiHeader.biClrUsed = 0;
			bitmapInfo.bmiHeader.biClrImportant = 0;


			//HDC hDC1 = ::GetDC(NULL);
			m_hMemDC = CreateCompatibleDC(0);

			m_bIsReleaseDC = true;

			unsigned char* buf;
			HBITMAP bitmap = ::CreateDIBSection(m_hMemDC, &bitmapInfo, DIB_RGB_COLORS, (void**)&buf, 0, 0);
			m_oldBitmap = (HBITMAP)::SelectObject(m_hMemDC, bitmap);

			m_surface.attach(buf, size_t(width), size_t(height), BitmapFormatType32bppARGB);
		
		}
		CGraphicsOpenGLWin::~CGraphicsOpenGLWin()
		{
			if (m_bIsReleaseDC)
				::DeleteDC(m_hMemDC);
		}

		const CBitmap& CGraphicsOpenGLWin::GetSurface() const
		{
			return m_surface;
		}
		CBitmap& CGraphicsOpenGLWin::GetSurface()
		{
			return m_surface;
		}
		void  CGraphicsOpenGLWin::StartDrawing(IDisplayTransformation *pDT)
		{
	 

		

			if (::wglGetCurrentContext() == 0)
			{
				PIXELFORMATDESCRIPTOR pfd;
				// Set the pixel format
				ZeroMemory(&pfd, sizeof(pfd));
				pfd.nSize = sizeof(pfd);
				pfd.nVersion = 1;
				pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_BITMAP;
				pfd.iPixelType = PFD_TYPE_RGBA;
				pfd.cColorBits = 32;
				pfd.cDepthBits = 32;
				pfd.iLayerType = PFD_MAIN_PLANE;

				//	::DescribePixelFormat(m_hMemDC, 1, sizeof(pfd), &pfd);
				int idx = ChoosePixelFormat(m_hMemDC, &pfd);
				BOOL res = ::SetPixelFormat(m_hMemDC, idx, &pfd);

				m_hglrc = ::wglCreateContext(m_hMemDC);
				 res = ::wglMakeCurrent(m_hMemDC, m_hglrc);
				init();
			}
		}
		void  CGraphicsOpenGLWin::EndDrawing()
		{
			::glFlush();
			//wglMakeCurrent(NULL, NULL);
			//wglDeleteContext(rc_);
		}

		void  CGraphicsOpenGLWin::Copy(IGraphics* pSrc, const GPoint& srcPoint, const GRect& dstRect, bool bBlend)
		{
			BITMAPINFO bmi;
			::memset(&bmi, 0, sizeof(bmi));
			bmi.bmiHeader.biBitCount = 32;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biHeight = (int)pSrc->GetSurface().height();
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = pSrc->GetSurface().width();
			::StretchDIBits(
				m_hMemDC,
				(int)dstRect.xMin, (int)dstRect.yMin, (int)dstRect.width(), (int)dstRect.height(),
				(int)srcPoint.x, (int)(pSrc->GetSurface().height() - (srcPoint.y + dstRect.height() - 1)), (int)dstRect.width(), (int)dstRect.height(),
				pSrc->GetSurface().bits(),
				&bmi,
				DIB_RGB_COLORS, SRCCOPY);
		}

		void CGraphicsOpenGLWin::init()
		{
			::glMatrixMode(GL_PROJECTION);
			::glLoadIdentity();
			::glOrtho(0, m_nWidth, m_nHeight, 0, 0, 1);
			::glDisable(GL_DEPTH_TEST);
			::glMatrixMode(GL_MODELVIEW);
			::glLoadIdentity();
			::glEnable(GL_BLEND);
			::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			::glColor4f(1.0, 0, 0, 1.0);
			::glEnable(GL_LINE_SMOOTH);
			//::glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);

			const GLubyte* str = glGetString(GL_VENDOR);
			str = glGetString(GL_RENDERER);
			str = glGetString(GL_VERSION);
		}

	}
}


