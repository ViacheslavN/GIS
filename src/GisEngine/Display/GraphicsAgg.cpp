#include "stdafx.h"
#include "GraphicsAgg.h"
namespace GisEngine
{
	namespace Display
	{
		CGraphicsAgg::CGraphicsAgg(unsigned char* buf, GUnits  width, GUnits height, bool bRelease, bool flipY) : 
			m_nWidth(width),
			m_nHeight(height),
			offset_stroke_(vertex_src_),
			conv_stroke_(offset_stroke_),
			dash_stroke_(vertex_src_),
			dash_offset_stroke_(dash_stroke_),
			dash_conv_stroke_(dash_offset_stroke_),
			font_manager_(font_engine_),
			curves_(font_manager_.path_adaptor()),
			contour_(curves_),
			bspline_(this->vertex_src_),
			fsegm_(curves_),
			ftrans_(fsegm_, tcurve_),
			fcontour_(ftrans_),
			rendering_buffer_(rbuf_),
			m_dc(0),
			m_bflipY(flipY)


		{

			size_t line_size =  4 * (((size_t)width * 32 + 31) / 32);
			m_surface.attach(buf, size_t(width), size_t(height), BitmapFormatType32bppARGB, 0, bRelease);
			rbuf_.attach(m_surface.bits(), (uint32)width, (uint32)height, flipY ? (int)line_size : -1 *(int)line_size);
			renderer_.attach(renderer_base_);
			renderer_base_.attach(this->rendering_buffer_);


		}

		CGraphicsAgg::CGraphicsAgg( GUnits  width, GUnits height, bool flipY): 
			m_nWidth(width),
			m_nHeight(height),
			offset_stroke_(vertex_src_),
			conv_stroke_(offset_stroke_),
			dash_stroke_(vertex_src_),
			dash_offset_stroke_(dash_stroke_),
			dash_conv_stroke_(dash_offset_stroke_),
			font_manager_(font_engine_),
			curves_(font_manager_.path_adaptor()),
			contour_(curves_),
			bspline_(this->vertex_src_),
			fsegm_(curves_),
			ftrans_(fsegm_, tcurve_),
			fcontour_(ftrans_),
			rendering_buffer_(rbuf_),
			m_dc(0),
			m_bflipY(flipY)
		{
			size_t line_size =  4 * (((size_t)width * 32 + 31) / 32);
#ifndef ANDROID
			m_dc = ::CreateCompatibleDC(0);


			size_t img_size = line_size* (int)height;
			size_t rgb_size = 0;
			size_t full_size = sizeof(BITMAPINFOHEADER);// + rgb_size + img_size;

			BITMAPINFO bitmapInfo;
			bitmapInfo.bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
			bitmapInfo.bmiHeader.biWidth  = (LONG)width;
			bitmapInfo.bmiHeader.biHeight = (LONG)height;
			bitmapInfo.bmiHeader.biPlanes = 1;
			bitmapInfo.bmiHeader.biBitCount = 32;
			bitmapInfo.bmiHeader.biCompression = BI_RGB;
			bitmapInfo.bmiHeader.biSizeImage = img_size;
			bitmapInfo.bmiHeader.biXPelsPerMeter = 0;
			bitmapInfo.bmiHeader.biYPelsPerMeter = 0;
			bitmapInfo.bmiHeader.biClrUsed = 0;
			bitmapInfo.bmiHeader.biClrImportant = 0;


			unsigned char* buf;
			HBITMAP bitmap = ::CreateDIBSection(m_dc, &bitmapInfo, DIB_RGB_COLORS, (void**)&buf, 0, 0);
			m_oldBitmap = (HBITMAP)::SelectObject(m_dc, bitmap);
			m_surface.attach(buf, size_t(width), size_t(height), BitmapFormatType32bppARGB);
			rbuf_.attach(m_surface.bits(), (int)width, (int)height, flipY ? (int)line_size : -1 *(int)line_size);
#else
		
			unsigned char* pBuf = new unsigned char[line_size* height_];
			m_surface.attach(pBuf, size_t(width), size_t(height),BitmapFormatType32bppARGB, 0, true);
			rbuf_.attach(m_surface.bits(), (int)width, (int)height, (int)(flipY ? line_size : -line_size));
#endif

			renderer_.attach(renderer_base_);
			renderer_base_.attach(this->rendering_buffer_);
		}
		CGraphicsAgg::~CGraphicsAgg()
		{
#ifndef ANDROID
			::DeleteObject(::SelectObject(m_dc, m_oldBitmap));
			::DeleteDC(m_dc);
#endif
		}
		HDC  CGraphicsAgg::GetDC()
		{
			return m_dc;
		}

