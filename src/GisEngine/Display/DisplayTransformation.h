#ifndef GIS_ENGINE_DISPLAY_TRANSFORMATION_H_
#define GIS_ENGINE_DISPLAY_TRANSFORMATION_H_

#include "Common/Common.h"
#include "Common/Units.h"
#include "DisplayTypes.h"
#include "Point.h"
#include "Rect.h"
#include "GisGeometry/ISpatialReference.h"
namespace GisEngine
{
	namespace Display
	{
		class IDisplayTransformation
		{
			public:
				IDisplayTransformation(){}
				virtual ~IDisplayTransformation(){}

				virtual void SetMapPos(const GisXYPoint &map_pos, double new_scale) = 0;
				virtual const GisXYPoint& GetMapPos() const= 0;

				virtual void SetMapVisibleRect(const GisBoundingBox& bound) = 0;
				virtual const GisBoundingBox& GetFittedBounds() const = 0;

				virtual void         SetDeviceRect(const GRect& bound, DisplayTransformationPreserve preserve_type = DisplayTransformationPreserveCenterExtent) = 0;
				virtual const GRect& GetDeviceRect() const = 0;

				virtual void SetDeviceClipRect(const GRect& devRect) = 0;
				virtual const GRect& GetDeviceClipRect() const  = 0;

				virtual void SetReferenceScale(double dScale)= 0;
				virtual double GetReferenceScale() const= 0;
				virtual bool UseReferenceScale() const = 0;

				virtual  double GetScale() const = 0;
				virtual  void SetRotation(double degrees ) = 0;
				virtual  double GetRotation() = 0;


				virtual void SetResolution(double pDpi) = 0;
				virtual double GetResolution()= 0;

				virtual void SetUnits(Common::Units units) = 0;
				virtual Common::Units GetUnits() const = 0;


				virtual void   SetSpatialReference(Geometry::ISpatialReference *pSp) = 0;
				virtual Geometry::ISpatialReference* GetSpatialReference() const = 0;

	 
				virtual void MapToDevice(const GisXYPoint *pIn, GPoint *pOut, int nPoints) = 0;

				virtual void MapToDevice(const CommonLib::CGeoShape& geom, GPoint **pOut, int** partCounts, int* count) = 0;
				virtual void DeviceToMap(const GPoint *pIn,  GisXYPoint *pOut, int nPoints) = 0;
				virtual void MapToDevice(const GisBoundingBox& mapBox, GRect& rect) = 0;
				virtual void DeviceToMap(const GRect& rect, GisBoundingBox& mapBox) = 0;

				virtual double DeviceToMapMeasure(double deviceLen) = 0;
				virtual double MapToDeviceMeasure(double mapLen) = 0;


				virtual void SetVerticalFlip(bool flag) = 0;
				virtual bool GetVerticalFlip() const = 0;
				virtual void SetHorizontalFlip(bool flag) = 0;
				virtual bool GetHorizontalFlip() const = 0;


				virtual bool GetEnable3D() const = 0;
				virtual void SetEnable3D(bool enable) = 0;

				virtual const GRect& GetClipRect() const = 0;
				virtual void  SetClipRect(const GRect& rect) = 0;
				virtual bool  ClipExists() = 0;
				virtual void  RemoveClip() = 0;
		};
	}
}

#endif