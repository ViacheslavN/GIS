#ifndef GIS_ENGINE_DISPLAY_VERTEX_SRC_H_
#define GIS_ENGINE_DISPLAY_VERTEX_SRC_H_

#include "agg/agg_ellipse.h"
#include "agg/agg_conv_dash.h"
#include "agg/agg_rounded_rect.h"
#include "Point.h"
#include "conv_offset.h"
namespace GisEngine
{
	namespace Display
	{
		class CVertexSrc
		{
		public:
			CVertexSrc();
			void attach(const GPoint* points, const int* pointCounts, int count);
			void attach(agg::conv_dash<CVertexSrc>& dasher);
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
			agg::conv_dash<CVertexSrc>* m_pDasher;
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

						*x = m_vecPoints[m_nIdx].x;
						*y = m_vecPoints[m_nIdx].y;
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
						m_movePoint.x = GUnits(vx);
						m_movePoint.y = GUnits(vy);
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
	}
}
#endif