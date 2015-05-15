#include "stdafx.h"
#include "Rect.h"

namespace GisEngine
{
	namespace Display
	{
		GRect::GRect()
		{
			set(0, 0, -1, -1);
		}
		GRect::GRect(const GPoint& _min, const GPoint& _max)
		{
			set(_min.m_x, _min.m_y, _max.m_x, _max.m_y);
		}
		GRect::GRect(GUnits minX, GUnits minY, GUnits maxX, GUnits maxY)
		{
			set(minX, minY, maxX, maxY);
		}
		GRect::GRect(const GRect& rc)
		{
			set(rc.m_xMin, rc.m_yMin, rc.m_xMax, rc.m_yMax);
		}


		bool GRect::operator==(const GRect& rc) const
		{
			return m_xMin == rc.m_xMin && m_xMax == rc.m_xMax && m_yMin == rc.m_yMin && m_yMax == rc.m_yMax;
		}
		bool GRect::operator!=(const GRect& rc) const
		{
			return m_xMin != rc.m_xMin || m_xMax != rc.m_xMax || m_yMin != rc.m_yMin || m_yMax != rc.m_yMax;
		}

		void GRect::set(GUnits xMin, GUnits yMin, GUnits xMax, GUnits yMax)
		{
			m_xMin = xMin;
			m_xMax = xMax;
			m_yMin = yMin;
			m_yMax = yMax;
		}
		void GRect::null()
		{ 
			set(0,0,0,0); 
		}
		GRect& GRect::offset(GUnits xOffset, GUnits yOffset)
		{
			 m_xMin += xOffset;
			 m_xMax += xOffset; 
			 m_yMin += yOffset;
			 m_yMax += yOffset;
			 return *this;
		}
		GRect& GRect::offset(const GPoint& point)
		{
			return offset(point.m_x, point.m_y); 
		}
		void GRect::inflate(GUnits dx, GUnits dy)
		{
			 m_xMin -= dx; 
			 m_xMax += dx; 
			 m_yMin -= dy; 
			 m_yMax += dy; 
		}
 
		void GRect::expandRect(const GRect &rc)
		{
			if(rc.isEmpty())
				return;

			if(isEmpty())
			{
				set(rc.m_xMin, rc.m_yMin, rc.m_xMax, rc.m_yMax);
			}
			else
				set(std::min(m_xMin, rc.m_xMin),std::min(m_yMin, rc.m_yMin), std::max(m_xMax, rc.m_xMax),std::max(m_yMax, rc.m_yMax));
		}
		void GRect::expandRect(const GPoint &p)
		{
			if(isEmpty())
			{
				set(p.m_x, p.m_y, p.m_x, p.m_y);
			}
			else
				set(std::min(m_xMin, p.m_x), std::min(m_yMin, p.m_y), std::max(m_xMax, p.m_x), std::max(m_yMax, p.m_y));
		}
		GRect& GRect::intersect(const GRect &rc)
		{
			if(this == &rc)
				return *this;

			if(isEmpty() || rc.isEmpty())
			{
				*this = GRect();
				return *this;
			}

			m_xMin= std::max(m_xMin, rc.m_xMin);
			m_yMin = std::max(m_yMin, rc.m_yMin);
			m_xMax = std::min(m_xMax, rc.m_xMax);
			m_yMax = std::min(m_yMax, rc.m_yMax);
			return *this;
		}
		bool GRect::isIntersect(const GRect &rc) const
		{
			if(isEmpty() || rc.isEmpty())
				return false;

			if(m_xMin >= rc.m_xMax) return false;
			if(m_xMax <= rc.m_xMin) return false;
			if(m_yMin >= rc.m_yMax) return false;
			if(m_yMax <= rc.m_yMin) return false;
			return true;
		}
		bool GRect::isInRect(const GRect &rc) const
		{
			if(isEmpty() || rc.isEmpty())
				return false;

			if(m_xMin > rc.m_xMin) return false;
			if(m_xMax < rc.m_xMax) return false;
			if(m_yMin > rc.m_yMin) return false;
			if(m_yMax < rc.m_yMax) return false;
			return true;
		}
		GUnits GRect::width()  const
		{
			return m_xMax - m_xMin;
		}
		GUnits  GRect::height() const
		{
			return m_yMax - m_yMin;
		}
		bool GRect::isEmpty() const
		{
			return (m_xMax < m_xMin|| (m_yMax < m_yMin)); 
		}
		bool GRect::isNull() const
		{
			return (m_xMax == m_xMin) && (m_yMax == m_yMin); 
		}
		GPoint GRect::CenterPoint() const
		{
			return GPoint((m_xMin + m_xMax)/2, (m_yMin + m_yMax)/2);
		}
		void GRect::scale(double scl) 
		{ 
			scale(scl,scl);
		}
		void GRect::scale(double sclx, double scly)
		{
			m_xMin  = (GUnits)(m_xMin * sclx);
			m_xMax  = (GUnits)(m_xMax * sclx);
			m_yMin  = (GUnits)(m_yMin * scly);
			m_yMax  = (GUnits)(m_yMax * scly);
		}
	 	
		bool GRect::pointInRect(const GPoint &p) const
		{
			return (p.m_x > m_xMin) && (p.m_y > m_yMin) &&
				(p.m_x < m_xMax) && (p.m_y < m_yMax) ;
		}

		bool GRect::pointInRectEQ(const GPoint &p) const
		{
			return (p.m_x >= m_xMin) && (p.m_y >= m_yMin) &&
				(p.m_x <= m_xMax) && (p.m_y <= m_yMax) ;
		}
	}
}