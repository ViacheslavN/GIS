#ifndef GIS_ENGINE_CARTOGRAPHY_H_
#define GIS_ENGINE_CARTOGRAPHY_H_

#include "CommonLibrary/IRefCnt.h"
#include "Display/Display.h"
#include "GisGeometry/Geometry.h"
#include "Common/GisEngineCommon.h"
#include "GeoDatabase/GeoDatabase.h"
#include "CommonLibrary/CSSection.h"

#include "CommonLibrary/delegate.h"

namespace GisEngine
{
	namespace Cartography
	{

		enum eDrawPhase
		{
			DrawPhaseNone       = 0,
			DrawPhaseGeography  = 1,
			DrawPhaseAnnotation = 2,
			DrawPhaseDrawAnnoCache = 4,
			DrawPhaseSelection  = 8,
			DrawPhaseGraphics   = 16,
			DrawPhaseAll        = 0xFFFF
		};

		enum eLabelStrategy
		{
			LabelStrategyAlone   = 0,
			LabelStrategySimple  = 1,
			LabelStrategyComplex = 2
		};


		enum eLayerTypeID
		{
			UndefineLayerID,
			FeatureLayerID

		};

		enum eSymbolAssignerID
		{
			UndefineSymbolAssignerID,
			SimpleSymbolAssignerID,
			ClassBreaksSymbolAssignerID,
			UniqueValueSymbolAssignerID,
			MaskSymbolAssignerID,
			StoredSymbolAssignerID
		};

		enum eFeatureRendererID
		{
			UndefineFeatureRendererID,
			SimpleFeatureRendererID
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
		struct ILegendGroup;
		struct ILegendClass;
		struct IGeoBookmark;
		struct ISymbolAssigner;
		struct ILabelCache;
		struct ILabelingOptions;
		struct ILabel;
		struct IFeatureLayer;
		struct IElement;

	

		COMMON_LIB_REFPTR_TYPEDEF(ILayer);
		COMMON_LIB_REFPTR_TYPEDEF(IMap);
		COMMON_LIB_REFPTR_TYPEDEF(IFeatureRenderers);
		COMMON_LIB_REFPTR_TYPEDEF(ILayers);
		COMMON_LIB_REFPTR_TYPEDEF(IGraphicsContainer);
		COMMON_LIB_REFPTR_TYPEDEF(IFeatureRenderer);
		COMMON_LIB_REFPTR_TYPEDEF(ISelection);
		COMMON_LIB_REFPTR_TYPEDEF(ILabelEngine);
		COMMON_LIB_REFPTR_TYPEDEF(IBookmarks);
		COMMON_LIB_REFPTR_TYPEDEF(ILegendGroup);
		COMMON_LIB_REFPTR_TYPEDEF(ILegendClass);
		COMMON_LIB_REFPTR_TYPEDEF(IGeoBookmark);
		COMMON_LIB_REFPTR_TYPEDEF(ISymbolAssigner);
		COMMON_LIB_REFPTR_TYPEDEF(ILabelCache);
		COMMON_LIB_REFPTR_TYPEDEF(ILabelingOptions);
		COMMON_LIB_REFPTR_TYPEDEF(IFeatureLayer);
		COMMON_LIB_REFPTR_TYPEDEF(IElement);

		typedef GisCommon::IEnumT<ILayerPtr> IEnumLayers;



		COMMON_LIB_REFPTR_TYPEDEF(IEnumLayers);

		typedef CommonLib::delegate2_t<Display::IDisplay*, eDrawPhase> OnBeforeDraw;
		typedef CommonLib::delegate2_t<Display::IDisplay*, eDrawPhase> OnAfterDraw;

