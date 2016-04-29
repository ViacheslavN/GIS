#include "stdafx.h"
#include "Map.h"
#include "Layers.h"
#include "Selection.h"
#include "GisGeometry/Envelope.h"
#include "Display/DisplayTransformation2D.h"
#include "CommonLibrary/crc.h"
#include "GisGeometry/SpatialReferenceProj4.h"
#include "Display/LoaderSymbols.h"
#include "LoaderLayers.h"
namespace GisEngine
{
	namespace Cartography
	{
		CMap::CMap() :
			m_dMinScale(0.0),
			m_dMaxScale(0.0),
			m_MapUnits(GisCommon::UnitsUnknown),
			m_bFlipVertical(false),
			m_bflipHorizontal(false),
			m_bViewPos(false),
			m_bHasReferenceScale(false),
			m_bDelayDrawing(false),
			m_dReferenceScale(0.),
			m_bCalcBB(false)
		{
			m_pLayers = new CLayers();
			m_pSelection = new CSelection();
		}
		CMap::~CMap()
		{

		}

		CommonLib::CString	CMap::GetName() const
		{
			return m_sName;
		}
		void   CMap::SetName(const  CommonLib::CString& sName)
		{
			m_sName = sName;
		}
		ILayersPtr   CMap::GetLayers() const
		{
			return m_pLayers;
		}
		 void	CMap::SelectFeatures(const GisBoundingBox& extent, bool resetSelection)
		 {
			 if(resetSelection)
				 m_pSelection->Clear();

			 int layerCount = m_pLayers->GetLayerCount();
			 for(int i = 0; i < layerCount; ++i)
			 {
				 ILayerPtr pLayer = m_pLayers->GetLayer(i);
				 if(!pLayer->GetVisible() || !pLayer->IsValid() || pLayer->GetLayerID() != FeatureLayerID)
					 continue;

			    IFeatureLayer *pFeatureLayer = dynamic_cast< IFeatureLayer *>(pLayer.get());
				assert(pFeatureLayer);

				if(!pFeatureLayer->GetSelectable())
					 continue;
				 
				 pFeatureLayer->SelectFeatures(extent, m_pSelection.get(), m_pSpatialRef.get());
			 }
		 }
		ISelectionPtr   CMap::GetSelection() const
		{
			return m_pSelection;
		}
		GisGeometry::IEnvelopePtr   CMap::GetFullExtent(GisGeometry::ISpatialReference* spatRef) const
		{
			if(m_pFullExtent.get())
				return GisGeometry::IEnvelopePtr( m_pFullExtent->clone());

			int layerCount = m_pLayers->GetLayerCount();
			if( layerCount == 0 )
			{
				GisBoundingBox bbox;
				bbox.type = CommonLib::bbox_type_normal;
					bbox.xMin = bbox.yMin = -1;
				bbox.xMax = bbox.yMax = 1;
				return GisGeometry::IEnvelopePtr(new GisGeometry::CEnvelope(bbox, spatRef ? spatRef : m_pSpatialRef.get()));
			}

			GisGeometry::IEnvelopePtr pEnvelope = GisGeometry::IEnvelopePtr(new GisGeometry::CEnvelope(GisBoundingBox(), spatRef ? spatRef : m_pSpatialRef.get()));
			for(int i = 0; i < layerCount; ++i)
				pEnvelope->Expand(m_pLayers->GetLayer(i)->GetExtent().get());

			m_pFullExtent = pEnvelope;
			return m_pFullExtent;
		}
		void   CMap::SetFullExtent(GisGeometry::IEnvelope* pEnv)
		{
			if(!pEnv)
				return;
			m_pFullExtent = pEnv->clone();
		}
		GisGeometry::ISpatialReferencePtr CMap::GetSpatialReference() const
		{
			return m_pSpatialRef;
		}
		void   CMap::SetSpatialReference(GisGeometry::ISpatialReference* spatRef)
		{
			m_pSpatialRef = spatRef;
		}
		void  CMap::Draw(Display::IDisplay* pDisplay, GisCommon::ITrackCancel* pTrackCancel)
		{
			 PartialDraw( DrawPhaseAll , pDisplay, pTrackCancel );
		}
		void   CMap::PartialDraw( eDrawPhase phase, Display::IDisplay* pDisplay, GisCommon::ITrackCancel* pTrackCancel)
		{
			if(m_bDelayDrawing)
				return;

			Display::IDisplayTransformationPtr pTrans = pDisplay->GetTransformation();
			double scale = pTrans->GetScale();

			double maximumScale = GetMaximumScale();
			double minimumScale = GetMinimumScale();

			OnBeforeDrawEvent.fire(pDisplay, DrawPhaseNone);

			double oldScale = pTrans->GetReferenceScale();
			//if(GetHasReferenceScale() && GetReferenceScale() != 0.0)
			if(m_bHasReferenceScale && m_dReferenceScale)
				pTrans->SetReferenceScale(m_dReferenceScale);
			else
				pTrans->SetReferenceScale(pTrans->GetScale());

			int layerCount = m_pLayers->GetLayerCount();
			int classIndex = 0;
			if ( phase & DrawPhaseAnnotation )
			{
				//TO DO Draw Label
			}

			  eDrawPhase phaseMask = (eDrawPhase)(phase & (DrawPhaseGeography | DrawPhaseAnnotation));

			  if ( phaseMask & DrawPhaseGeography )
			  {  
			 
				  if ( m_pBackgroundSymbol.get() )
				  {
					  /*
					  GisBoundingBox box;
					  trans->DeviceToMap(trans->GetDeviceRect(), box);
					  CommonLib::CGeoShape shape(box);
					  */
					  m_pBackgroundSymbol->Prepare(pDisplay);
					  m_pBackgroundSymbol->FillRect(pDisplay, pTrans->GetDeviceRect());
					  m_pBackgroundSymbol->Reset();
				  }
			  }

			  if(phaseMask)
			  {
				 
				  OnBeforeDrawEvent.fire(pDisplay, phaseMask);
				  for(int i = 0; i < layerCount; i++)
				  {
					  CHECK_TRACK_CANCEL(pTrackCancel);

			
					  ILayerPtr pLayer = m_pLayers->GetLayer(i);
					  if(pLayer.get())
					  {
						  pLayer->Draw(phaseMask, pDisplay, pTrackCancel);
					  }

				  }
			 
			  }
			  if ( phase & DrawPhaseSelection )
			  {
				  OnBeforeDrawEvent.fire(pDisplay, DrawPhaseSelection);
				  if(m_pSelection.get())
					  m_pSelection->Draw(pDisplay, pTrackCancel);
				
				  OnAfterDrawEvent.fire(pDisplay, DrawPhaseSelection);
			  }
			  if (phase & DrawPhaseGeography )
				   OnAfterDrawEvent.fire(pDisplay, DrawPhaseGeography);

			  if ( phase & DrawPhaseGraphics )
			  {
				  OnBeforeDrawEvent.fire(pDisplay, DrawPhaseGraphics);
				  for(int i = 0; i < layerCount; i++)
				  {
					  ILayerPtr pLayer = m_pLayers->GetLayer(i);
					  if(pLayer.get())
					  {
						  pLayer->Draw(DrawPhaseGraphics, pDisplay, pTrackCancel);
					  }
				  }

				  if(m_pGraphicsContainer.get())
				  {
					  for (size_t i = 0, sz = m_pGraphicsContainer->GetEnumCount(); i < sz; ++i)
					  {
						  IElementPtr pElement = m_pGraphicsContainer->GetElement(i);
						  pElement->Activate(pDisplay);
						  pElement->Draw(pDisplay, pTrackCancel);
						  pElement->Deactivate();

					  }
				  }
				  

				 OnAfterDrawEvent.fire(pDisplay, DrawPhaseGraphics);
			  }


			  if(m_pForegroundSymbol.get())
			  {
				  m_pForegroundSymbol->Prepare(pDisplay);
				  m_pForegroundSymbol->FillRect(pDisplay, pTrans->GetDeviceRect());
				  m_pForegroundSymbol->Reset();
			  }

			  pTrans->SetReferenceScale(oldScale);
			  OnAfterDrawEvent.fire(pDisplay, DrawPhaseNone);

		}
		ILabelEnginePtr  CMap::GetLabelEngine() const
		{
			return m_pLabelEngine;
		}
		void   CMap::SetLabelEngine(ILabelEngine*pEngine)
		{
			m_pLabelEngine = pEngine;
		}
		GisCommon::Units	CMap::GetMapUnits() const
		{
			if(!m_pSpatialRef.get())
				return m_MapUnits;
			GisCommon::Units ret = m_pSpatialRef->GetUnits();
			return (GisCommon::UnitsUnknown == ret) ? m_MapUnits : ret;
		}
		void   CMap::SetMapUnits( GisCommon::Units units )
		{
			m_MapUnits = units;
		}
		IGraphicsContainerPtr  CMap::GetGraphicsContainer() const
		{
			return m_pGraphicsContainer;
		}
		void       CMap::SetDelayDrawing(bool bDelay)
		{
			m_bDelayDrawing = bDelay;
		}
		IBookmarksPtr  CMap::GetBookmarks() const
		{
			return m_pBookmarks;
		}
		GisCommon::IPropertySetPtr  CMap::GetMapProperties()
		{
			return m_pPropertySet;
		}
		Display::IFillSymbolPtr	CMap::GetBackgroundSymbol() const
		{
			return m_pBackgroundSymbol;
		}
		void CMap::SetBackgroundSymbol(Display::IFillSymbol* pSymbol)
		{
			m_pBackgroundSymbol = pSymbol;
		}
		Display::IFillSymbolPtr	CMap::GetForegroundSymbol() const
		{
			return m_pForegroundSymbol;
		}
		void	 CMap::SetForegroundSymbol(Display::IFillSymbol* pSymbol)
		{
			m_pForegroundSymbol = pSymbol;
		}
		void	CMap::SetViewPos(const Display::ViewPosition& pos)
		{
			m_ViewPos = pos;
			m_bViewPos = true;
		}
		Display::ViewPosition	CMap::GetViewPos(bool bCalcIfAbsent, Display::IDisplayTransformation *pTrans)
		{
			if (!m_bViewPos && bCalcIfAbsent)
			{
				GisGeometry::ISpatialReferencePtr pSprefUse;
				if (m_pExtent.get())
				{
					pSprefUse = m_pExtent->GetSpatialReference();
				}
				if (!pSprefUse)
				{
					pSprefUse = m_pSpatialRef;
				}
				Display::IDisplayTransformationPtr trans(pTrans ? pTrans
					: (Display::IDisplayTransformation*)new  Display::CDisplayTransformation2D(96, m_MapUnits, Display::GRect(0, 0, 1024, 768), m_ViewPos.m_dScale));
				trans->SetSpatialReference(pSprefUse.get());
				trans->SetMapVisibleRect(GetExtent(pSprefUse.get(), true, trans.get())->GetBoundingBox());
				m_ViewPos = Display::ViewPosition(trans->GetMapPos(), trans->GetScale(),trans->GetRotation());
				m_bViewPos = true;
			}

			return m_ViewPos;
		}
		void	 CMap::SetExtent(GisGeometry::IEnvelope *pExtent)
		{
			if(!pExtent)
				return;

			m_pExtent = pExtent->clone();
		}
		GisGeometry::IEnvelopePtr	CMap::GetExtent(GisGeometry::ISpatialReference* pSpatRef, bool calc_if_absent, Display::IDisplayTransformation *pTrans)
		{
			GisGeometry::ISpatialReferencePtr pSprefUse(pSpatRef ? pSpatRef : m_pSpatialRef.get());
			if (!m_pExtent.get() && calc_if_absent)
			{
				if (m_bViewPos)
				{
					Display::IDisplayTransformationPtr trans(pTrans ? pTrans : (Display::IDisplayTransformation*)new  Display::CDisplayTransformation2D(96, m_MapUnits, Display::GRect(0, 0, 1024, 768), m_ViewPos.m_dScale));
					trans->SetSpatialReference(pSprefUse.get());
					trans->SetMapPos(m_ViewPos.m_Center, m_ViewPos.m_dScale);
					m_pExtent = new GisGeometry::CEnvelope(trans->GetFittedBounds(), pSprefUse.get());
				}
				else
				{
					m_pExtent = GetFullExtent(pSprefUse.get());
				}
			}
			m_pExtent->Project(pSprefUse.get());
			return m_pExtent;
		}
		void 	  CMap::SetVerticalFlip(bool bFlag)
		{
			m_bFlipVertical = bFlag;
		}
		bool	  CMap::GetVerticalFlip() const
		{
			return m_bFlipVertical;
		}

