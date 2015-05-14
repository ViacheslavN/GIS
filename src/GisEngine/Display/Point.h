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

			bool    operator==(const GPoint&) const;
			bool    operator!=(const GPoint&) const;
			const GPoint &operator+=(const GPoint &);
			const GPoint &operator-=(const GPoint &);
		public:
			GUnits m_x, m_y;
		};
	}
}
#endif