#ifndef GIS_ENGINE_DISPLAY_DISPLAY_H_
#define GIS_ENGINE_DISPLAY_DISPLAY_H_

#include "Graphics.h"
#include "GraphTypes.h"
#include "Bitmap.h"
#include "Brush.h"
#include "Pen.h"
#include "Rect.h"
#include "DisplayTypes.h"
#include "CommonLibrary/IRefCnt.h"
#include "CommonLibrary/GeoShape.h"
#include "Common/GisEngineCommon.h"
#include "GisGeometry/Geometry.h"
#include "CommonLibrary/delegate.h"

namespace GisEngine
{
	namespace Display
	{


		enum eSymbolID
		{
			UndefineSymbolID,
			SimpleLineSymbolID,
			MarkerLineSymboID,
			HashLineSymbolID,
			MarkerLineSymbolID,
			PictureLineSymbol,
			SimpleFillSymbolID,
			LineFillSymbolID,
			TextFillSymbolID,
			TextSymbolID,
			MultiLayerSymbolID
		};

	

	

		struct ISymbol;
		struct ILineSymbol;
		struct IMarkerSymbol;
		struct ITextBackground;
		struct ITextSymbol;
		struct IFillSymbol;
		struct IDisplay;
		struct IClip;
		struct IDisplayTransformation;

		COMMON_LIB_REFPTR_TYPEDEF(ISymbol);
		COMMON_LIB_REFPTR_TYPEDEF(ILineSymbol);
		COMMON_LIB_REFPTR_TYPEDEF(IMarkerSymbol);
		COMMON_LIB_REFPTR_TYPEDEF(ITextBackground);
		COMMON_LIB_REFPTR_TYPEDEF(ITextSymbol);
		COMMON_LIB_REFPTR_TYPEDEF(IFillSymbol);
		COMMON_LIB_REFPTR_TYPEDEF(IDisplay);
		COMMON_LIB_REFPTR_TYPEDEF(IClip);
		COMMON_LIB_REFPTR_TYPEDEF(IDisplayTransformation);


		struct ViewPosition
		{
			ViewPosition() :  m_dScale(0), m_dRotation(0)
			{
				m_Center.x = 0.;
				m_Center.y = 0.;
			}
			 ViewPosition(const GisXYPoint &center, double scale, double rotation ) :
					m_Center(center), m_dScale(scale), m_dRotation(rotation){}
			~ViewPosition(){}
			GisXYPoint m_Center;
			double m_dScale;
			double m_dRotation;
		};
		struct  IDisplay : CommonLib::AutoRefCounter
		{
			IDisplay(){}
			virtual ~IDisplay(){}

			virtual void SetClipGeometry( const CommonLib::CGeoShape& clipGeom )= 0;
			virtual void SetClipRect( const GisBoundingBox& clipGeom )= 0;

			virtual void StartDrawing( IGraphics *pGraphics )= 0;
			virtual void FinishDrawing()= 0;
			virtual IGraphicsPtr GetGraphics() = 0;

			virtual IDisplayTransformationPtr GetTransformation() = 0;
			virtual void SetTransformation( IDisplayTransformation* pDisplayTransformation )= 0;

			virtual void Lock() = 0;
			virtual void UnLock() = 0;
		};

		typedef CommonLib::delegate1_t<IDisplayTransformation*>         OnDeviceFrameChanged;
		typedef CommonLib::delegate1_t<IDisplayTransformation*>         OnResolutionChanged;
		typedef CommonLib::delegate1_t<IDisplayTransformation*>         OnRotationChanged;
		typedef CommonLib::delegate1_t<IDisplayTransformation*>         OnUnitsChanged;
		typedef CommonLib::delegate1_t<IDisplayTransformation*>         OnVisibleBoundsChanged;


		struct IDisplayTransformation : public CommonLib::AutoRefCounter
		{
		public:
			IDisplayTransformation(){}
			virtual ~IDisplayTransformation(){}

			virtual void SetMapPos(const GisXYPoint &map_pos, double new_scale) = 0;
			virtual GisXYPoint GetMapPos() const= 0;

			virtual void SetMapVisibleRect(const GisBoundingBox& bound) = 0;
			virtual const GisBoundingBox& GetFittedBounds() const = 0;

			virtual void         SetDeviceRect(const GRect& bound, eDisplayTransformationPreserve preserve_type = DisplayTransformationPreserveCenterExtent) = 0;
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

			virtual void SetUnits(GisCommon::Units units) = 0;
			virtual GisCommon::Units GetUnits() const = 0;


			virtual void   SetSpatialReference(GisGeometry::ISpatialReference *pSp) = 0;
			virtual GisGeometry::ISpatialReferencePtr GetSpatialReference() const = 0;