		struct IMap : public  CommonLib::AutoRefCounter , 
					  public GisCommon::IStreamSerialize, 
					  public GisCommon::IXMLSerialize
		{
			IMap(){}
			~IMap(){}
			virtual CommonLib::CString	              GetName() const = 0;
			virtual void                              SetName(const  CommonLib::CString& name) = 0;
			virtual ILayersPtr                        GetLayers() const = 0;
			virtual  void							  SelectFeatures(const GisBoundingBox& extent, bool resetSelection) = 0;
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
			virtual Display::IFillSymbolPtr			  GetBackgroundSymbol() const= 0;
			virtual void							  SetBackgroundSymbol(Display::IFillSymbol* symbol) = 0;
			virtual Display::IFillSymbolPtr			  GetForegroundSymbol() const= 0;
			virtual void							  SetForegroundSymbol(Display::IFillSymbol* symbol) = 0;
			virtual void							  SetViewPos(const Display::ViewPosition& pos) = 0;
			virtual Display::ViewPosition			  GetViewPos(bool calc_if_absent, Display::IDisplayTransformation *pTrans) = 0;
			virtual void							  SetExtent(GisGeometry::IEnvelope *extent) = 0;
			virtual GisGeometry::IEnvelopePtr		  GetExtent(GisGeometry::ISpatialReference* spatRef, bool calc_if_absent, Display::IDisplayTransformation *pTrans) = 0;
			virtual void							  SetVerticalFlip(bool flag) = 0;
			virtual bool							  GetVerticalFlip() const = 0;
			virtual void							  SetHorizontalFlip(bool flag) = 0;
			virtual bool							  GetHorizontalFlip() const = 0;

			virtual double							  GetMinimumScale() = 0;
			virtual void							  SetMinimumScale(double scale) = 0;
			virtual double							  GetMaximumScale() = 0;
			virtual void							  SetMaximumScale(double scale) = 0;
			virtual bool							  GetHasReferenceScale() const = 0;
			virtual void							  SetHasReferenceScale(bool flag) = 0;
			virtual double							  GetReferenceScale() const = 0;
			virtual void							  SetReferenceScale(double scale) = 0;

		};

		struct  ILayer : public CommonLib::AutoRefCounter , 
						 public GisCommon::IStreamSerialize, 
						  public GisCommon::IXMLSerialize
		{
			ILayer(){}
			virtual ~ILayer(){}
			virtual	uint32					  GetLayerID() const = 0;
			virtual GisGeometry::IEnvelopePtr GetExtent() const = 0;
			virtual void                      Draw(eDrawPhase phase, Display::IDisplay* display, GisCommon::ITrackCancel* trackCancel) = 0;
			virtual double                    GetMaximumScale() const = 0;
			virtual void                      SetMaximumScale(double scale) = 0;
			virtual double                    GetMinimumScale() const = 0;
			virtual void                      SetMinimumScale(double scale) = 0;
			virtual const CommonLib::CString&   GetName() const = 0;
			virtual void                      SetName(const CommonLib::CString& name) = 0;
			virtual eDrawPhase                GetSupportedDrawPhases() const = 0;
			virtual bool                      IsValid() const = 0;
			virtual bool                      GetVisible() const = 0;
			virtual void                      SetVisible(bool flag) = 0;
			virtual bool                      IsActiveOnScale(double scale) const = 0; 
			virtual uint32					  GetCheckCancelStep() const = 0;
			virtual void					  SetCheckCancelStep(uint32 nCount) = 0;
		};


		typedef CommonLib::delegate1_t<ILayers*> OnRemoveAllLayers;
		typedef CommonLib::delegate2_t<ILayers*, ILayer*> OnLayerAdded;
		typedef CommonLib::delegate2_t<ILayers*, ILayer*> OnLayerRemove;
		typedef CommonLib::delegate3_t<ILayers*, ILayer*, int> OnLayerMoved;
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


			virtual void SetOnRemoveAllLayers(OnRemoveAllLayers* pFunck, bool bAdd) = 0;
			virtual void SetOnLayerAdded(OnLayerAdded* pFunck, bool bAdd) = 0;
			virtual void SetOnLayerRemove(OnLayerRemove* pFunck, bool bAdd) = 0;
			virtual void SetOnLayerMoved(OnLayerMoved* pFunck, bool bAdd) = 0;
		};


