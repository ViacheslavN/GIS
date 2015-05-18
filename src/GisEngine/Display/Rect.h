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

			void set(GUnits xMin, GUnits yMin, GUnits xMax, GUnits yMax);
			GRect& offset(GUnits xOffset, GUnits yOffset);
			GRect& offset(const GPoint& point);
			void inflate(GUnits dx, GUnits dy);
			void null();
			void expandRect(const GRect &rc);
			void expandRect(const GPoint &p);
			GRect& intersect(const GRect &rc);
			bool isIntersect(const GRect &rc) const;
			bool isInRect(const GRect &rc) const;
			GUnits  width()  const;
			GUnits  height() const;
			bool isEmpty() const;
			bool isNull() const;
			GPoint CenterPoint() const;
			void scale(double scl) ;
			void scale(double sclx, double scly);
			bool pointInRect(const GPoint &p) const;
			bool GRect::pointInRectEQ(const GPoint &p) const;

		public:
			GUnits xMin;
			GUnits yMin;
			GUnits xMax;
			GUnits yMax;
		};
	}
}
#endif