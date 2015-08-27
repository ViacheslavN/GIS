#ifndef GIS_ENGINE_CARTOGRAPHY_H_
#define GIS_ENGINE_CARTOGRAPHY_H_

#include "CommonLibrary/IRefCnt.h"
#include "Display/Display.h"
#include "GisGeometry/Geometry.h"
#include "Common/GisEngineCommon.h"
#include "GeoDatabase/GeoDatabase.h"

namespace GisEngine
{
	namespace Cartography
	{

		enum eDrawPhase
		{
			DrawPhaseNone       = 0x0,
			DrawPhaseGeography  = 0x1,
			DrawPhaseAnnotation = 0x2,
			DrawPhaseSelection  = 0x4,
			DrawPhaseGraphics   = 0x8,
			DrawPhaseAll        = 0xFFFF
		};

		struct ILayer;
		struct IMap;
		struct IFeatureLayer;
		struct IFeatureRenderers;
		struct ILayers;
		struct IGraphicsContainer;
		struct IFeatureRenderer;
		struct ISelection;
		struct ILabelEngine;
		struct IBookmarks;

		COMMON_LIB_REFPTR_TYPEDEF(ILayer);
		COMMON_LIB_REFPTR_TYPEDEF(IMap);
		COMMON_LIB_REFPTR_TYPEDEF(IFeatureRenderers);
		COMMON_LIB_REFPTR_TYPEDEF(ILayers);
		COMMON_LIB_REFPTR_TYPEDEF(IGraphicsContainer);
		COMMON_LIB_REFPTR_TYPEDEF(IFeatureRenderer);
		COMMON_LIB_REFPTR_TYPEDEF(ISelection);
		COMMON_LIB_REFPTR_TYPEDEF(ILabelEngine);
		COMMON_LIB_REFPTR_TYPEDEF(IBookmarks);

		struct IMap : public  CommonLib::AutoRefCounter , 
					  public GisCommon::IStreamSerialize, 
					  public GisCommon::IXMLSerialize
		{
			IMap(){}
			~IMap(){}
			virtual CommonLib::str_t	              GetName() const = 0;
			virtual void                              SetName(const  CommonLib::str_t& name) = 0;
			virtual ILayersPtr                        GetLayers() const = 0;
			virtual ISelectionPtr                     GetSelection() const = 0;
			virtual GisGeometry::IEnvelopePtr         GetFullExtent(GisGeometry::ISpatialReference* spatRef = NULL) const = 0;
			virtual void                              SetFullExtent(GisGeometry::IEnvelope* env) = 0;
			virtual GisGeometry::ISpatialReferencePtr GetSpatialReference() const = 0;
			virtual void                              SetSpatialReference(GisGeometry::ISpatialReference* spatRef) = 0;
			virtual void                              Draw(Display::IDisplay* display, GisCommon::ITrackCancel* trackCancel) = 0;
			virtual void                              PartialDraw( eDrawPhase phase, Display::IDisplay* display, GisCommon::ITrackCancel* trackCancel) = 0;
			virtual ILabelEnginePtr                   GetLabelEngine() const = 0;
			virtual void                              SetLabelEngine(ILabelEngine* engine) = 0;
			virtual  GisCommon::Units		          GetMapUnits() const = 0;
			virtual void                              SetMapUnits( GisCommon::Units units ) = 0;
			virtual IGraphicsContainerPtr             GetGraphicsContainer() const = 0;
			virtual void                              SetDelayDrawing(bool delay) = 0;
			virtual IBookmarksPtr                     GetBookmarks() const = 0;
			virtual GisCommon::IPropertySetPtr        GetMapProperties() = 0;
		};

		struct  ILayer : public CommonLib::AutoRefCounter , 
						 public GisCommon::IStreamSerialize, 
						  public GisCommon::IXMLSerialize
		{
			ILayer(){}
			virtual ~ILayer(){}
			virtual GisGeometry::IEnvelopePtr GetExtent() const = 0;
			virtual void                      Draw(eDrawPhase phase, Display::IDisplay* display, GisCommon::ITrackCancel* trackCancel) = 0;
			virtual double                    GetMaximumScale() const = 0;
			virtual void                      SetMaximumScale(double scale) = 0;
			virtual double                    GetMinimumScale() const = 0;
			virtual void                      SetMinimumScale(double scale) = 0;
			virtual const CommonLib::str_t&   GetName() const = 0;
			virtual void                      SetName(const CommonLib::str_t& name) = 0;
			virtual eDrawPhase                GetSupportedDrawPhases() const = 0;
			virtual bool                      IsValid() const = 0;
			virtual bool                      GetVisible() const = 0;
			virtual void                      SetVisible(bool flag) = 0;
			virtual bool                      IsActiveOnScale(double scale) const = 0; 
		};

