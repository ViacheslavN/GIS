#ifndef GIS_ENGINE_DISPLAY_DISPLAY_H_
#define GIS_ENGINE_DISPLAY_DISPLAY_H_

#include "Graphics.h"
#include "GraphTypes.h"
#include "IDisplayTransformation.h"
#include "Bitmap.h"
#include "Brush.h"
#include "Pen.h"
#include "Rect.h"

#include "CommonLibrary/IRefCnt.h"
#include "CommonLibrary/GeoShape.h"
#include "Common/GisEngineCommon.h"

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
			MultiLayerSymbolID,
			

		};

	

	

		struct ISymbol;
		struct ILineSymbol;
		struct IMarkerSymbol;
		struct ITextBackground;
		struct ITextSymbol;

		COMMON_LIB_REFPTR_TYPEDEF(ISymbol);
		COMMON_LIB_REFPTR_TYPEDEF(ILineSymbol);
		COMMON_LIB_REFPTR_TYPEDEF(IMarkerSymbol);
		COMMON_LIB_REFPTR_TYPEDEF(ITextBackground);
		COMMON_LIB_REFPTR_TYPEDEF(ITextSymbol);


		struct ViewPosition
		{
			ViewPosition(){}
			 ViewPosition(const GisXYPoint &center, double scale, double rotation ) :
					m_Center(center), m_dScale(scale), m_dRotation(rotation){}
			~ViewPosition(){}
			GisXYPoint m_Center;
			double m_dScale;
			double m_dRotation;
		};
		struct  IDisplay 
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
		    virtual void FlushBuffers(IDisplay* display, GisCommon::ITrackCancel* trackCancel);
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
			virtual double GetAngle() const = 0;
			virtual void   SetAngle( double dAngle ) = 0;
			virtual Color  GetColor() const = 0;
			virtual void   SetColor(const Color &color ) = 0;
			virtual const CFont& GetFont() const = 0;
			virtual     CFont& GetFont()  = 0;
			virtual void   SetFont(const  CFont& font ) = 0;
			virtual void GetTextSize(IDisplay* display, const CommonLib::str_t& szText, double *pxSize , double *pySize) const = 0; // Gets the x and y dimensions of 'text' in points (1/72 inch). 
			virtual double GetSize() const = 0;
			virtual void SetSize(double size) = 0;
			virtual  const CommonLib::str_t& GetText() const = 0;
			virtual void SetText(const CommonLib::str_t& szText ) = 0;
			virtual ITextBackgroundPtr GetTextBackground() const = 0;
			virtual void SetTextBackground( ITextBackground *bg ) = 0;
		};



		struct ITextBackground : public CommonLib::AutoRefCounter
		{
			ITextBackground(){}
			virtual ~ITextBackground(){}
			virtual void                      Setup( IDisplay* display  ) = 0;
			virtual void                      Draw() = 0;
			virtual void                      Reset() = 0;
			virtual void                      GetBoundaryRect(IDisplay* display, GisBoundingBox &bbox) const = 0;
			virtual ITextSymbolPtr           GetTextSymbol() const = 0;
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