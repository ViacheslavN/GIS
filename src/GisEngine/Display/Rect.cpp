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
			set(_min.x, _min.y, _max.x, _max.y);
		}
		GRect::GRect(GUnits minX, GUnits minY, GUnits maxX, GUnits maxY)
		{
			set(minX, minY, maxX, maxY);
		}
		GRect::GRect(const GRect& rc)
		{
			set(rc.xMin, rc.yMin, rc.xMax, rc.yMax);
		}


		bool GRect::operator==(const GRect& rc) const
		{
			return xMin == rc.xMin && xMax == rc.xMax && yMin == rc.yMin && yMax == rc.yMax;
		}
		bool GRect::operator!=(const GRect& rc) const
		{
			return xMin != rc.xMin || xMax != rc.xMax || yMin != rc.yMin || yMax != rc.yMax;
		}

		void GRect::set(GUnits _xMin, GUnits _yMin, GUnits _xMax, GUnits _yMax)
		{
			xMin = _xMin;
			yMin = _yMin;
			xMax = _xMax;
			yMax = _yMax;
		}
		void GRect::null()
		{ 
			set(0,0,0,0); 
		}
		GRect& GRect::offset(GUnits xOffset, GUnits yOffset)
		{
			 xMin += xOffset;
			 xMax += xOffset; 
			 yMin += yOffset;
			 yMax += yOffset;
			 return *this;
		}
		GRect& GRect::offset(const GPoint& point)
		{
			return offset(point.x, point.y); 
		}
		void GRect::inflate(GUnits dx, GUnits dy)
		{
			 xMin -= dx; 
			 xMax += dx; 
			 yMin -= dy; 
			 yMax += dy; 
		}
 
		void GRect::expandRect(const GRect &rc)
		{
			if(rc.isEmpty())
				return;

			if(isEmpty())
			{
				set(rc.xMin, rc.yMin, rc.xMax, rc.yMax);
			}
			else
				set(min(xMin, rc.xMin),min(yMin, rc.yMin), max(xMax, rc.xMax),max(yMax, rc.yMax));
		}
		void GRect::expandRect(const GPoint &p)
		{
			if(isEmpty())
			{
				set(p.x, p.y, p.x, p.y);
			}
			else
				set(min(xMin, p.x), min(yMin, p.y), max(xMax, p.x), max(yMax, p.y));
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

			xMin= max(xMin, rc.xMin);
			yMin = max(yMin, rc.yMin);
			xMax = min(xMax, rc.xMax);
			yMax = min(yMax, rc.yMax);
			return *this;
		}
		bool GRect::isIntersect(const GRect &rc) const
		{
			if(isEmpty() || rc.isEmpty())
				return false;

			if(xMin >= rc.xMax) return false;
			if(xMax <= rc.xMin) return false;
			if(yMin >= rc.yMax) return false;
			if(yMax <= rc.yMin) return false;
			return true;
		}
		bool GRect::isInRect(const GRect &rc) const
		{
			if(isEmpty() || rc.isEmpty())
				return false;

			if(xMin > rc.xMin) return false;
			if(xMax < rc.xMax) return false;
			if(yMin > rc.yMin) return false;
			if(yMax < rc.yMax) return false;
			return true;
		}
		GUnits GRect::width()  const
		{
			return xMax - xMin;
		}
		GUnits  GRect::height() const
		{
			return yMax - yMin;
		}
		bool GRect::isEmpty() const
		{
			return (xMax < xMin|| (yMax < yMin)); 
		}
		bool GRect::isNull() const
		{
			return (xMax == xMin) && (yMax == yMin); 
		}
		GPoint GRect::CenterPoint() const
		{
			return GPoint((xMin + xMax)/2, (yMin + yMax)/2);
		}
		void GRect::scale(double scl) 
		{ 
			scale(scl,scl);
		}
		void GRect::scale(double sclx, double scly)
		{
			xMin  = (GUnits)(xMin * sclx);
			xMax  = (GUnits)(xMax * sclx);
			yMin  = (GUnits)(yMin * scly);
			yMax  = (GUnits)(yMax * scly);
		}
	 	
		bool GRect::pointInRect(const GPoint &p) const
		{
			return (p.x > xMin) && (p.y > yMin) &&
				(p.x < xMax) && (p.y < yMax) ;
		}

		bool GRect::pointInRectEQ(const GPoint &p) const
		{
			return (p.x >= xMin) && (p.y >= yMin) &&
				(p.x <= xMax) && (p.y <= yMax) ;
		}
	}
}