		struct IFeatureLayer : public ILayer
		{
		 
			IFeatureLayer(){};
			virtual ~IFeatureLayer(){};
			virtual const CommonLib::CString&        GetDisplayField() const = 0;
			virtual void                             SetDisplayField(const  CommonLib::CString& sField) = 0;
			virtual GeoDatabase::IFeatureClassPtr    GetFeatureClass() const = 0;
			virtual void                             SetFeatureClass(GeoDatabase::IFeatureClass* featureClass) = 0;
			virtual bool                             GetSelectable() const = 0;
			virtual void                             SetSelectable(bool flag) = 0;
			virtual const CommonLib::CString&		 GetDefinitionQuery() const= 0;
			virtual void							 SetDefinitionQuery(const CommonLib::CString& )= 0;
			virtual int								 GetRendererCount() const = 0;
			virtual IFeatureRendererPtr				 GetRenderer(int index) const = 0;
			virtual void							 AddRenderer(IFeatureRenderer* renderer) = 0;
			virtual void							 RemoveRenderer(IFeatureRenderer* renderer) = 0;
			virtual void							 ClearRenders() = 0;
			virtual void							 SelectFeatures(const GisBoundingBox& extent, ISelection *pSelection,  GisGeometry::ISpatialReference *pSpRef) = 0;
			virtual void DrawFeatures(eDrawPhase phase, GisCommon::IEnumIDs* ids, Display::IDisplay* display, GisCommon::ITrackCancel* trackCancel, Display::ISymbol* customSymbol) const = 0;
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

		struct  IFeatureRenderer : public CommonLib::AutoRefCounter, 
									public GisCommon::IStreamSerialize, 
									public GisCommon::IXMLSerialize
		{
			IFeatureRenderer(){};
			virtual ~IFeatureRenderer(){};
			virtual	uint32					GetFeatureRendererID()  const = 0;
			virtual bool                   CanRender(GeoDatabase::IFeatureClass* cls, Display::IDisplay* display) const = 0;
			virtual void                   PrepareFilter(GeoDatabase::IFeatureClass* cls, GeoDatabase::IQueryFilter* filter) const = 0;
			virtual Display::ISymbolPtr    GetSymbolByFeature(GeoDatabase::IFeature* feature) const = 0;
			virtual double                 GetMaximumScale() const = 0;
			virtual void                   SetMaximumScale(double scale) = 0;
			virtual double                 GetMinimumScale() const = 0;
			virtual void                   SetMinimumScale(double scale) = 0;
			virtual const CommonLib::CString&  GetShapeField() const = 0;
			virtual void                   SetShapeField(const CommonLib::CString&  field) = 0;
			virtual ISymbolAssignerPtr		GetSymbolAssigner() const = 0;
			virtual  void					SetSymbolAssigner(ISymbolAssigner* assigner) = 0;
			/*virtual bool Draw(eDrawPhase phase, GeoDatabase::IFeatureClass* featureClass, GeoDatabase::IQueryFilter* filter, 
				Display::IDisplay* display, GisCommon::ITrackCancel* trackCancel, Display::ISymbol* customSymbol = 0, bool drawAlways = false) = 0;*/
			virtual void DrawFeature(Display::IDisplay* display, GeoDatabase::IFeature* feature, Display::ISymbol* customSymbol = 0) = 0;
		};

		typedef CommonLib::delegate_t OnSelectChange;
 

		struct ISelection : public CommonLib::AutoRefCounter
		{
		 