		void	  CMap::SetHorizontalFlip(bool bFlag)
		{
			m_bflipHorizontal = bFlag;
		}
		bool	  CMap::GetHorizontalFlip() const
		{
			return m_bflipHorizontal;
		}

		double	CMap::GetMinimumScale()
		{
			return m_dMinScale;
		}
		void	CMap::SetMinimumScale(double scale)
		{
			m_dMinScale = scale;
		}
		double	CMap::GetMaximumScale()
		{
			return m_dMaxScale;
		}
		void	CMap::SetMaximumScale(double scale)
		{
			m_dMaxScale = scale;
		}
		bool	CMap::GetHasReferenceScale() const
		{
			return m_bHasReferenceScale;
		}
		void	CMap::SetHasReferenceScale(bool flag)
		{
			m_bHasReferenceScale = flag;
		}
		double	CMap::GetReferenceScale() const
		{
			return m_dReferenceScale;
		}
		void	CMap::SetReferenceScale(double scale)
		{
			m_dReferenceScale = scale;
		}


		bool CMap::save(CommonLib::IWriteStream *pWriteStream) const
		{
			CommonLib::CWriteMemoryStream mapStream;
			uint32 nCrc = 0;
			
			mapStream.write(m_sName);
			mapStream.write((uint16)m_MapUnits);
			mapStream.write(m_dMinScale);
			mapStream.write(m_dMaxScale);
			mapStream.write(m_bHasReferenceScale);
			mapStream.write(m_dReferenceScale);
			mapStream.write(m_bFlipVertical);
			mapStream.write(m_bflipHorizontal);
			mapStream.write(m_pSpatialRef.get() ? true : false);
			if(m_pSpatialRef.get())
				m_pSpatialRef->save(&mapStream);

			GeoDatabase::IWorkspace::SaveWks(&mapStream);
			mapStream.write(m_pBackgroundSymbol.get() ? true : false);
			if(m_pBackgroundSymbol.get())
				m_pBackgroundSymbol->save(&mapStream);
			mapStream.write(m_pForegroundSymbol.get() ? true : false);
			if(m_pForegroundSymbol.get())
				m_pForegroundSymbol->save(&mapStream);
			mapStream.write(m_pLayers->GetLayerCount());
			for (int i = 0, sz = m_pLayers->GetLayerCount(); i < sz; ++i)
			{
				ILayerPtr pLayer = m_pLayers->GetLayer(i);
				pLayer->save(&mapStream);
			}
			uint32 crc = CommonLib::Crc32(mapStream.buffer(), mapStream.pos());
			pWriteStream->write(crc);
			pWriteStream->write(&mapStream, 0, mapStream.pos());
			return true;
		}
		bool CMap::load(CommonLib::IReadStream* pReadStream)
		{
			uint32 nCRC = pReadStream->readIntu32();
			CommonLib::CReadMemoryStream mapStream;
			SAFE_READ(pReadStream->save_read(&mapStream, true))
			uint32 nCalcCRC = CommonLib::Crc32(mapStream.buffer(), mapStream.size()); 
			if(nCalcCRC != nCRC)
				return false;

			SAFE_READ(mapStream.save_read(m_sName))
			
			uint16 MapUnits = 0;
			SAFE_READ(mapStream.save_read(MapUnits))
			m_MapUnits = (GisCommon::Units)MapUnits;
			SAFE_READ(mapStream.save_read(m_dMinScale))
			SAFE_READ(mapStream.save_read(m_dMaxScale))
			SAFE_READ(mapStream.save_read(m_bHasReferenceScale))
			SAFE_READ(mapStream.save_read(m_dReferenceScale))
			SAFE_READ(mapStream.save_read(m_bFlipVertical))
			SAFE_READ(mapStream.save_read(m_bflipHorizontal))
			bool bSpRef = false;
			SAFE_READ(mapStream.save_read(bSpRef))
			if(bSpRef)
			{
				m_pSpatialRef = new GisGeometry::CSpatialReferenceProj4();
				((GisCommon::IStreamSerialize*)m_pSpatialRef.get())->load(&mapStream);

			}
			GeoDatabase::IWorkspace::LoadWks(&mapStream);
			bool bBgSymbol = false;
			SAFE_READ(mapStream.save_read(bBgSymbol))
			if(bBgSymbol)
			{
				m_pBackgroundSymbol = (Display::IFillSymbol*)Display::LoaderSymbol::LoadSymbol(&mapStream).get();
			}

			bool bFgSymbol = false;
			SAFE_READ(mapStream.save_read(bFgSymbol))
			if(bFgSymbol)
			{
				m_pForegroundSymbol = (Display::IFillSymbol*)Display::LoaderSymbol::LoadSymbol(&mapStream).get();
			}
			 
			int nLayerCnt = 0;
			SAFE_READ(mapStream.save_read(nLayerCnt))
			for (int i = 0; i < nLayerCnt; ++i)
			{
				ILayerPtr pLayer =  LoaderLayers::LoadLayer(&mapStream);
				m_pLayers->AddLayer(pLayer.get());
			}


			return true;
		}

