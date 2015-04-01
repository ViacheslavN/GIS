#ifndef GIS_ENGINE_DISPLAY_GRAPHICS_H_
#define GIS_ENGINE_DISPLAY_GRAPHICS_H_
#include "CommonLibrary/GeneralTypes.h"
#include "GraphTypes.h"
#include "Bitmap.h"
#include "Common/Common.h"
#include "Point.h"
#include "Rect.h"

namespace GisEngine
{
	namespace Display
	{

		class IGraphics
		{
			public:
				IGraphics(){}
				virtual ~IGraphics(){}

				virtual DeviceType  GetDeviceType() const = 0;
				virtual void        StartDrawing() = 0;
				virtual void        EndDrawing() = 0;
				virtual GRect       GetClipRect() const = 0;
				virtual void        SetClipRect(const GRect& rect) = 0;
				virtual void        SetClipRgn(const GPoint* lpPoints, const int *lpPolyCounts, int nCount) = 0;
				virtual void        RemoveClip() = 0;
				virtual void        Erase(const Color& color, const GRect *rect = 0) = 0;
				virtual Graphics*   CreateCompatibleGraphics(GUnits width, GUnits height) = 0;
				virtual void        Copy(Graphics* src, const GPoint& srcPoint, const GRect& dstRect, bool bBlend = true) = 0;

		};
	}
}

#endif