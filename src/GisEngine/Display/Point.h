#ifndef GIS_ENGINE_DISPLAY_POINT_H_
#define GIS_ENGINE_DISPLAY_POINT_H_
#include "GraphTypes.h"
namespace GisEngine
{
	namespace Display
	{
		class GPoint
		{ 
		public:
			GPoint();
			GPoint(GUnits X, GUnits Y);
			GPoint(const GPoint& gp);

			bool    operator==(const CPoint&) const;
			bool    operator!=(const CPoint&) const;
			const GPoint &operator+=(const CPoint &);
			const GPoint &operator-=(const CPoint &);
		public:
			GPoint x, y;
		};
	}
}
#endif