			virtual void MapToDevice(const GisXYPoint *pIn, GPoint *pOut, int nPoints) = 0;
			virtual void MapToDevice(const CommonLib::CGeoShape& geom, GPoint **pOut, int** partCounts, int* count) = 0;
			virtual void MapToDevice(const GisBoundingBox& mapBox, GRect& rect) = 0;

			virtual int MapToDeviceOpt(const GisXYPoint *pIn, GPoint *pOut, int nPoints, CommonLib::eShapeType) = 0;



			virtual void DeviceToMap(const GPoint *pIn,  GisXYPoint *pOut, int nPoints) = 0;
			virtual void DeviceToMap(const GRect& rect, GisBoundingBox& mapBox) = 0;

			virtual double DeviceToMapMeasure(double deviceLen) = 0;
			virtual double MapToDeviceMeasure(double mapLen) = 0;


			virtual void SetVerticalFlip(bool flag) = 0;
			virtual bool GetVerticalFlip() const = 0;
			virtual void SetHorizontalFlip(bool flag) = 0;
			virtual bool GetHorizontalFlip() const = 0;


			virtual bool GetEnable3D() const = 0;
			virtual void SetEnable3D(bool enable) = 0;
			virtual void SetAngle3D(double dAndle) = 0;
			virtual double GetAngle3D() const = 0;

			virtual const GRect& GetClipRect() const = 0;
			virtual void  SetClipRect(const GRect& rect) = 0;
			virtual bool  ClipExists() = 0;
			virtual void  RemoveClip() = 0;

			virtual void SetClipper(IClip *pClip) = 0;
			virtual IClipPtr GetClipper() const = 0;

			virtual void SetOnDeviceFrameChanged(OnDeviceFrameChanged* pFunck, bool bAdd) = 0;
			virtual void SetOnResolutionChanged(OnResolutionChanged* pFunck, bool bAdd) = 0;
			virtual void SetOnRotationChanged(OnRotationChanged* pFunck, bool bAdd) = 0;
			virtual void SetOnUnitsChanged(OnUnitsChanged* pFunck, bool bAdd) = 0;
			virtual void SetOnVisibleBoundsChanged(OnVisibleBoundsChanged* pFunck, bool bAdd) = 0;
		};

		struct IClip : CommonLib::AutoRefCounter
		{
			public:
				IClip(){}
				virtual ~IClip(){}
				virtual int  clipLine(const GRect& clipper, GPoint* pBeg, GPoint* pEnd) = 0;
				virtual void clipLine(const GRect& clipper, GPoint** ppPoints, int** ppnPointCounts, int* pCount) = 0;
				virtual void clipRing(const GRect& clipper, GPoint** ppPoints, int* pPointCount) = 0;
				virtual void clipPolygon(const GRect& clipper, GPoint** ppPoints, int** ppPointCounts, int* pCount) = 0;
		/*		virtual void clipPolygonByPolygon(GPoint** ppClipPoints, int** ppClipPointCounts, int* pClipCount, 
										GPoint** points, int** pointCounts, int* count) = 0;*/
		};

		struct  ISymbol : public CommonLib::AutoRefCounter , 
						  public GisCommon::IStreamSerialize, 
						  public GisCommon::IXMLSerialize
		{
			ISymbol(){}
			virtual ~ISymbol(){}
			
			virtual uint32 GetSymbolID() const = 0;
			virtual void Init( IDisplay* display  ) = 0;
			virtual void Reset() = 0;
			virtual bool CanDraw(CommonLib::CGeoShape* pShape) const = 0;
			virtual void Draw(IDisplay* display, CommonLib::CGeoShape* pShape) = 0;
		    virtual void FlushBuffers(IDisplay* display, GisCommon::ITrackCancel* trackCancel) = 0;
			virtual void GetBoundaryRect(CommonLib::CGeoShape* pShape, IDisplay* display,  GRect &rect) const = 0;
			virtual bool GetScaleDependent() const = 0;
			virtual void SetScaleDependent(bool flag) = 0;
			virtual bool GetDrawToBuffers() const = 0;
			virtual void SetDrawToBuffers(bool flag) = 0;
			virtual void DrawDirectly(IDisplay* display, const GPoint* lpPoints, const int *lpPolyCounts, int nCount ) = 0;

			virtual void  DrawGeometryEx(IDisplay* pDisplay, const GPoint* points, const int* polyCounts, size_t polyCount) = 0;
			virtual void  QueryBoundaryRectEx(IDisplay* pDisplay, const GPoint* points, const int* polyCounts, size_t polyCount,   GRect &rect) const= 0;
			virtual void  Prepare(IDisplay* pDisplay) = 0;
		};