			virtual void                       AddFeature(ILayer* layer, int featureID) = 0;
			virtual void                       Clear() = 0;
			virtual void                       ClearForLayer(ILayer* layer) = 0;
			virtual void                       RemoveFeature(ILayer* layer, int featureID) = 0;
			virtual IEnumLayersPtr			   GetLayers() const = 0;
			virtual GisCommon::IEnumIDsPtr     GetFeatures(ILayer* layer) const = 0;
			virtual void                       Draw(Display::IDisplay* display, GisCommon::ITrackCancel* trackCancel) = 0;
			virtual Display::ISymbolPtr		   GetSymbol() const = 0;
			virtual void                       SetSymbol(Display::ISymbol* symbol) = 0;
			virtual bool                       IsEmpty() const = 0;


			virtual void SetOnSelectChange(OnSelectChange* pFunck, bool bAdd) = 0;
		};

		struct ILegendClass : public CommonLib::AutoRefCounter
		{
			ILegendClass(){}
			virtual ~ILegendClass(){}
			virtual const CommonLib::CString&	   GetLabel() const = 0;
			virtual void					   SetLabel(const CommonLib::CString &label) = 0;
			virtual Display::ISymbolPtr		   GetSymbol() const = 0;
			virtual void					   SetSymbol( Display::ISymbol *symbol ) = 0;        
		};


		struct  ILegendInfo : public CommonLib::AutoRefCounter
		{

			//virtual ILegendGroupPtr GetLegendGroup( int index ) const = 0;    
			//virtual int             GetLegendGroupCount() const = 0;

			virtual int                    GetSymbolCount() const = 0;
			virtual Display::ISymbolPtr	   GetSymbolByIndex(int index) const = 0;
			virtual void                   SetSymbolByIndex(int index, Display::ISymbol *symbol ) = 0;
		};

		struct  ILegendGroup : public CommonLib::AutoRefCounter
		{
			ILegendGroup(){}
			virtual ~ILegendGroup(){}
			virtual int                   AddClass( ILegendClass *legendClass ) = 0;          
			virtual ILegendClassPtr       GetClass( int index) const = 0;                     
			virtual int                   GetClassCount() const = 0;                          
			virtual void                  ClearClasses() = 0;                                 
			virtual const CommonLib::CString&	  GetCaption() const = 0;                         
			virtual void                  SetCaption(const CommonLib::CString&caption)  = 0;    
			virtual bool                  GetVisible() const = 0;                             
			virtual void                  SetVisible( bool bVisible ) = 0;                     
		};

		struct ILabelRenderer : public CommonLib::AutoRefCounter
		{
			ILabelRenderer(){}
			virtual ~ILabelRenderer(){}
			virtual ILabelEnginePtr      GetLabelEngine() const = 0;
			virtual void                 SetLabelEngine(ILabelEngine* engine) = 0;
			virtual CommonLib::CString	 GetLabelField() const = 0;
			virtual void                 SetLabelField(const CommonLib::CString& field) = 0;
			virtual eLabelStrategy       GetStrategy() const = 0;
			virtual void                 SetStrategy(const eLabelStrategy& strategy) = 0;
			virtual int                  GetClassIndex() const = 0;
			virtual void                 SetClassIndex(int index) = 0;
		 
		};


		struct ILabelEngine : public CommonLib::AutoRefCounter
		{
			ILabelEngine(){}
			virtual ~ILabelEngine(){}
			virtual ILabelCachePtr GetLabelCache() const = 0;
			virtual void           BeginLabeling(Display::IDisplay* display) = 0;
			virtual void           OptimizeLabels(GisCommon::ITrackCancel* trackCancel) = 0;
			virtual void           DrawLabels(GisCommon::ITrackCancel* trackCancel) = 0;
			virtual void           EndLabeling() = 0;
		};

