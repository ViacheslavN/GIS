#include "stdafx.h"
#include "FeatureLayer.h"
#include "GeoDatabase/QueryFilter.h"
#include "Display/DisplayUtils.h"
#include "GisGeometry/Envelope.h"
#include "RenderersLoader.h"

namespace GisEngine
{
	namespace Cartography
	{
		CFeatureLayer::CFeatureLayer()
		{
			m_nLayerSymbolID = FeatureLayerID;
		}
		CFeatureLayer::~CFeatureLayer()
		{

		}

		void CFeatureLayer::DrawFeatures(eDrawPhase phase, GisCommon::IEnumIDs* ids, Display::IDisplay* display, GisCommon::ITrackCancel* trackCancel, Display::ISymbol* customSymbol) const
		{

		}

		void CFeatureLayer::DrawEx(eDrawPhase phase, Display::IDisplay* pDisplay, GisCommon::ITrackCancel* pTrackCancel)
		{

			if(!IsValid())
				return;

			double oldRefScale = pDisplay->GetTransformation()->GetReferenceScale();
			if(m_hasReferenceScale)
				pDisplay->GetTransformation()->SetReferenceScale(m_hasReferenceScale);

			if((phase & DrawPhaseGeography))
			{

				GisBoundingBox bbox = pDisplay->GetTransformation()->GetFittedBounds();
				Display::GRect oldClipRect = pDisplay->GetTransformation()->GetDeviceClipRect();


				CalcBB(pDisplay, bbox);

				GisGeometry::ISpatialReferencePtr outSpatRef = pDisplay->GetTransformation()->GetSpatialReference();

				GisGeometry::IEnvelopePtr fullEnv  = m_pFeatureClass->GetExtent();
				GisGeometry::ISpatialReferencePtr spatRefFC = fullEnv->GetSpatialReference();


				GisGeometry::CEnvelope env(bbox, outSpatRef.get());

				if(!env.Intersect(fullEnv.get()))
				{
					pDisplay->GetTransformation()->SetReferenceScale(oldRefScale);
					return;
				}
				bbox = env.GetBoundingBox();

				GeoDatabase::CQueryFilter filter;
				filter.AddRef();
				filter.SetOutputSpatialReference(outSpatRef.get());
				filter.SetSpatialRel(GeoDatabase::srlIntersects);
				filter.SetBB(pDisplay->GetTransformation()->GetFittedBounds());
				 double precision = pDisplay->GetTransformation()->DeviceToMapMeasure(0.25);

				 if(spatRefFC.get() && outSpatRef.get())
				 {
					 GisBoundingBox box = pDisplay->GetTransformation()->GetFittedBounds();
					 box.xMin = (box.xMin + box.xMax) / 2;
					 box.yMin = (box.yMin + box.yMax) / 2;
					 box.xMax = box.xMin + precision;
					 box.yMax = box.yMin + precision;
					 if(!outSpatRef->Project(spatRefFC.get(), box))
						 precision = 0.0;
					 else
						 precision = min(box.xMax - box.xMin, box.yMax - box.yMin);
				 }

				 filter.SetPrecision(precision);

				 std::vector<IFeatureRenderer*> vecRenderes;
				 for (size_t i = 0, sz = m_vecRenderers.size(); i < sz; ++i)
				 {
					IFeatureRenderer *pRender = m_vecRenderers[i].get();
					if(!pRender->CanRender(m_pFeatureClass.get(), pDisplay))
						continue;
					pRender->PrepareFilter(m_pFeatureClass.get(), &filter);
					vecRenderes.push_back(pRender);
				 }

				 if(vecRenderes.empty())
				 {
					 pDisplay->GetTransformation()->SetReferenceScale(oldRefScale); //TO DO create holder
					 return;
				 }

				GeoDatabase::ICursorPtr pCursor = m_pFeatureClass->Search(&filter, true);
				if(!pCursor.get())
				{
					 pDisplay->GetTransformation()->SetReferenceScale(oldRefScale); //TO DO create holder
					return;
				}
				
				GeoDatabase::IRowPtr pRow;
				uint32 nRow = 0;
				pDisplay->Lock();
				while(pCursor->NextRow(&pRow))
				{
				
					if(!(nRow % GetCheckCancelStep()))
					{
						CHECK_TRACK_CANCEL_BREAK(pTrackCancel)
	
						 pDisplay->UnLock();
						 pDisplay->Lock();
					}

					GeoDatabase::IFeature *pFeature = (GeoDatabase::IFeature*)pRow.get();
					if(!pFeature)
						continue;

					for (size_t i = 0, sz = vecRenderes.size(); i < sz; ++i)
					{
						vecRenderes[i]->DrawFeature(pDisplay, pFeature);
					}

					nRow++;
				}
			}

			if(m_hasReferenceScale)
				pDisplay->GetTransformation()->SetReferenceScale(oldRefScale);
		}

