#ifndef GIS_ENGINE_DISPLAY_TRANSFORMATION_2D_H_
#define GIS_ENGINE_DISPLAY_TRANSFORMATION_2D_H_
#include "Display.h"
namespace GisEngine
{
	namespace Display
	{

		class CDisplayTransformation2D : IDisplayTransformation
		{
			public:

				CDisplayTransformation2D(double resolution, GisCommon::Units units, const GRect &dev_rect = GRect(), double scale = 1.0);
				~CDisplayTransformation2D();

				virtual void SetMapPos(const GisXYPoint &map_pos, double new_scale);
				virtual GisXYPoint GetMapPos() const;

				virtual void SetMapVisibleRect(const GisBoundingBox& bound);
				virtual const GisBoundingBox& GetFittedBounds() const;

				virtual void         SetDeviceRect(const GRect& bound, eDisplayTransformationPreserve preserve_type = DisplayTransformationPreserveCenterExtent);
				virtual const GRect& GetDeviceRect() const;

				virtual void SetDeviceClipRect(const GRect& devRect);
				virtual const GRect& GetDeviceClipRect() const ;

				virtual void SetReferenceScale(double dScale);
				virtual double GetReferenceScale() const;
				virtual bool UseReferenceScale() const;

				virtual  double GetScale() const;
				virtual  void SetRotation(double degrees );
				virtual  double GetRotation();


				virtual void SetResolution(double pDpi);
				virtual double GetResolution();

				virtual void SetUnits(GisCommon::Units units);
				virtual GisCommon::Units GetUnits() const;


				virtual void   SetSpatialReference(GisGeometry::ISpatialReference *pSp);
				virtual GisGeometry::ISpatialReferencePtr GetSpatialReference() const;


				virtual void MapToDevice(const GisXYPoint *pIn, GPoint *pOut, int nPoints);
				virtual void MapToDevice(const CommonLib::CGeoShape& geom, GPoint **pOut, int** partCounts, int* count);
				virtual void MapToDevice(const GisBoundingBox& mapBox, GRect& rect);
				virtual int MapToDeviceOpt(const GisXYPoint *pIn, GPoint *pOut, int nPoints, CommonLib::eShapeType);
				virtual void MapToDeviceOpt(const GisXYPoint pIn, GPoint& pOut);
			 
 

				virtual void DeviceToMap(const GPoint *pIn,  GisXYPoint *pOut, int nPoints);
				virtual void DeviceToMap(const GRect& rect, GisBoundingBox& mapBox);

				virtual double DeviceToMapMeasure(double deviceLen);
				virtual double MapToDeviceMeasure(double mapLen);


				virtual void SetVerticalFlip(bool flag);
				virtual bool GetVerticalFlip() const;
				virtual void SetHorizontalFlip(bool flag);
				virtual bool GetHorizontalFlip() const;


				virtual bool GetEnable3D() const;
				virtual void SetEnable3D(bool enable);
				virtual void SetAngle3D(double dAndle);
				virtual double GetAngle3D() const ;

				virtual const GRect& GetClipRect() const;
				virtual void  SetClipRect(const GRect& rect);
				virtual bool  ClipExists();
				virtual void  RemoveClip();

				virtual void SetClipper(IClip *pCLip);
				virtual IClipPtr GetClipper() const;


				virtual void SetOnDeviceFrameChanged(OnDeviceFrameChanged* pFunck, bool bAdd);
				virtual void SetOnResolutionChanged(OnResolutionChanged* pFunck, bool bAdd);
				virtual void SetOnRotationChanged(OnRotationChanged* pFunck, bool bAdd);
				virtual void SetOnUnitsChanged(OnUnitsChanged* pFunck, bool bAdd);
				virtual void SetOnVisibleBoundsChanged(OnVisibleBoundsChanged* pFunck, bool bAdd);

			private:
				 


				void SetClientRect(const GRect &arg);
				void UpdateScaleRatio();
				double CalcMapUnitPerInch();
				void SetMatrix();
				void UpdateFittedBounds();

				void rasterize3D (GUnits &x, GUnits &y);
				void unrasterize3D (GUnits &x, GUnits &y);
			private:
				GRect m_devClipRect;
				GRect m_clipRect;
				bool m_bClipExists;
				GisBoundingBox m_mapCurFittedExtent;
				GRect m_ClientRect; 
				double m_dRefScale; 
				double m_dCurScale;
				double m_dScaleRatio; 
				double m_dResolution; 
				GisCommon::Units m_mapUnits;
				double m_dAngle;
				GisGeometry::ISpatialReferencePtr m_pSpatialRef;
	
				double     m_MatrixDev2Map[2][2];
				double     m_MatrixMap2Dev[2][2];

				GUnits m_AnchorDev[2];
				double m_AnchorMap[2];


				bool m_bVerticalFlip;
				bool m_bHorizontalFlip;
				
				
				bool m_bPseudo3D;
				double m_dAngle3D;
				double m_dSin;
				double m_dCos;

				CommonLib::Event1<IDisplayTransformation*>         OnDeviceFrameChangedEvent;
				CommonLib::Event1<IDisplayTransformation*>         OnResolutionChangedEvent;
				CommonLib::Event1<IDisplayTransformation*>         OnRotationChangedEvent;
				CommonLib::Event1<IDisplayTransformation*>         OnUnitsChangedEvent;
				CommonLib::Event1<IDisplayTransformation*>         OnVisibleBoundsChangedEvent;

				std::vector<GPoint> m_vecAlloc; //to do set alloc
				std::vector<int> m_vecPart;

				IClipPtr m_pClipper;

		};
	}
}

#endif