		struct  IMultiLayerSymbol 
		{
			IMultiLayerSymbol(){}
			virtual ~IMultiLayerSymbol(){}
			virtual int               AddLayer( ISymbol *pSym ) = 0;
			virtual void              DeleteLayer( int index ) = 0;
			virtual void              ClearLayers() = 0;
			virtual int               GetCount() const = 0;
			virtual ISymbolPtr        GetLayer( int index ) const  = 0;
			virtual void              MoveLayer( int indexFrom, int indexTo ) = 0;
			virtual bool              GetUseCache() const = 0;
			virtual void              SetUseCache(bool use) = 0;
		};

		struct ILineSymbol  : public ISymbol
		{
			ILineSymbol(){}
			virtual ~ILineSymbol(){}
			virtual Color  GetColor() const = 0;
			virtual void   SetColor(const Color &color) = 0;
			virtual double GetWidth() const = 0;  
			virtual void   SetWidth(double width) = 0;
			
		};

		struct  ISimpleLineSymbol  : public ILineSymbol 
		{
			ISimpleLineSymbol(){}
			virtual ~ISimpleLineSymbol(){}
			virtual ePenType				 GetStyle() const = 0;
			virtual void                SetStyle( ePenType style ) = 0;
			virtual eCapType             GetCapType() const = 0;
			virtual void                SetCapType( eCapType cap ) = 0;
			virtual eJoinType            GetJoinType() const = 0;
			virtual void                SetJoinType( eJoinType join ) = 0;
		};

		struct  IMarkerSymbol : public ISymbol
		{

			IMarkerSymbol(){}
			virtual ~IMarkerSymbol(){}
			virtual double GetAngle() const = 0;
			virtual void SetAngle(double angle) = 0;

			virtual Color GetColor() const = 0;
			virtual void SetColor(const Color &color) = 0;

			virtual double GetSize() const = 0;
			virtual void SetSize(double size) = 0;

			virtual double GetXOffset() const = 0;
			virtual void SetXOffset(double offset) = 0;

			virtual double GetYOffset() const = 0;
			virtual void   SetYOffset(double offset) = 0;

			virtual bool GetIgnoreRotation() const = 0;
			virtual void SetIgnoreRotation(bool gnore) = 0;
		};


		struct  ISimpleMarkerSymbol : public IMarkerSymbol
		{
			ISimpleMarkerSymbol(){}
			virtual ~ISimpleMarkerSymbol(){}
			virtual void                 SetStyle(eSimpleMarkerStyle style) = 0;
			virtual void                 SetOutline(bool bOutline) = 0;
			virtual bool                 IsOutline() const = 0;
			virtual Color                GetOutlineColor() const = 0;
			virtual void                 SetOutlineColor( const Color &color) = 0;
			virtual double               GetOutlineSize() const = 0;
			virtual void                 SetOutlineSize(double size) = 0;
		};  
		struct ICharacterMarkerSymbol  : public IMarkerSymbol
		{

			ICharacterMarkerSymbol(){}
			virtual ~ICharacterMarkerSymbol(){}
			virtual const CFont& GetFont() const = 0;
			virtual CFont& GetFont() = 0;
			virtual void        SetFont( const CFont &font ) = 0;
			virtual int         GetCharacterIndex() const = 0; 
			virtual void        SetCharacterIndex( int index ) = 0;
		};
		struct IPictureMarkerSymbol : public IMarkerSymbol
		{
			IPictureMarkerSymbol(){}
			virtual ~IPictureMarkerSymbol(){}
			virtual Color         GetBitmapTransparencyColor() const = 0;
			virtual void          SetBitmapTransparencyColor( const Color &color ) = 0;
			virtual BitmapPtr		 GetBitmap() = 0;
			virtual void          SetBitmap(CBitmap* pImage) = 0;
			virtual Color         GetBackgroundColor() const = 0;
			virtual void          SetBackgroundColor( const Color &color ) = 0;
			virtual bool          GetDrawExact() const = 0;
			virtual void          SetDrawExact( bool drawExact ) = 0;
		};

		struct IFillSymbol : public ISymbol
		{
			IFillSymbol(){}
			virtual ~IFillSymbol(){}
			virtual ILineSymbolPtr GetOutlineSymbol() const = 0;
			virtual void SetOutlineSymbol(ILineSymbol *pLine) = 0;
			virtual void FillRect(IDisplay* pDisplay, const Display::GRect& rect) = 0;

			virtual Color GetColor() const = 0;
			virtual void  SetColor(const Color &color) = 0;
		};

		struct ISimpleFillSymbol : public IFillSymbol
		{
			ISimpleFillSymbol(){}
			virtual ~ISimpleFillSymbol(){}
			virtual eSimpleFillStyle GetStyle() const = 0;
			virtual void SetStyle(eSimpleFillStyle style) = 0;
			virtual Color GetBackgroundColor() const = 0;
			virtual void  SetBackgroundColor( Color color ) = 0;
		};