		 DeviceType  CGraphicsAgg::GetDeviceType() const
		 {
			   return DeviceTypeDisplay;
		 }
		 void   CGraphicsAgg::StartDrawing()
		 {
			 RemoveClip();
		 }
		 void CGraphicsAgg::EndDrawing()
		 {
			 RemoveClip();
		 }
		 GRect CGraphicsAgg::GetClipRect() const 
		 {
			 return m_ClipRect;
		 }
		 void CGraphicsAgg::SetClipRect(const GRect& rect)
		 {
			 GRect ClipRect = rect;
			 ClipRect.xMin += m_org.x;
			 ClipRect.xMax += m_org.x;
			 ClipRect.yMin += m_org.y;
			 ClipRect.yMax += m_org.y;
			 this->rasterizer_.clip_box(ClipRect.xMin, ClipRect.yMin, ClipRect.xMax, ClipRect.yMax);
			 this->renderer_base_.clip_box((int)ClipRect.xMin, (int)ClipRect.yMin, (int)(ClipRect.xMax - 1), (int)(ClipRect.yMax - 1));
			 m_ClipRect = ClipRect;
		 }
		 void CGraphicsAgg::SetClipRgn(const GPoint* lpPoints, const int *lpPolyCounts, int nCount)
		 {

		 }
		 void CGraphicsAgg::RemoveClip() 
		 {
			 renderer_base_.reset_clipping(true);
			 rasterizer_.clip_box(0, 0, GetSurface().width(), GetSurface().height());
			 m_ClipRect.set(0, 0, (GUnits)GetSurface().width(), (GUnits)GetSurface().height());
			 renderer_base_.reset_polygon_clipping();
		 }
		 void CGraphicsAgg::Erase(const Color& color, const GRect *rect)
		 {
			 if(rect == 0)
			 {
				 if(color.GetRGBA() == 0)
					 ::memset(GetSurface().bits(), 0, 4 * this->renderer_base_.width() * this->renderer_base_.height());
				 else
					 this->renderer_base_.clear(agg::rgba8(color.GetR(), color.GetG(), color.GetB(), color.GetA()));
			 }
			 else
				 this->renderer_base_.copy_bar((int)rect->xMin, (int)rect->yMin, (int)(rect->xMax - 1), (int)(rect->yMax - 1), agg::rgba8(color.GetR(), color.GetG(), color.GetB(), color.GetA()));
		 }

		 IGraphics*   CGraphicsAgg::CreateCompatibleGraphics(GUnits width, GUnits height)
		 {
			   return new CGraphicsAgg(width, height, m_bflipY);
		 }

