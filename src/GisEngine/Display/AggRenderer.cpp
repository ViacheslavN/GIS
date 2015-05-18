#include "stdafx.h"
#include "AggRenderer.h"
namespace GisEngine
{
	namespace Display
	{
		vertex_src::vertex_src()
			: m_pDasher(0),
			m_pEllipse(0),
			m_pRounded_rect(0),
			m_nOffsetX(0),
			m_nOffsetY(0)
		{
		}

		void vertex_src::attach(const GPoint* pPoints, const int* pPointCounts, int count)
		{
			m_pDasher = 0;
			m_pPoints = pPoints;
			m_pEllipse = 0;
			m_pRounded_rect = 0;
			m_pPointCounts = pPointCounts;
			m_nIdx= 0;
			m_nPathID = 0;
			m_nCurPointCount = 0;
			m_nCount = count;
		}

		void vertex_src::attach(agg::ellipse& ellipse)
		{
			m_pEllipse = &ellipse;
			m_pRounded_rect = 0;
		}
		void vertex_src::attach(agg::rounded_rect& rounded_rect)
		{
			m_pRounded_rect = &rounded_rect;
			m_pEllipse = 0;
		}
		void vertex_src::rewind(unsigned path_id)
		{
			if(m_pEllipse)
			{
				m_pEllipse->rewind(path_id);
				return;
			}

			if(m_pRounded_rect)
			{
				m_pRounded_rect->rewind(path_id);
				return;
			}

			if(m_pDasher)
			{
				m_pDasher->rewind(path_id);
				return;
			}

			if((int)path_id >= m_nCount)
				return;

			m_nIdx = 0;
			unsigned i;
			for(i = 0; i < path_id; i++)
				m_nIdx += m_pPointCounts[i];
			m_nCurPointCount = m_pPointCounts[path_id];
			m_nPathID = path_id;
		}

		unsigned vertex_src::vertex(double *x, double *y)
		{
			if(m_pEllipse)
			{
				double x_, y_;
				unsigned ret = m_pEllipse->vertex(&x_, &y_);
				*x = x_ + m_nOffsetX;
				*y = y_ + m_nOffsetY;
				return ret;
			}

			if(m_pRounded_rect)
			{
				double x_, y_;
				unsigned ret = m_pRounded_rect->vertex(&x_, &y_);
				*x = x_ + m_nOffsetX;
				*y = y_ + m_nOffsetY;
				return ret;
			}

			if(m_pDasher)
			{
				double x_, y_;
				unsigned ret  = m_pDasher->vertex(&x_, &y_);
				*x = x_ + m_nOffsetX;
				*y = y_ + m_nOffsetY;
				return ret;
			}

			unsigned ret;
			if(m_nCurPointCount == 0) // end of part
			{
				if(m_nPathID == m_nCount - 1) // end of shape
				{
					return agg::path_cmd_stop;
				}
				else
				{
					m_nPathID++;
					m_nCurPointCount = m_pPointCounts[m_nPathID];
					ret = agg::path_cmd_move_to;
				}
			}
			else if(m_nCurPointCount == m_pPointCounts[m_nPathID]) // part begin
				ret = agg::path_cmd_move_to;
			else
				ret = agg::path_cmd_line_to;
 
			*x = m_pPoints[m_nIdx].m_x + m_nOffsetX + 0.5;
			*y = m_pPoints[m_nIdx].m_y + m_nOffsetY + 0.5;
 
			m_nIdx++;
			m_nCurPointCount--;
			return ret;
		}

		void vertex_src::set_offset(GUnits offsetX, GUnits offsetY)
		{
			m_nOffsetX = offsetX;
			m_nOffsetY = offsetY;
		}

		void vertex_src::get_offset(GUnits* offsetX, GUnits* offsetY)
		{
			*offsetX = m_nOffsetY;
			*offsetY = m_nOffsetY;
		}
	}
}