		struct  ILayers : public CommonLib::AutoRefCounter
		{
			ILayers(){}
			virtual ~ILayers(){}
			virtual int       GetLayerCount() const = 0;
			virtual ILayerPtr GetLayer(int index) const = 0;
			virtual void      AddLayer(ILayer* layer) = 0;
			virtual void      InsertLayer(ILayer* layer, int index) = 0;
			virtual void      RemoveLayer(ILayer* layer) = 0;
			virtual void      RemoveAllLayers() = 0;
			virtual void      MoveLayer(ILayer* layer, int index) = 0;
		};


		struct IFeatureLayer : public ILayer
		{
		 
			IFeatureLayer();
			virtual ~IFeatureLayer();
			virtual const CommonLib::str_t&           GetDisplayField() const = 0;
			virtual void                             SetDisplayField(const  const CommonLib::str_t& & field) = 0;
			virtual GeoDatabase::IFeatureClassPtr    GetFeatureClass() const = 0;
			virtual void                             SetFeatureClass(GeoDatabase::IFeatureClass* featureClass) = 0;
			virtual bool                             GetSelectable() const = 0;
			virtual void                             SetSelectable(bool flag) = 0;
			virtual IFeatureRenderersPtr             GetRenderers() const = 0;	
		};

		struct IGroupLayer : public ILayer
		{
			IGroupLayer();
			virtual ~IGroupLayer();
			virtual ILayersPtr GetChildren() const = 0;
			virtual bool       GetExpanded() = 0;
			virtual void       SetExpanded(bool flag) = 0;
		};


		struct IMapLayer : public IGroupLayer
		{
			IMapLayer();
			virtual ~IMapLayer();
			virtual IMapPtr    GetMap() const = 0;
			virtual void       SetMap(IMap *map) = 0;
		};

		struct  IGraphicsLayer : public ILayer
		{
			IGraphicsLayer();
			virtual ~IGraphicsLayer();
			virtual IGraphicsContainerPtr    GetGraphicsContainer() const = 0;
			virtual void                     SetGraphicsContainer(IGraphicsContainer *container) = 0;
			virtual bool                     GetShowLegend() = 0;
			virtual void                     SetShowLegend(bool show) = 0;
		};



		struct  IFeatureRenderers : public CommonLib::AutoRefCounter  
		{
			IFeatureRenderers();
			virtual ~IFeatureRenderers();
			virtual int                 GetRendererCount() const = 0;
			virtual IFeatureRendererPtr GetRenderer(int index) const = 0;
			virtual void                AddRenderer(IFeatureRenderer* renderer) = 0;
			virtual void                RemoveRenderer(IFeatureRenderer* renderer) = 0;
			virtual void                Clear() = 0;
		};

		struct  IFeatureRenderer : public CommonLib::AutoRefCounter
		{
			IFeatureRenderer();
			virtual ~IFeatureRenderer();
			virtual bool                   CanRender(GeoDatabase::IFeatureClass* cls, Display::IDisplay* display) const = 0;
			virtual void                   PrepareFilter(GeoDatabase::IFeatureClass* cls, GeoDatabase::IQueryFilter* filter) const = 0;
			virtual Display::ISymbolPtr    GetSymbolByFeature(GeoDatabase::IFeature* feature) const = 0;
			virtual double                 GetMaximumScale() const = 0;
			virtual void                   SetMaximumScale(double scale) = 0;
			virtual double                 GetMinimumScale() const = 0;
			virtual void                   SetMinimumScale(double scale) = 0;
			virtual const CommonLib::str_t&  GetShapeField() const = 0;
			virtual void                   SetShapeField(const CommonLib::str_t&  field) = 0;
		};
	}
}

#endif