		 void CGraphicsAgg::InCopy(const GPoint& srcPoint, const GRect& dstRect)
		 {
			 GPoint srcPoint2 = srcPoint;
			 GRect dstRect2 = dstRect;
			 if(dstRect2.xMax > (GUnits)this->renderer_base_.width())
				 dstRect2.xMax = (GUnits)this->renderer_base_.width();
			 if(dstRect2.yMax > (GUnits)this->renderer_base_.height())
				 dstRect2.yMax = (GUnits)this->renderer_base_.height();
			 if(dstRect2.xMin < 0)
			 {
				 srcPoint2.x -= dstRect2.xMin;
				 dstRect2.xMin = 0;
			 }
			 if(dstRect2.yMin < 0)
			 {
				 srcPoint2.y -= dstRect2.yMin;
				 dstRect2.yMin = 0;
			 }

			 if(dstRect2.isEmpty())
				 return;

			 bool forwardY = (srcPoint2.y > dstRect2.yMin);
			 bool forwardX = (srcPoint2.y != dstRect2.yMin) || ((srcPoint2.y == dstRect2.yMin) && (srcPoint2.x > dstRect2.xMin));

			 int ybegin = (int)((forwardY) ? srcPoint2.y : (srcPoint2.y + dstRect2.height()));
			 int yend = (int)((forwardY) ? (srcPoint2.y + dstRect2.height()) : srcPoint2.y - 1);

			 int yoffset = (int)(dstRect2.yMin - srcPoint2.y);
			 int inc = (forwardY) ? 1 : -1;

			 for(int y = ybegin; y != yend; y += inc)
			 {
				 if(y >= (int)this->renderer_base_.height() || y < 0)
					 continue;

				 if(y + yoffset >= (int)this->renderer_base_.height() || y + yoffset < 0)
					 continue;

				 agg::int8u* row = this->renderer_base_.ren().row_ptr(y);
				 agg::int8u* dstRow = this->renderer_base_.ren().row_ptr(y + yoffset);

#ifdef _FLOAT_GUNITS_
				 if(forwardX)
					 ::memcpy(dstRow + (int)(dstRect2.xMin * 4), row + (int)(srcPoint2.x * 4), (int)(dstRect2.width() * 4));
				 else
					 ::memmove(dstRow + (int)(dstRect2.xMin * 4), row + (int)(srcPoint2.x * 4), (int)(dstRect2.width() * 4));
#else
				 if(forwardX)
					 ::memcpy(dstRow + (dstRect2.xMin << 2), row + (srcPoint2.x << 2), dstRect2.Width() << 2);
				 else
					 ::memmove(dstRow + (dstRect2.xMin << 2), row + (srcPoint2.x << 2), dstRect2.Width() << 2);
#endif

			 }
		 }
		 void CGraphicsAgg::Copy(IGraphics* pSrc, const GPoint& srcPt, const GRect& dstRC, bool bBlend)
		 {
			 GPoint srcPoint = srcPt;
			 srcPoint.x += m_org.x;
			 srcPoint.y += m_org.y;
			 GRect dstRect = dstRC;
			 dstRect.xMin += m_org.x;
			 dstRect.xMax += m_org.x;
			 dstRect.yMin += m_org.y;
			 dstRect.yMax += m_org.y;
			 if(pSrc == this)
				 InCopy(srcPoint, dstRect);
			 else if (CGraphicsAgg * pGraphicsAgg = dynamic_cast<CGraphicsAgg*>(pSrc))
			 {
				 agg::rect_i rect((int)srcPoint.x, (int)srcPoint.y, (int)(srcPoint.x + dstRect.width() - 1), (int)(srcPoint.y + dstRect.height() - 1));
				 renderer_base_.blend_from(pGraphicsAgg->rendering_buffer_, &rect, (int)(dstRect.xMin - srcPoint.x), (int)(dstRect.yMin - srcPoint.y));
			 }
			    copy(pSrc, srcPoint, dstRect);
		 }

		 void CGraphicsAgg::copy(IGraphics* pSrc, const GPoint& srcPoint, const GRect& dstRect, bool bBlend)
		 {
			 agg::rect_i rect((int)srcPoint.x, (int)srcPoint.y, (int)(srcPoint.x + dstRect.width() - 1), (int)(srcPoint.y + dstRect.height() - 1));
			 for(int y = (int)srcPoint.y, dsty = (int)dstRect.yMin; y < (int)(srcPoint.y + dstRect.height()); y++, dsty++)
			 {
				 for(int x = (int)srcPoint.x, dstx = (int)dstRect.xMin; x < (int)(srcPoint.x + dstRect.width()); x++, dstx++)
				 {
					 if(dstx >= 0 && dstx < (int)this->rendering_buffer_.width() && dsty >= 0 && dsty < (int)rendering_buffer_.height())
					 {
						 Color pixel = pSrc->GetPixel((GUnits)x, (GUnits)y);
						 this->rendering_buffer_.blend_pixel(dstx, dsty, agg::rgba8(pixel.GetR(), pixel.GetG(), pixel.GetB(), pixel.GetA()), pixel.GetA());
					 }
				 }
			 }
		 }

		 void CGraphicsAgg::DrawPoint(const CPen* pPen, const CBrush*  pBbrush, const GPoint& Pt)
		 {}
		 void CGraphicsAgg::DrawPoint(const CPen* pPen, const CBrush*  pBbrush, GUnits dX, GUnits dY)
		 {}
		 void CGraphicsAgg::DrawPixel(GUnits dX, GUnits dY, const Color &color)
		 {}

		 void CGraphicsAgg::DrawLineSeg(const CPen* pPen, const GPoint& P1, const GPoint& P2)
		 {
			  DrawLineSeg(pPen, P1.x, P1.y, P2.x, P2.y);
		 }
		 void CGraphicsAgg::DrawLineSeg(const CPen* pPen, GUnits dX1, GUnits dY1, GUnits dX2, GUnits dY2)
		 {
			 GPoint points[2];
			 points[0].x = dX1;
			 points[0].y = dY1;
			 points[1].x = dX2;
			 points[1].y = dY2;
			 DrawLine(pPen, points, 2);
		 }