		struct IPictureFillSymbol : public IFillSymbol
		{
			IPictureFillSymbol(){}
			virtual ~IPictureFillSymbol(){}
			virtual Color           GetBitmapTransparencyColor() const = 0;
			virtual void            SetBitmapTransparencyColor( const Color &color ) = 0;
			virtual Color           GetBackgroundColor() const = 0;
			virtual void            SetBackgroundColor( const Color &color ) = 0;
			virtual BitmapPtr		GetBitmap() = 0;
			virtual void            SetBitmap(CBitmap * pImage) = 0;
		};

		struct IMarkerFillSymbol  : public IFillSymbol
		{
			IMarkerFillSymbol(){}
			virtual ~IMarkerFillSymbol(){}
			virtual IMarkerSymbolPtr   GetMarkerSymbol() const = 0;
			virtual void               SetMarkerSymbol(IMarkerSymbol *pMarker) = 0;
			virtual eMarkerFillStyle GetStyle() const = 0;
			virtual void               SetStyle(eMarkerFillStyle style) = 0;
			virtual double GetXSeparation() const = 0;
			virtual void   SetXSeparation( double sep) = 0;
			virtual double GetYSeparation() const = 0;
			virtual void   SetYSeparation( double sep) = 0;  
			virtual double GetXOffset() const = 0;
			virtual void   SetXOffset( double offset) = 0;
			virtual double GetYOffset() const = 0;
			virtual void   SetYOffset( double offset) = 0;
		};

		struct  ILineFillSymbol  : public IFillSymbol
		{
			ILineFillSymbol(){}
			virtual ~ILineFillSymbol(){}
			virtual ILineSymbolPtr GetLineSymbol() const  = 0;
			virtual void           SetLineSymbol(ILineSymbol *pLine) = 0;
			virtual double         GetAngle() const = 0;
			virtual void           SetAngle( double angle ) = 0;
			virtual double         GetOffset() const = 0;
			virtual void           SetOffset( double offset) = 0;
			virtual double         GetSeparation() const = 0;
			virtual void           SetSeparation( double sep) = 0;
		};

		struct ITextSymbol : public ISymbol
		{
			ITextSymbol(){}
			virtual ~ITextSymbol(){}
			virtual GUnits GetAngle() const = 0;
			virtual void   SetAngle( GUnits dAngle ) = 0;
			virtual Color  GetColor() const = 0;
			virtual void   SetColor(const Color &color ) = 0;
			virtual const CFont& GetFont() const = 0;
			virtual     CFont& GetFont()  = 0;
			virtual void   SetFont(const  CFont& font ) = 0;
			virtual void GetTextSize(IDisplay* display, const CommonLib::CString& szText, GUnits *pxSize , GUnits *pySize, GUnits* baseLine) const = 0;
			virtual GUnits GetSize() const = 0;
			virtual void SetSize(GUnits size) = 0;
			virtual  const CommonLib::CString& GetText() const = 0;
			virtual void SetText(const CommonLib::CString& szText ) = 0;
			virtual ITextBackgroundPtr GetTextBackground() const = 0;
			virtual void SetTextBackground( ITextBackground *bg ) = 0;
			virtual int   GetTextDrawFlags() const = 0;
			virtual void  SetTextDrawFlags(int flags) = 0;
		};


		struct ITextBackground : public CommonLib::AutoRefCounter	, 
								 public GisCommon::IStreamSerialize, 
									public GisCommon::IXMLSerialize
		{
			ITextBackground(){}
			virtual ~ITextBackground(){}
			virtual void                      Setup( IDisplay* display  ) = 0;
			virtual void                      Draw(IDisplay* display, const GRect& rect) = 0;
			virtual void                      Reset() = 0;
			virtual void                      GetBoundaryRect(IDisplay* display, GisBoundingBox &bbox) const = 0;
			virtual ITextSymbolPtr            GetTextSymbol() const = 0;
			virtual void                      SetTextSymbol( ITextSymbol *symbol ) = 0;
			virtual void                      SetTextBox( const GisBoundingBox &box ) = 0;
		};

		struct  IMarkerTextBackground : public ITextBackground
		{
			IMarkerTextBackground(){}
			virtual ~IMarkerTextBackground(){}
			virtual IMarkerSymbolPtr GetSymbol() const = 0;
			virtual void             SetSymbol( IMarkerSymbol *symbol ) = 0;
			virtual bool             GetScaleToFit() const = 0;
			virtual void             SetScaleToFit( bool bFit ) = 0;
		};


	}
}


#endif