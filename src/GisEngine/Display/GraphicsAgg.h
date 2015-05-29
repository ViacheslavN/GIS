#ifndef GIS_ENGINE_DISPLAY_GRAPHICS_AGG_H_
#define GIS_ENGINE_DISPLAY_GRAPHICS_AGG_H_
#include "Graphics.h"

#include "agg/agg_renderer_base.h"
#include "agg/agg_scanline_p.h"
#include "agg/agg_renderer_scanline.h"
#include "agg/agg_pixfmt_rgba.h"

#include "agg/agg_rasterizer_outline.h"
#include "agg/agg_renderer_primitives.h"
#include "agg/agg_rasterizer_outline_aa.h"
#include "agg/agg_renderer_outline_aa.h"
#include "agg/agg_ellipse.h"
#include "agg/agg_font_cache_manager.h"
#include "agg/agg_rasterizer_scanline_aa.h"
#include "agg/agg_conv_dash.h"
#include "agg/agg_conv_stroke.h"



#include "agg/agg_span_allocator.h"
#include "agg/agg_span_interpolator_linear.h"
#include "agg/agg_span_image_filter.h"
#include "agg/agg_image_accessors.h"
#include "agg/agg_span_image_filter_rgba.h"
#include "agg/agg_span_pattern_rgba.h"

#include "agg/agg_span_gradient.h"
#include "agg/agg_pixfmt_gray.h"
#include "agg/agg_pixfmt_amask_adaptor.h"
#include "agg/agg_pixfmt_rgb_packed.h"
#include "agg/agg_alpha_mask_u8.h"

#include "agg/agg_font_cache_manager.h"
#include "agg/agg_font_freetype.h"
#include "agg/agg_conv_contour.h"
#include "conv_offset.h"
#include "agg/agg_conv_bspline.h"
#include "agg/agg_trans_single_path.h"
#include "agg/agg_conv_segmentator.h"
#include "agg/agg_conv_transform.h"
#include "agg/agg_rounded_rect.h"
#include "VertexSrc.h"

namespace GisEngine
{
	namespace Display
	{


		class CGraphicsAgg : public IGraphics
		{
			public:
				CGraphicsAgg(unsigned char* buf, GUnits  width, GUnits height, bool bRelease, bool flipY = false);
				CGraphicsAgg( GUnits  width, GUnits height, bool flipY = false);
				~CGraphicsAgg();

				virtual DeviceType  GetDeviceType() const;
				virtual void        StartDrawing();
				virtual void        EndDrawing();
				virtual GRect       GetClipRect() const ;
				virtual void        SetClipRect(const GRect& rect);
				virtual void        SetClipRgn(const GPoint* lpPoints, const int *lpPolyCounts, int nCount);
				virtual void        RemoveClip() ;
				virtual void        Erase(const Color& color, const GRect *rect = 0) ;
				virtual IGraphics*   CreateCompatibleGraphics(GUnits width, GUnits height);
				virtual void        Copy(IGraphics* src, const GPoint& srcPoint, const GRect& dstRect, bool bBlend = true);

				virtual void DrawPoint(const CPen* pPen, const CBrush*  pBbrush, const GPoint& Pt);
				virtual void DrawPoint(const CPen* pPen, const CBrush*  pBbrush, GUnits dX, GUnits dY);
				virtual void DrawPixel(GUnits dX, GUnits dY, const Color &color);

				virtual void DrawLineSeg(const CPen* pPen, const GPoint& P1, const GPoint& P2);
				virtual void DrawLineSeg(const CPen* pPen, GUnits dX1, GUnits dY1, GUnits dX2, GUnits dY2);

				virtual void DrawLine(const CPen* pPen, const GPoint* pPoints, int nNumPoints) ;

				virtual void DrawRoundRect(const CPen* pPen, const CBrush*  pBbrush, const GRect& Rect, GUnits radius);
				virtual void DrawRect(const CPen* pPen, const CBrush*  pBbrush, const GRect& Rect);
				virtual void DrawRect(CPen* pPen, CBrush*  pBbrush, const GPoint& LTPoint, const GPoint& RBPoint);
				virtual void DrawRect(CPen* pPen, CBrush*  pBbrush, GUnits dLTX, GUnits dLTY, GUnits dRBX, GUnits dRBY) ;

				virtual void DrawRectEx(const CPen* pPen, const CBrush*  pBbrush, const GRect& Rect, const GPoint& originMin, const GPoint& originMax);
				virtual void DrawEllipse(const CPen* pPen, const CBrush*  pBbrush, const GRect& Rect);
				virtual void DrawEllipse(const CPen* pPen, const CBrush*  pBbrush, const GPoint& LTPoint, const GPoint& RBPoint);
				virtual void DrawEllipse(const CPen* pPen, const CBrush*  pBbrush, GUnits dLTX, GUnits dLTY, GUnits dRBX, GUnits dRBY);

				virtual void DrawPolygon(const CPen* pPen, const CBrush*  pBbrush, const GPoint* pPoints, int nNumPoints);
				virtual void DrawPolyPolygon(const CPen* pPen, const CBrush*  pBbrush, const GPoint* lpPoints, const int *lpPolyCounts, int nCount);