		 void CGraphicsAgg::DrawLine(const CPen* pPen, const GPoint* pPoints, int nNumPoints)
		 {
			 if(pPen == 0 || pPen->getPenType() == PenTypeNull)
				 return;

			 if(pPen->getOffset() != 0 && nNumPoints == 2 && pPoints[0] == pPoints[1])
				 return;

			 if(pPen->getOffset() != 0)
			 {
				 this->offset_stroke_.set_offset(pPen->getOffset());
				 this->dash_offset_stroke_.set_offset(pPen->getOffset());
			 }

			 if(pPoints == 0)
				 return;

			 if(pPen->getWidth() == 0)
				 return;

			 this->vertex_src_.attach(pPoints, &nNumPoints, 1);
			 draw_line(pPen);

			 if(pPen->getOffset() != 0)
			 {
				 this->offset_stroke_.set_offset(0);
				 this->dash_offset_stroke_.set_offset(0);
			 }
		 }
		 template <typename Stroke>
		 void AppendPen(const CPen* pPen, Stroke& stroke)
		 {
			 switch(pPen->getCapType())
			 {
			 case CapTypeButt:
				 stroke.line_cap(agg::butt_cap);
				 break;
			 case CapTypeRound:
				 stroke.line_cap(agg::round_cap);
				 break;
			 case CapTypeSquare:
				 stroke.line_cap(agg::square_cap);
				 break;
			 }
			 switch(pPen->getJoinType())
			 {
			 case JoinTypeMiter:
				 stroke.line_join(agg::miter_join);
				 break;
			 case JoinTypeRound:
				 stroke.line_join(agg::round_join);
				 break;
			 case JoinTypeBevel:
				 stroke.line_join(agg::bevel_join);
				 break;
			 }
		 }

		 void CGraphicsAgg::draw_line(const CPen* pPen)
		 {
			 if(pPen->getPenType() != PenTypeSolid || pPen->getTemplates().size() > 0)
			 {
				 this->dash_stroke_.remove_all_dashes();
				 if(pPen->getTemplates().size() > 0)
				 {
					 for(size_t i = 0, sz = pPen->getTemplates().size(); i < sz; i++)
						 this->dash_stroke_.add_dash(pPen->getTemplates()[i].first, pPen->getTemplates()[i].second);
				 }
				 else
				 {
					 switch(pPen->getPenType())
					 {
					 case PenTypeDash:
						 this->dash_stroke_.add_dash(3 * pPen->getWidth(), 2 * pPen->getWidth());
						 break;
					 case PenTypeDot:
						 this->dash_stroke_.add_dash(pPen->getWidth(), 2 * pPen->getWidth());
						 break;
					 case PenTypeDashDot:
						 this->dash_stroke_.add_dash(3 * pPen->getWidth(), 2 * pPen->getWidth());
						 this->dash_stroke_.add_dash(pPen->getWidth(), 2 * pPen->getWidth());
						 break;
					 case PenTypeDashDotDot: 
						 this->dash_stroke_.add_dash(3 * pPen->getWidth(), 2 * pPen->getWidth());
						 this->dash_stroke_.add_dash(pPen->getWidth(), 2 * pPen->getWidth());
						 this->dash_stroke_.add_dash(pPen->getWidth(), 2 * pPen->getWidth());
						 break;
					 }
				 }
			 }

			 this->renderer_.color(agg::rgba8(pPen->getColor().GetR(),
				 pPen->getColor().GetG(),
				 pPen->getColor().GetB(),
				 pPen->getColor().GetA()));

			 if(pPen->getPenType() != PenTypeSolid || pPen->getTemplates().size() > 0)
			 {
				 AppendPen(pPen, this->dash_conv_stroke_);
				 this->dash_conv_stroke_.width(pPen->getWidth());
				 this->rasterizer_.add_path(this->dash_conv_stroke_);
			 }
			 else
			 {
				 AppendPen(pPen, this->conv_stroke_);
				 this->conv_stroke_.width(pPen->getWidth());
				 this->rasterizer_.add_path(this->conv_stroke_);
			 }
			 if(pPen->getTexture() == 0)
			 {
				 agg::render_scanlines(this->rasterizer_, this->scanline_, 
					 this->renderer_);
			 }
			 else
			 {
				 if(pPen->getTexture()->bpp() != 32)
					 return;

				 CBitmap* pTexture = pPen->getTexture();
				 int width = (int)pTexture->width();
				 int height = (int)pTexture->height();
				 this->pattern_rbuf_.attach(pTexture->bits(), width, height, -width*4); 
				 this->pattern_rgba32_.attach(this->pattern_rbuf_);

				 pattern_accessor_t pattern_accessor(this->pattern_rgba32_);
				 span_pattern_generator_t span_pattern_generator(pattern_accessor, (int)m_brushOrg.x, (int)m_brushOrg.y);
				 agg::render_scanlines_aa(this->rasterizer_, this->scanline_,
					 this->renderer_base_, this->span_allocator_,
					 span_pattern_generator);
			 }
		 }

