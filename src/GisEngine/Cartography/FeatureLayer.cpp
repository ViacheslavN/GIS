#include "stdafx.h"
#include "FeatureLayer.h"
#include "GeoDatabase/QueryFilter.h"
#include "Display/DisplayUtils.h"
#include "GisGeometry/Envelope.h"

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
				filter.SetOutputSpatialReference(outSpatRef.get());
				filter.SetSpatialRel(GeoDatabase::srlEnvelopeIntersects);
				 
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
						 if(pTrackCancel && !pTrackCancel->Continue())
							 break;

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

		const CommonLib::str_t& CFeatureLayer::GetDisplayField() const
		{	 
			return m_sDisplayField;
		}

		void CFeatureLayer::SetDisplayField(const CommonLib::str_t& field)
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


		bool CFeatureLayer::save(CommonLib::IWriteStream *pWriteStream) const
		{
			if(!TBase::save(pWriteStream))
				return false;

			pWriteStream->write(m_sDisplayField);
			pWriteStream->write(m_sQuery);
			pWriteStream->write(m_bSelectable);
			pWriteStream->write(m_hasReferenceScale);
			pWriteStream->write((uint32)m_vecRenderers.size());
			for (size_t i = 0, sz = m_vecRenderers.size(); i < sz; ++i)
			{
				m_vecRenderers[i]->save(pWriteStream);
			}

			pWriteStream->write(m_pFeatureClass.get() ? true : false);
			m_pFeatureClass->s


			return true;
		}
		bool CFeatureLayer::load(CommonLib::IReadStream* pReadStream)
		{
			if(!TBase::load(pReadStream))
				return false;

			return true;
		}
		bool CFeatureLayer::saveXML(GisCommon::IXMLNode* pXmlNode) const
		{
			if(!TBase::saveXML(pXmlNode))
				return false;

			return true;
		}
		bool CFeatureLayer::load(GisCommon::IXMLNode* pXmlNode)
		{
			if(!TBase::load(pXmlNode))
				return false;

			return true;
		}

	}


}