				virtual void DrawPolygonEx(const CPen* pPen, const CBrush*  pBbrush, const GPoint* pPoints, int nNumPoints, const GPoint& originMin, const GPoint& originMax);
				virtual void DrawPolyPolygonEx(const CPen* pPen, const CBrush*  pBbrush, const GPoint* lpPoints, const int *lpPolyCounts, int nCount, const GPoint& originMin, const GPoint& originMax);

				virtual void QueryTextMetrics(const CFont* pFont, GUnits* height, GUnits* baseLine, GUnits* lineSpacing) ;
				virtual void QueryTextMetrics(const CFont* pFont, const wchar_t* text, int len, GUnits* width, GUnits* height, GUnits* baseLine);
				virtual void DrawText(const CFont* pFont, const wchar_t* text, int len, const GPoint& point, int drawFlags = TextDrawAll);
				virtual void DrawTextByLine(const CFont* pFont, const wchar_t* text, int len, const GPoint* pPoints, int nNumPoints);

				virtual void DrawBitmap(const CBitmap* bitmap, const GRect& dstRect, bool flip_y, unsigned char alpha = 255);
				virtual void DrawRotatedBitmap(const CBitmap* bitmap, const GPoint& center, double angle, bool flip_y = false, unsigned char alpha = 255, double scale_x = 1.0, double scale_y = 1.0, bool clip = true, bool onGrid = false);

				virtual Color GetPixel(GUnits x, GUnits y);

				virtual GPoint GetViewportOrg() const;
				virtual void   SetViewportOrg(const GPoint& org);

				virtual GPoint GetBrushOrg() const ;
				virtual void   SetBrushOrg(const GPoint& org);

				virtual const CBitmap& GetSurface() const;
				virtual CBitmap& GetSurface();

				virtual size_t GetWidth() const;
				virtual size_t GetHeight() const; 

				virtual void Lock();
				virtual void Release();

				HDC GetDC();
			private:
#ifdef ANDROID
				typedef agg::pixfmt_rgba32 pixfmt_t; 
#else
				typedef agg::pixfmt_bgra32 pixfmt_t;
#endif
				typedef agg::scanline_p8 scanline_t;
 				typedef agg::renderer_base<pixfmt_t> renderer_base_t;
				typedef agg::renderer_scanline_aa_solid<renderer_base_t> renderer_t;
				typedef agg::rasterizer_scanline_aa<> rasterizer_t;

				// images support
				typedef agg::span_allocator<agg::rgba8> span_allocator_t;
				typedef agg::wrap_mode_repeat pattern_wrap_t;
				typedef agg::image_accessor_wrap<agg::pixfmt_bgra32, pattern_wrap_t, pattern_wrap_t> pattern_accessor_t;
				typedef agg::span_pattern_rgba<pattern_accessor_t> span_pattern_generator_t;

				typedef agg::font_engine_freetype_int32 font_engine_t;
				typedef agg::font_cache_manager<font_engine_t> font_cache_manager_t;
				typedef agg::conv_curve<font_cache_manager_t::path_adaptor_type> conv_curve_t;
				typedef agg::conv_contour<conv_curve_t> conv_contour_type;
				typedef agg::conv_segmentator<conv_curve_t>                      conv_font_segm_type;
				typedef agg::conv_transform<conv_font_segm_type, agg::trans_single_path> conv_font_trans_type;
				typedef agg::conv_contour<conv_font_trans_type>                          trans_type_contour;

		private:
			void InCopy(const GPoint& srcPoint, const GRect& dstRect);
			void copy(IGraphics* pSrc, const GPoint& srcPoint, const GRect& dstRect, bool bBlend = true);
			void draw_line(const CPen* pPen);
			void draw_poly_polygon(const CBrush* pBrush, const GPoint& originMin = GPoint(), const GPoint& originMax = GPoint());
		private:
				scanline_t scanline_;
				renderer_base_t renderer_base_;
				renderer_t renderer_;
				rasterizer_t rasterizer_;
				agg::rendering_buffer rbuf_;
				pixfmt_t	rendering_buffer_;

				agg::conv_stroke<offset_stroke<CVertexSrc> > conv_stroke_;
				agg::conv_dash<CVertexSrc> dash_stroke_;
				agg::conv_stroke<offset_stroke<agg::conv_dash<CVertexSrc> > > dash_conv_stroke_;
				CVertexSrc vertex_src_;
				offset_stroke<CVertexSrc>                  offset_stroke_;
				offset_stroke<agg::conv_dash<CVertexSrc> > dash_offset_stroke_;
				agg::ellipse ellipse_;
				agg::rounded_rect rounded_rect_;


				//images support
				span_allocator_t         span_allocator_;
				agg::pixfmt_bgra32       pattern_rgba32_;
				agg::rendering_buffer    pattern_rbuf_;
				span_pattern_generator_t span_pattern_generator_;


				font_engine_t        font_engine_;
				font_cache_manager_t font_manager_;
				conv_curve_t         curves_;
				conv_contour_type    contour_;
				agg::conv_bspline<CVertexSrc>       bspline_;
				agg::trans_single_path tcurve_;
				conv_font_segm_type  fsegm_;
				conv_font_trans_type ftrans_;
				trans_type_contour fcontour_;
				CBitmap m_surface;


				GUnits m_nWidth;
				GUnits m_nHeight;
				HDC m_dc;
#ifndef ANDROID
				HBITMAP m_oldBitmap;
#endif

				GRect m_ClipRect;
				bool m_bflipY;
				GPoint m_org;
				GPoint m_brushOrg;

		};
	}
}

#endif