		 void CGraphicsAgg::DrawRoundRect(const CPen* pPen, const CBrush*  pBbrush, const GRect& Rect, GUnits radius){}
		 void CGraphicsAgg::DrawRect(const CPen* pPen, const CBrush*  pBbrush, const GRect& Rect){}
		 void CGraphicsAgg::DrawRect(CPen* pPen, CBrush*  pBbrush, const GPoint& LTPoint, const GPoint& RBPoint){}
		 void CGraphicsAgg::DrawRect(CPen* pPen, CBrush*  pBbrush, GUnits dLTX, GUnits dLTY, GUnits dRBX, GUnits dRBY) {}

		 void CGraphicsAgg::DrawRectEx(const CPen* pPen, const CBrush*  pBbrush, const GRect& Rect, const GPoint& originMin, const GPoint& originMax){}
		 void CGraphicsAgg::DrawEllipse(const CPen* pPen, const CBrush*  pBbrush, const GRect& Rect){}
		 void CGraphicsAgg::DrawEllipse(const CPen* pPen, const CBrush*  pBbrush, const GPoint& LTPoint, const GPoint& RBPoint){}
		 void CGraphicsAgg::DrawEllipse(const CPen* pPen, const CBrush*  pBbrush, GUnits dLTX, GUnits dLTY, GUnits dRBX, GUnits dRBY){}

		 void CGraphicsAgg::DrawPolygon(const CPen* pPen, const CBrush*  pBbrush, const GPoint* pPoints, int nNumPoints){}
		 void CGraphicsAgg::DrawPolyPolygon(const CPen* pPen, const CBrush*  pBbrush, const GPoint* lpPoints, const int *lpPolyCounts, int nCount){}

		 void CGraphicsAgg::DrawPolygonEx(const CPen* pPen, const CBrush*  pBbrush, const GPoint* pPoints, int nNumPoints, const GPoint& originMin, const GPoint& originMax){}
		 void CGraphicsAgg::DrawPolyPolygonEx(const CPen* pPen, const CBrush*  pBbrush, const GPoint* lpPoints, const int *lpPolyCounts, int nCount, const GPoint& originMin, const GPoint& originMax){}

		 void CGraphicsAgg::QueryTextMetrics(const CFont* pFont, GUnits* height, GUnits* baseLine, GUnits* lineSpacing) {}
		 void CGraphicsAgg::QueryTextMetrics(const CFont* pFont, const wchar_t* text, int len, GUnits* width, GUnits* height, GUnits* baseLine){}
		 void CGraphicsAgg::DrawText(const CFont* pFont, const wchar_t* text, int len, const GPoint& point, int drawFlags){}
		 void CGraphicsAgg::DrawTextByLine(const CFont* pFont, const wchar_t* text, int len, const GPoint* pPoints, int nNumPoints){}

		 void CGraphicsAgg::DrawBitmap(const CBitmap* bitmap, const GRect& dstRect, bool flip_y, unsigned char alpha){}
		 void CGraphicsAgg::DrawRotatedBitmap(const CBitmap* bitmap, const GPoint& center, double angle, bool flip_y, unsigned char alpha, double scale_x , double scale_y, bool clip, bool onGrid)
		 {}

		 Color CGraphicsAgg::GetPixel(GUnits x, GUnits y)
		 {
			 agg::rgba8 pixel = this->rendering_buffer_.pixel((int)x, (int)y);
			 return Color(pixel.r, pixel.g, pixel.b, pixel.a);
		 }

		 GPoint CGraphicsAgg::GetViewportOrg() const
		 {
			 return m_org;
		 }
		 void   CGraphicsAgg::SetViewportOrg(const GPoint& org)
		 {
			 m_org = org;
		 }

		 GPoint CGraphicsAgg::GetBrushOrg() const
		 {
			  return m_brushOrg;
		 }
		 void   CGraphicsAgg::SetBrushOrg(const GPoint& org)
		 {
			 m_brushOrg = org;
		 }

		 const CBitmap& CGraphicsAgg::GetSurface() const
		 {
			 return m_surface;
		 }
		 CBitmap& CGraphicsAgg::GetSurface()
		 {
			 return m_surface;
		 }

		 size_t CGraphicsAgg::GetWidth() const
		 {
			 return (size_t)m_nWidth;
		 }
		 size_t CGraphicsAgg::GetHeight() const
		 {
			 return (size_t)m_nHeight;
		 } 

		 void CGraphicsAgg::Lock(){}
		 void CGraphicsAgg::Release(){}
	}
}
