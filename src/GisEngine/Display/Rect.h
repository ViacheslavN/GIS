#ifndef GIS_ENGINE_DISPLAY_RECT_H_
#define GIS_ENGINE_DISPLAY_RECT_H_

#include "GraphTypes.h"
#include "Point.h"
namespace GisEngine
{
	namespace Display
	{

		class GRect 
		{ 
		public:
			GRect();
			GRect(const GPoint& _min, const GPoint& _max);
			GRect(GUnits minX, GUnits minY, GUnits maxX, GUnits maxY);
			GRect(const GRect&);


			bool operator==(const GRect&) const;
			bool operator!=(const GRect&) const;

			void set(GUnits minX, GUnits minY, GUnits maxX, GUnits maxY);
			GRect& offset(GUnits xOffset, GUnits yOffset);
			GRect& offset(const GPoint& point);
			void inflate(GUnits dx, GUnits dy);
			void null() { set(0,0,0,0); } 
			void expandRect(const GRect &a);
			void expandRect(const GPoint &p);
			GRect& intersect(const GRect &a);
			bool IsIntersect(const GRect &a) const;
			bool IsInRect(const GRect &a) const;
			GUnits  width()  const;
			GUnits  height() const;
			bool isEmpty() const;
			bool isNull() const;
			GPoint CenterPoint() const;
			void scale(double scl) { scale(scl,scl); }
			void scale(double sclx, double scly);
			
			bool     pointInRect(const GPoint &p) const;

		public:
			GPoint Min;
			GPoint Max;    
		};
	}
}
#endif