#ifndef GIS_ENGINE_DISPLAY_DISPLAY_UTILS_H
#define GIS_ENGINE_DISPLAY_DISPLAY_UTILS_H
#include "Display.h"

namespace GisEngine
{
	namespace Display
	{

	 

		GUnits SymbolSizeToDeviceSize(IDisplayTransformation *trans, double sizeSymb, bool scaleDependent);
		double DeviceSizeToSymbolSize(IDisplayTransformation *trans, GUnits sizeDev, bool scaleDependent);
		GUnits SymbolSizeToDeviceSize(double dpi, double sizeSymb);
		double DeviceSizeToSymbolSize(double dpi, GUnits sizeDev);
		void RotateCoords(const GPoint& center, double angle, GPoint* points, int count);

		inline bool is_clockwise(const GPoint* points, int pointCount)
		{
#ifdef _FLOAT_GUNITS_
			double S = 0;
#else
			int64 S = 0;
#endif
			for(int i = 0; i < pointCount - 1; i++)
				S += (points[i + 1].y + points[i].y) * (points[i + 1].x - points[i].x);
			return S >= 0;
		}

	}

}

#endif