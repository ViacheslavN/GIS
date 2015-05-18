#ifndef GIS_ENGINE_DISPLAY_AGG_RENDER_H_
#define GIS_ENGINE_DISPLAY_AGG_RENDER_H_
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
namespace GisEngine
{
	namespace Display
	{


		class vertex_src
		{
		public:
			vertex_src();
			void attach(const GPoint* points, const int* pointCounts, int count);
			void attach(agg::conv_dash<vertex_src>& dasher);
			void attach(agg::ellipse& ellipse);
			void attach(agg::rounded_rect& rounded_rect);
			void rewind(unsigned path_id);
			unsigned vertex(double *x, double *y);
			void set_offset(GUnits offsetX, GUnits offsetY);
			void get_offset(GUnits* offsetX, GUnits* offsetY);
		private:
			const GPoint*               m_pPoints;
			const int*                  m_pPointCounts;
			int                         m_nCount;
			int                         m_nIdx;
			int                         m_nCurPointCount;
			unsigned                    m_nPathID;
			agg::conv_dash<vertex_src>* m_pDasher;
			agg::ellipse*               m_pEllipse;
			agg::rounded_rect*          m_pRounded_rect;
			GUnits                      m_nOffsetX;
			GUnits                      m_nOffsetY;
		};
		template <typename VertexSrc>
		class offset_stroke
		{
		public:
			offset_stroke(VertexSrc& src)
				: m_pSrc(&src),
				m_nIdx(0),
				m_bStop(false),
				m_nOffset(0),
				m_bStart(true)
			{
			}
			void set_offset(GUnits offset)
			{
				m_nOffset = offset;
			}
		public:
			void rewind(unsigned path_id)
			{
				m_pSrc->rewind(path_id);
				m_vecPoints.clear();
				m_bStop = false;
				m_bStart = true;
			}
			unsigned vertex(double *x, double *y)
			{
				if(m_nOffset == 0.0)
					return m_pSrc->vertex(x, y);

				while(true)
				{
					if((m_vecPoints.size() > 0) && m_nIdx < m_vecPoints.size())
					{
						unsigned ret = (m_nIdx > 0) ? agg::path_cmd_line_to : agg::path_cmd_move_to;

						*x = m_vecPoints[m_nIdx].m_x;
						*y = m_vecPoints[m_nIdx].m_y;
						m_nIdx++;
						return ret;
					}
					else if(m_bStop)
					{
						m_vecPoints.clear();
						return agg::path_cmd_stop;
					}

					double vx, vy;
					unsigned cmd;
					if(m_bStart)
					{
						cmd = m_pSrc->vertex(&vx, &vy);
						assert(cmd != agg::path_cmd_stop);
						m_vecPoints.push_back(GPoint(GUnits(vx), GUnits(vy)));
						m_bStart = false;
					}
					else
					{
						m_vecPoints.clear();
						m_vecPoints.push_back(m_movePoint);
					}

					do
					{
						cmd = m_pSrc->vertex(&vx, &vy);
						m_vecPoints.push_back(GPoint(GUnits(vx), GUnits(vy)));
					} while(cmd == agg::path_cmd_line_to);

					assert(cmd == agg::path_cmd_stop || cmd == agg::path_cmd_move_to);
					if(cmd == agg::path_cmd_stop)
					{
						m_vecPoints.resize(m_vecPoints.size() - 1);
						m_bStop = true;
					}
					else
					{
						m_vecPoints.resize(m_vecPoints.size() - 1);
						m_movePoint.m_x = GUnits(vx);
						m_movePoint.m_y = GUnits(vy);
					}

					if(m_vecPoints.size() > 1)
					{
						const GPoint* points = &m_vecPoints[0];
						int pointCount = (int)m_vecPoints.size();

						conv_offset::gen(m_nOffset, &points, &pointCount, 1);
						if(points)
						{
							m_vecPoints.resize(pointCount);
							if(pointCount > 0)
								::memmove(&m_vecPoints[0], points, sizeof(GPoint) * pointCount);
						}

					}
					else
					{
						m_vecPoints.clear();
					}
					m_nIdx = 0;
				}
			}
		private:
			VertexSrc* m_pSrc;
			std::vector<GPoint> m_vecPoints;
			GPoint              m_movePoint;
			size_t   m_nIdx;
			bool     m_bStop;
			bool     m_bStart;
			GUnits   m_nOffset;
		};


		template <typename TPixelFmt>
		class AggRenderer 
		{
		public:
			 
		public:
			AggRenderer(GUnits width, GUnits height, bool flipY) :
				 
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
				rendering_buffer_(rbuf_)
			{

				//TO DO ñreate buf
				renderer_.attach(renderer_base_);
				renderer_base_.attach(this->rendering_buffer_);
			}

		protected:
			typedef agg::scanline_p8 scanline_t;
			typedef agg::renderer_base<TPixelFmt> renderer_base_t;
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

		public:
			TPixelFmt& rendering_buffer() { return  rendering_buffer_; }

		protected:
			scanline_t scanline_;
			renderer_base_t renderer_base_;
			renderer_t renderer_;
			rasterizer_t rasterizer_;
		    agg::rendering_buffer rbuf_;
			TPixelFmt	rendering_buffer_;

			agg::conv_stroke<offset_stroke<vertex_src> > conv_stroke_;
			agg::conv_dash<vertex_src> dash_stroke_;
			agg::conv_stroke<offset_stroke<agg::conv_dash<vertex_src> > > dash_conv_stroke_;
			vertex_src vertex_src_;
			offset_stroke<vertex_src>                  offset_stroke_;
			offset_stroke<agg::conv_dash<vertex_src> > dash_offset_stroke_;
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
			agg::conv_bspline<vertex_src>                    bspline_;
			agg::trans_single_path tcurve_;
			conv_font_segm_type  fsegm_;
			conv_font_trans_type ftrans_;
			trans_type_contour fcontour_;
		};
	}
}

#endif