		struct ILabelCache : public CommonLib::AutoRefCounter
		{
			ILabelCache(){}
			virtual ~ILabelCache(){}
			virtual void           Setup(Display::IDisplay* pDisplay) = 0;
			virtual void           Clear() = 0;
			virtual void           AddLabel(const CommonLib::CString& text,
				CommonLib::CGeoShape* shape,
				double angle,
				double width,
				double height,
				Display::ITextSymbol* symbol,
				int classIndex,
				ILabelingOptions* options) = 0;
			virtual int  GetLabelCount() const = 0;
			virtual const ILabel*  GetLabel(int idx) const = 0;
		};


		struct ILabel
		{
			ILabel(){}
			virtual ~ILabel(){}
			virtual CommonLib::CString       GetText() const = 0;
			virtual Display::ITextSymbolPtr GetSymbol() const = 0;

		};

		struct  IBookmarks : public CommonLib::AutoRefCounter
		{
			IBookmarks(){}
			virtual ~IBookmarks(){}
			virtual int                 GetBookmarkCount() const = 0;
			virtual IGeoBookmarkPtr		GetBookmark(int index) const = 0;
			virtual void                AddBookmark(IGeoBookmark* bookmark) = 0;
			virtual void                RemoveBookmark(IGeoBookmark* bookmark) = 0;
			virtual void                RemoveAllBookmarks() = 0;
		};

		struct  IGeoBookmark  : public CommonLib::AutoRefCounter
		{
			IGeoBookmark(){}
			virtual ~IGeoBookmark(){}
			virtual const CommonLib::CString&       GetName() const = 0;  
			virtual void                          SetName(const CommonLib::CString& csName ) = 0; 
			virtual Display::ViewPosition		  GetPosition() const = 0;
			virtual void                          SetPosition(const Display::ViewPosition& pos) = 0;
			virtual GisGeometry::IEnvelopePtr     GetExtent() const = 0;
			virtual void                          SetExtent( GisGeometry::IEnvelope *extent ) = 0 ;
			virtual void                          ZoomTo( Display::IDisplayTransformation *pTrans ) const = 0;
		};


		struct  ISymbolAssigner  : public CommonLib::AutoRefCounter, 
									public GisCommon::IStreamSerialize, 
									public GisCommon::IXMLSerialize
		{
			ISymbolAssigner(){}
			virtual ~ISymbolAssigner(){}
			virtual uint32				   GetSymbolAssignerID() const = 0;
			virtual bool                   CanAssign(GeoDatabase::IFeatureClass* cls) const = 0;
			virtual void                   PrepareFilter(GeoDatabase::IFeatureClass* cls, GeoDatabase::IQueryFilter* filter) const = 0;
			virtual Display::ISymbolPtr	   GetSymbolByFeature(GeoDatabase::IFeature* feature) const = 0;
			virtual void                   SetupSymbols(Display::IDisplay* display) = 0;
			virtual void                   ResetSymbols() = 0;
			virtual void				   FlushBuffers(Display::IDisplay* pDisplay, GisCommon::ITrackCancel* trackCancel) = 0;
		};

		struct ISimpleSymbolAssigner : public ISymbolAssigner
		{
			ISimpleSymbolAssigner(){}
			virtual ~ISimpleSymbolAssigner(){}
			virtual const CommonLib::CString&		GetDescription() const = 0;
			virtual void						SetDescription(const CommonLib::CString& sDesc) = 0;
			virtual  const CommonLib::CString&    GetLabel() const = 0;
			virtual void						SetLabel(const CommonLib::CString& sLabel) = 0;
			virtual Display::ISymbolPtr			GetSymbol() const = 0;
			virtual void						SetSymbol(Display::ISymbol* symbol) = 0;


		};