		bool CFeatureLayer::IsValid() const
		{
			return m_pFeatureClass.get() != 0 && m_vecRenderers.size() > 0;
		}
		void CFeatureLayer::CalcBB(Display::IDisplay* pDisplay, GisBoundingBox& bb)
		{
			Display::GRect wndRC = pDisplay->GetTransformation()->GetDeviceRect();

			Display::GUnits dx = Display::SymbolSizeToDeviceSize(pDisplay->GetTransformation().get(), m_dDrawingWidth, m_bDrawingWidthScaleDependent);
			if(dx < 2)
				dx = 2;

			double map_x = pDisplay->GetTransformation()->DeviceToMapMeasure(dx);

			bb.xMin -= map_x;
			bb.xMax += map_x;

			bb.yMin -= map_x;
			bb.yMax += map_x;

			wndRC.inflate(dx, dx);
			pDisplay->GetTransformation()->SetDeviceClipRect(wndRC);
		}

		GisGeometry::IEnvelopePtr CFeatureLayer::GetExtent() const
		{

			if(!m_pFeatureClass.get())
				return GisGeometry::IEnvelopePtr();

			return m_pFeatureClass->GetExtent();
		}

		eDrawPhase CFeatureLayer::GetSupportedDrawPhases() const
		{
			return DrawPhaseGeography;
		}
		bool CFeatureLayer::IsActiveOnScale(double scale) const
		{
			if(!TBase::IsActiveOnScale(scale))
				return false;
 
			for(size_t i = 0; i < m_vecRenderers.size(); ++i)
			{
				double minScale = m_vecRenderers[i]->GetMinimumScale();
				double maxScale = m_vecRenderers[i]->GetMaximumScale();
				if((minScale == 0.0 || minScale > scale) && (maxScale == 0.0 || maxScale < scale))
					return true;
			}

			return false;
		}

		const CommonLib::CString& CFeatureLayer::GetDisplayField() const
		{	 
			return m_sDisplayField;
		}

		void CFeatureLayer::SetDisplayField(const CommonLib::CString& field)
		{		 
			m_sDisplayField = field;
		}

		GeoDatabase::IFeatureClassPtr CFeatureLayer::GetFeatureClass() const
		{
			return m_pFeatureClass;
		}

		void  CFeatureLayer::SetFeatureClass(GeoDatabase::IFeatureClass* featureClass)
		{
			m_pFeatureClass = featureClass;
		}
		bool  CFeatureLayer::GetSelectable() const
		{
			return m_bSelectable;
		}
		void  CFeatureLayer::SetSelectable(bool flag)
		{
			m_bSelectable = flag;
		}
		int	  CFeatureLayer::GetRendererCount() const
		{
			return m_vecRenderers.size();
		}
		IFeatureRendererPtr	CFeatureLayer::GetRenderer(int index) const
		{
			return m_vecRenderers[index];
		}
		void  CFeatureLayer::AddRenderer(IFeatureRenderer* renderer)
		{
			m_vecRenderers.push_back(IFeatureRendererPtr(renderer));
		}
		void  CFeatureLayer::RemoveRenderer(IFeatureRenderer* renderer)
		{
			TFeatureRenderer::iterator it = std::find(m_vecRenderers.begin(), m_vecRenderers.end(), IFeatureRendererPtr(renderer));
			if(it != m_vecRenderers.end())
				m_vecRenderers.erase(it);
		}
		void CFeatureLayer::ClearRenders()
		{
			m_vecRenderers.clear();
		}
		const CommonLib::CString&	CFeatureLayer::GetDefinitionQuery() const
		{
			return m_sQuery;
		}
		 void	CFeatureLayer::SetDefinitionQuery(const CommonLib::CString& sQuery)
		{
			m_sQuery = sQuery;
		}