		bool CMap::saveXML(GisCommon::IXMLNode* pXmlNode) const
		{
			GisCommon::IXMLNodePtr pMapNode = pXmlNode->CreateChildNode(L"Map");

			pMapNode->AddPropertyString(L"Name", m_sName);
			pMapNode->AddPropertyInt16U(L"MapUnits", m_MapUnits);
			pMapNode->AddPropertyDouble(L"MinScale", m_dMinScale);
	 		pMapNode->AddPropertyDouble(L"MaxScale", m_dMaxScale);
			pMapNode->AddPropertyBool(L"HasReferenceScale", m_bHasReferenceScale);
			pMapNode->AddPropertyDouble(L"ReferenceScale", m_dReferenceScale);
			pMapNode->AddPropertyBool(L"FlipVertical", m_bFlipVertical);
			pMapNode->AddPropertyBool(L"FlipHorizonta", m_bflipHorizontal);
			if(m_pSpatialRef.get())
			{
				GisCommon::IXMLNodePtr pSpRefNode = pMapNode->CreateChildNode(L"SPRef");
				m_pSpatialRef->saveXML(pSpRefNode.get());
			}
			GeoDatabase::IWorkspace::SaveWks(pMapNode.get());
			if(m_pBackgroundSymbol.get())
			{
				GisCommon::IXMLNodePtr pBgSymbolNode = pMapNode->CreateChildNode(L"BackgroundSymbol");
				m_pBackgroundSymbol->saveXML(pBgSymbolNode.get());
			}

			if(m_pForegroundSymbol.get())
			{
				GisCommon::IXMLNodePtr pFgSymbolNode = pMapNode->CreateChildNode(L"ForegroundSymbol");
				m_pForegroundSymbol->saveXML(pFgSymbolNode.get());
			}
 
			GisCommon::IXMLNodePtr pLayersNode = pMapNode->CreateChildNode(L"Layers");
			for (int i = 0, sz = m_pLayers->GetLayerCount(); i < sz; ++i)
			{
				GisCommon::IXMLNodePtr pLayerNode = pLayersNode->CreateChildNode(L"Layer");
				ILayerPtr pLayer = m_pLayers->GetLayer(i);
				pLayer->saveXML(pLayerNode.get());
			}


			return true;
		}
		bool CMap::load(const GisCommon::IXMLNode* pMapNode)
		{

		/*	GisCommon::IXMLNodePtr pMapNode = pXmlNode->GetChild(L"Map");
			if(pMapNode.get())
				return false;*/

			m_sName = pMapNode->GetPropertyString(L"Name", m_sName);
			m_MapUnits = (GisCommon::Units)pMapNode->GetPropertyInt16U(L"MapUnits", m_MapUnits);
			m_dMinScale = pMapNode->GetPropertyDouble(L"MinScale", m_dMinScale);
			m_dMaxScale = pMapNode->GetPropertyDouble(L"MaxScale", m_dMaxScale);
			m_bHasReferenceScale = pMapNode->GetPropertyBool(L"HasReferenceScale", m_bHasReferenceScale);
			m_dReferenceScale = pMapNode->GetPropertyDouble(L"ReferenceScale", m_dReferenceScale);
			m_bFlipVertical = pMapNode->GetPropertyBool(L"FlipVertical", m_bFlipVertical);
			m_bflipHorizontal = pMapNode->GetPropertyBool(L"FlipHorizonta", m_bflipHorizontal);

			GisCommon::IXMLNodePtr pSpRefNode = pMapNode->GetChild(L"SPRef");
			if(pSpRefNode.get())
			{
				 m_pSpatialRef = new GisGeometry::CSpatialReferenceProj4();
				((GisCommon::IXMLSerialize*)m_pSpatialRef.get())->load(pSpRefNode.get());
			}
			GeoDatabase::IWorkspace::LoadWks(pMapNode);
			GisCommon::IXMLNodePtr pBgSymbolNode = pMapNode->GetChild(L"BackgroundSymbol");
			if(pBgSymbolNode.get())
			{
				m_pBackgroundSymbol = (Display::IFillSymbol*)Display::LoaderSymbol::LoadSymbol(pBgSymbolNode.get()).get();
			}
			GisCommon::IXMLNodePtr pFgSymbolNode = pMapNode->GetChild(L"ForegroundSymbol");
			if(pFgSymbolNode.get())
			{
				m_pForegroundSymbol = (Display::IFillSymbol*)Display::LoaderSymbol::LoadSymbol(pFgSymbolNode.get()).get();
			}

			GisCommon::IXMLNodePtr pLayersNode = pMapNode->GetChild(L"Layers");
			if(pLayersNode.get())
			{	
				for (int i = 0, nCount = pLayersNode->GetChildCnt();  i< nCount; ++i)
				{
					GisCommon::IXMLNodePtr pLayerNode = pLayersNode->GetChild(i);
					ILayerPtr pLayer  =  LoaderLayers::LoadLayer(pLayerNode.get());
					m_pLayers->AddLayer(pLayer.get());
				}
			}
			
			return true;
		}
	}
}