		struct  IClassBreaksSymbolAssigner : public ISymbolAssigner
		{
			IClassBreaksSymbolAssigner(){}
			virtual ~IClassBreaksSymbolAssigner();
			virtual const CommonLib::CString&	    GetField() const = 0;
			virtual void						SetField(const CommonLib::CString& field) = 0;
			virtual double						GetBreak(int index) const = 0;
			virtual void						SetBreak(int index, double val) = 0;
			virtual bool						GetBreakValueIncluded(int index) const = 0;
			virtual void						SetBreakValueIncluded(int index, bool val) = 0;
			virtual int							GetBreakCount() const = 0;
			virtual void						SetBreakCount(int count) = 0;
			virtual const CommonLib::CString&		GetLabel(int index) const = 0;
			virtual void						SetLabel(int index,  const CommonLib::CString&	val) = 0;
			virtual const CommonLib::CString&		GetDescription(int index) const = 0;
			virtual void						SetDescription(int index,  const CommonLib::CString& val) = 0;
			virtual Display::ISymbolPtr			GetSymbol(int index) const = 0;
			virtual void						SetSymbol(int index, Display::ISymbol* symbol) = 0;
			virtual double						GetMinimumBreak() const = 0;
			virtual void						SetMinimumBreak(double val) = 0;
			virtual bool						GetMinimumBreakIncluded() const = 0;
			virtual void						SetMinimumBreakIncluded(bool val) = 0;
			virtual Display::ISymbolPtr			GetDefaultSymbol() const = 0;
			virtual void						SetDefaultSymbol(Display::ISymbol* symbol) = 0;
			virtual  const CommonLib::CString&	GetDefaultLabel() const = 0;
			virtual void						SetDefaultLabel( const CommonLib::CString& val) = 0;
			virtual bool						GetUseDefaultSymbol() const = 0;
			virtual void						SetUseDefaultSymbol(bool use) = 0;
		};

		struct IUniqueValueSymbolAssigner : public ISymbolAssigner
		{
			IUniqueValueSymbolAssigner(){}
			virtual ~IUniqueValueSymbolAssigner(){}
			virtual int							GetFieldCount() const = 0;
			virtual void						SetFieldCount(int fieldCount) = 0;
			virtual const CommonLib::CString&		GetField(int fieldIndex) const = 0;
			virtual void						SetField(int fieldIndex, const CommonLib::CString& fieldName) = 0;
			virtual CommonLib::CVariant*		GetValue(int index, int fieldIndex) const = 0;
			virtual void						SetValue(int index, int fieldIndex, CommonLib::CVariant& val) = 0;
			virtual int							GetValueCount() const = 0;
			virtual void						SetValueCount(int count) = 0;
			virtual const CommonLib::CString&		GetLabel(int index) const = 0;
			virtual void						SetLabel(int index, const CommonLib::CString& val) = 0;
			virtual const CommonLib::CString&		GetDescription(int index) const = 0;
			virtual void						SetDescription(int index, const CommonLib::CString& val) = 0;
			virtual Display::ISymbolPtr			GetSymbol(int index) const = 0;
			virtual void						SetSymbol(int index, Display::ISymbol* symbol) = 0;
			virtual int							GetGroup(int index) const = 0;
			virtual void						SetGroup(int index, int group) = 0;
			virtual Display::ISymbolPtr			GetDefaultSymbol() const = 0;
			virtual void						SetDefaultSymbol(Display::ISymbol* symbol) = 0;
			virtual const CommonLib::CString&		GetDefaultLabel() const = 0;
			virtual void						SetDefaultLabel(const CommonLib::CString& val) = 0;
			virtual bool						GetUseDefaultSymbol() const = 0;
			virtual void						SetUseDefaultSymbol(bool use) = 0;
			virtual const CommonLib::CString&		GetHeadingLabel() const = 0;
			virtual void						SetHeadingLabel( const CommonLib::CString& val ) = 0;
		};


