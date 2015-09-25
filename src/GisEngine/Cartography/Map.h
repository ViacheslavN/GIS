#ifndef GIS_ENGINE_CARTOGRAPHY_MAP_H_
#define GIS_ENGINE_CARTOGRAPHY_MAP_H_

#include "Cartography.h"

namespace GisEngine
{
	namespace Cartography
	{
		class CMap : public IMap
		{
			public:
				CMap();
				~CMap();

				virtual CommonLib::CString	              GetName() const;
				virtual void                              SetName(const  CommonLib::CString& name);
				virtual ILayersPtr                        GetLayers() const;
				virtual  void							  SelectFeatures(const GisBoundingBox& extent, bool resetSelection);
				virtual ISelectionPtr                     GetSelection() const;
				virtual GisGeometry::IEnvelopePtr         GetFullExtent(GisGeometry::ISpatialReference* spatRef = NULL) const;
				virtual void                              SetFullExtent(GisGeometry::IEnvelope* env);
				virtual GisGeometry::ISpatialReferencePtr GetSpatialReference() const;
				virtual void                              SetSpatialReference(GisGeometry::ISpatialReference* spatRef);
				virtual void                              Draw(Display::IDisplay* display, GisCommon::ITrackCancel* trackCancel);
				virtual void                              PartialDraw( eDrawPhase phase, Display::IDisplay* display, GisCommon::ITrackCancel* trackCancel);
				virtual ILabelEnginePtr                   GetLabelEngine() const;
				virtual void                              SetLabelEngine(ILabelEngine* engine);
				virtual  GisCommon::Units		          GetMapUnits() const;
				virtual void                              SetMapUnits( GisCommon::Units units );
				virtual IGraphicsContainerPtr             GetGraphicsContainer() const;
				virtual void                              SetDelayDrawing(bool delay);
				virtual IBookmarksPtr                     GetBookmarks() const;
				virtual GisCommon::IPropertySetPtr        GetMapProperties();
				virtual Display::IFillSymbolPtr			  GetBackgroundSymbol() const;
				virtual void							  SetBackgroundSymbol(Display::IFillSymbol* symbol);
				virtual Display::IFillSymbolPtr			  GetForegroundSymbol() const;
				virtual void							  SetForegroundSymbol(Display::IFillSymbol* symbol);
				virtual void							  SetViewPos(const Display::ViewPosition& pos);
				virtual Display::ViewPosition			  GetViewPos(bool calc_if_absent, Display::IDisplayTransformation *pTrans);
				virtual void							  SetExtent(GisGeometry::IEnvelope *extent);
				virtual GisGeometry::IEnvelopePtr		  GetExtent(GisGeometry::ISpatialReference* spatRef, bool calc_if_absent, Display::IDisplayTransformation *pTrans);
				virtual void							  SetVerticalFlip(bool flag);
				virtual bool							  GetVerticalFlip() const;
				virtual void							  SetHorizontalFlip(bool flag);
				virtual bool							  GetHorizontalFlip() const;

				virtual double							  GetMinimumScale();
				virtual void							  SetMinimumScale(double scale);
				virtual double							  GetMaximumScale();
				virtual void							  SetMaximumScale(double scale);
				virtual bool							  GetHasReferenceScale() const;
				virtual void							  SetHasReferenceScale(bool flag);
				virtual double							  GetReferenceScale() const;
				virtual void							  SetReferenceScale(double scale);


				virtual bool save(CommonLib::IWriteStream *pWriteStream) const;
				virtual bool load(CommonLib::IReadStream* pReadStream);

				virtual bool saveXML(GisCommon::IXMLNode* pXmlNode) const;
				virtual bool load(const GisCommon::IXMLNode* pXmlNode);

			private:

				CommonLib::CString m_sName;
				ILayersPtr m_pLayers;
				ISelectionPtr m_pSelection;
				GisGeometry::ISpatialReferencePtr	m_pSpatialRef;
				double m_dMinScale;
				double m_dMaxScale;
				double m_dReferenceScale;
				GisCommon::Units	m_MapUnits;
				bool m_bFlipVertical;
				bool m_bflipHorizontal;
				mutable GisGeometry::IEnvelopePtr m_pFullExtent;
				GisGeometry::IEnvelopePtr m_pExtent;
				Display::ViewPosition  m_ViewPos;
				bool m_bViewPos;
				bool m_bHasReferenceScale;
				bool m_bDelayDrawing;

				Display::IFillSymbolPtr     m_pBackgroundSymbol;
				Display::IFillSymbolPtr		m_pForegroundSymbol;
				ILabelEnginePtr				m_pLabelEngine;	
				IGraphicsContainerPtr		m_pGraphicsContainer;
				IBookmarksPtr				m_pBookmarks;
				GisCommon::IPropertySetPtr	m_pPropertySet;

				CommonLib::Event2<Display::IDisplay*, eDrawPhase> OnBeforeDrawEvent;
				CommonLib::Event2<Display::IDisplay*, eDrawPhase> OnAfterDrawEvent;
				bool m_bCalcBB;
 
		};
	}
}

#endif