		bool CFeatureLayer::save(CommonLib::IWriteStream *pWriteStream) const
		{

			CommonLib::CWriteMemoryStream stream;
			if(!TBase::save(&stream))
				return false;

			stream.write(m_sDisplayField);
			stream.write(m_sQuery);
			stream.write(m_bSelectable);
			stream.write(m_hasReferenceScale);
			stream.write((uint32)m_vecRenderers.size());
			for (size_t i = 0, sz = m_vecRenderers.size(); i < sz; ++i)
			{
				m_vecRenderers[i]->save(&stream);
			}

			stream.write(m_pFeatureClass.get() ? true : false);
			if(m_pFeatureClass.get())
			{
				stream.write(m_pFeatureClass->GetWorkspace()->GetID());
				stream.write(m_pFeatureClass->GetDatasetName());
				m_pFeatureClass->save(&stream);
			}

			pWriteStream->write(&stream);
			return true;
		}
		bool CFeatureLayer::load(CommonLib::IReadStream* pReadStream)
		{
			CommonLib::FxMemoryReadStream stream;
			SAFE_READ(pReadStream->save_read(&stream, true))
			if(!TBase::load(&stream))
				return false;

			SAFE_READ(stream.save_read(m_sDisplayField))
			SAFE_READ(stream.save_read(m_sQuery))
			SAFE_READ(stream.save_read(m_bSelectable))
			SAFE_READ(stream.save_read(m_hasReferenceScale))
			uint32 nRenders = 0;
			SAFE_READ(stream.save_read(nRenders))
			for (size_t i = 0; i < nRenders; ++i)
			{
				IFeatureRendererPtr pRenderer = LoaderRenderers::LoadRenderer(&stream);
				if(pRenderer.get())
					m_vecRenderers.push_back(pRenderer);
			}
			bool bFC = false;
			SAFE_READ(stream.save_read(bFC))
			if(bFC)
			{
				int32 nWksID = -1;
				SAFE_READ(stream.save_read(nWksID))
				CommonLib::CString sFCName;
				SAFE_READ(stream.save_read(sFCName))
				GeoDatabase::IWorkspacePtr pWks = GeoDatabase::IWorkspace::GetWorkspaceByID(nWksID);
				if(pWks.get())
				{
					m_pFeatureClass = pWks->OpenFeatureClass(sFCName);
				}
			}

			return true;
		}
		bool CFeatureLayer::saveXML(GisCommon::IXMLNode* pXmlNode) const
		{
			if(!TBase::saveXML(pXmlNode))
				return false;

			pXmlNode->AddPropertyString(L"DisplayField", m_sDisplayField);
			pXmlNode->AddPropertyString(L"Query", m_sQuery);
			pXmlNode->AddPropertyBool(L"Selectable", m_bSelectable);
			pXmlNode->AddPropertyBool(L"HasReferenceScale", m_hasReferenceScale);
			 
			GisCommon::IXMLNodePtr pRenderens = pXmlNode->CreateChildNode(L"Renderers");
	 
			for (size_t i = 0, sz = m_vecRenderers.size(); i < sz; ++i)
			{
				GisCommon::IXMLNodePtr pRender = pRenderens->CreateChildNode(L"Render");
				m_vecRenderers[i]->saveXML(pRender.get());
			}
			if(m_pFeatureClass.get())
			{
				GisCommon::IXMLNodePtr pFcNode = pXmlNode->CreateChildNode(L"FeatureClass");
				pFcNode->AddPropertyInt32(L"WksID", m_pFeatureClass->GetWorkspace()->GetID());
				pFcNode->AddPropertyString(L"Name", m_pFeatureClass->GetDatasetName());
			}

			return true;
		}
		bool CFeatureLayer::load(const GisCommon::IXMLNode* pXmlNode)
		{
			if(!TBase::load(pXmlNode))
				return false;


			m_sDisplayField = pXmlNode->GetPropertyString(L"DisplayField", m_sDisplayField);
			m_sQuery = pXmlNode->GetPropertyString(L"Query", m_sQuery);
			m_bSelectable = pXmlNode->GetPropertyBool(L"Selectable", m_bSelectable);
			m_hasReferenceScale = pXmlNode->GetPropertyBool(L"HasReferenceScale", m_hasReferenceScale);

			GisCommon::IXMLNodePtr pRenderens = pXmlNode->GetChild(L"Renderers");
			if(pRenderens.get())
			{
				for (size_t i = 0, sz = pRenderens->GetChildCnt(); i < sz; ++i)
				{
					GisCommon::IXMLNodePtr pRenderNode = pRenderens->GetChild(i);
					IFeatureRendererPtr pRender =  LoaderRenderers::LoadRenderer(pRenderNode.get());
					if(pRender.get())
						m_vecRenderers.push_back(pRender);
				}
			}
			
			GisCommon::IXMLNodePtr pFcNode = pXmlNode->GetChild(L"FeatureClass");
			if(pFcNode.get())
			{
				int32 nWksID = pFcNode->GetPropertyInt32(L"WksID", -1);
				CommonLib::CString sFCName = pFcNode->GetPropertyString(L"Name", L"");
				GeoDatabase::IWorkspacePtr pWks = GeoDatabase::IWorkspace::GetWorkspaceByID(nWksID);
				if(pWks.get())
				{
					m_pFeatureClass = pWks->OpenFeatureClass(sFCName);
				}
			}

			return true;
		}

	}


}