		struct IRangeSymbolAssigner : public ISymbolAssigner
		{
			IRangeSymbolAssigner(){}
			virtual ~IRangeSymbolAssigner(){}
			virtual const CommonLib::CString&     GetField() const = 0;
			virtual void						SetField(const CommonLib::CString&  field) = 0;
			virtual void						GetRange(int index, CommonLib::CVariant& valFrom, CommonLib::CVariant& valTo) const = 0;
			virtual void						SetRange(int index, CommonLib::CVariant& valFrom, CommonLib::CVariant& valTo) = 0;
			virtual int							GetRangeCount() const = 0;
			virtual void						SetRangeCount(int count) = 0;
			virtual const CommonLib::CString&     GetLabel(int index) const = 0;
			virtual void						SetLabel(int index, const CommonLib::CString&  val) = 0;
			virtual const CommonLib::CString&     GetDescription(int index) const = 0;
			virtual void						SetDescription(int index, const CommonLib::CString&  val) = 0;
			virtual Display::ISymbolPtr			GetSymbol(int index) const = 0;
			virtual void						SetSymbol(int index, Display::ISymbol* symbol) = 0;
			virtual Display::ISymbolPtr			GetDefaultSymbol() const = 0;
			virtual void						SetDefaultSymbol(Display::ISymbol* symbol) = 0;
			virtual const CommonLib::CString&     GetDefaultLabel() const = 0;
			virtual void					    SetDefaultLabel(const CommonLib::CString& val) = 0;
			virtual bool						GetUseDefaultSymbol() const = 0;
			virtual void						SetUseDefaultSymbol(bool use) = 0;
			virtual void						SortRanges() = 0;
		};


		struct  IExpressionSymbolAssigner : public ISymbolAssigner
		{
			IExpressionSymbolAssigner(){}
			virtual ~IExpressionSymbolAssigner(){}
			virtual const CommonLib::CString&  GetExpression(int index) const = 0;
			virtual void                     SetExpression(int index, const CommonLib::CString& expr) = 0;
			virtual int						 GetExpressionCount() const = 0;
			virtual void					 SetExpressionCount(int count) = 0;
			virtual const CommonLib::CString&  GetLabel(int index) const = 0;
			virtual void					 SetLabel(int index, const  CommonLib::CString& val) = 0;
			virtual const CommonLib::CString&  GetDescription(int index) const = 0;
			virtual void                     SetDescription(int index, const CommonLib::CString& val) = 0;
			virtual Display::ISymbolPtr		 GetSymbol(int index) const = 0;
			virtual void					 SetSymbol(int index, Display::ISymbol* symbol) = 0;
			virtual Display::ISymbolPtr		 GetDefaultSymbol() const = 0;
			virtual void					 SetDefaultSymbol(Display::ISymbol* symbol) = 0;
			virtual const CommonLib::CString&  GetDefaultLabel() const = 0;
			virtual void					 SetDefaultLabel(const CommonLib::CString& val) = 0;
			virtual bool					 GetUseDefaultSymbol() const = 0;
			virtual void					 SetUseDefaultSymbol(bool use) = 0;
		};


		struct  IElement : public CommonLib::AutoRefCounter
		{
			IElement();
			virtual ~IElement(){}
			virtual void                      Draw(Display::IDisplay* pDisplay, GisCommon::ITrackCancel* trackCancel) = 0;
			virtual void                      Activate(Display::IDisplay* pDisplay) = 0;
			virtual void                      Deactivate() = 0;
			virtual void                      GetBounds(Display::IDisplay* pDisplay, GisBoundingBox & box) const = 0;
		};  

		struct  IGraphicsContainer  : public CommonLib::AutoRefCounter
		{
			IGraphicsContainer();
			virtual ~IGraphicsContainer(){}
			virtual bool                   IsEmpty() const = 0;
			virtual void                   AddElement( IElement* Element ) = 0;
			virtual void                   RemoveAllElements() = 0;
			virtual bool                   RemoveElement(IElement* Element) = 0;
			virtual bool                   BringToFront(IElement* Element) = 0;
			virtual bool                   SendToBack(IElement* Element) = 0;
			virtual	uint32				   GetEnumCount() const = 0;
			virtual IElementPtr			   GetElement(int nIdx) const = 0;
			
		};

	}
}

#endif