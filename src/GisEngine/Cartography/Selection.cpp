#include "stdafx.h"
#include "Selection.h"
#include "Common/SimpleEnum.h"

namespace GisEngine
{
	namespace Cartography
	{
		CSelection::CSelection()
		{}

		CSelection::~CSelection()
		{}

	
		void CSelection::SetOnSelectChange(OnSelectChange* pFunck, bool bAdd)
		{
			if(bAdd)
				OnSelectChangeEvent += pFunck;
			else
				OnSelectChangeEvent -= pFunck;
		}

		// ISelection
		void CSelection::AddFeature(ILayer* layer, int featureID)
		{
			{
				CommonLib::CSSection::scoped_lock lock(m_mutex);

				ILayerPtr layer2(layer);
				TFeatureMap::iterator it = m_features.find(layer2);
				if(it == m_features.end())
					it = m_features.insert(TFeatureMap::value_type(layer2, TFeatureIDSet())).first;

				if (!it->second.insert(featureID).second)
				{  
					return;
				}
			}
			OnSelectChangeEvent.fire();
		}

		bool CSelection::IsEmpty() const
		{
			CommonLib::CSSection::scoped_lock lock(m_mutex);
			return m_features.empty();
		}

		void CSelection::Clear()
		{
			{
				CommonLib::CSSection::scoped_lock lock(m_mutex);
				if (m_features.empty())
				{
					return;
				}
				m_features.clear();
			}
			OnSelectChangeEvent.fire();
		}

		void CSelection::ClearForLayer(ILayer* layer)
		{
			{
				CommonLib::CSSection::scoped_lock lock(m_mutex);

				TFeatureMap::iterator it = m_features.find(ILayerPtr(layer));
				if(it == m_features.end())
				{
					return;
				}
				assert(!it->second.empty());
				m_features.erase(it);
			}
			OnSelectChangeEvent.fire();
		}

		void CSelection::RemoveFeature(ILayer* layer, int featureID)
		{
			{
				CommonLib::CSSection::scoped_lock lock(m_mutex);

				TFeatureMap::iterator it = m_features.find(ILayerPtr(layer));
				if(it == m_features.end())
				{
					return;
				}
				assert(!it->second.empty());
				if (!it->second.erase(featureID))
				{
					return;
				}
				if (it->second.empty())
				{
					m_features.erase(it);
				}
			}
			OnSelectChangeEvent.fire();
		}

		IEnumLayersPtr CSelection::GetLayers() const
		{
			CommonLib::CSSection::scoped_lock lock(m_mutex);
			
			IEnumLayersPtr objs(new GisCommon::CSimpleEnum<ILayerPtr, IEnumLayers>());

			TFeatureMap::const_iterator it = m_features.begin();
			while(it != m_features.end())
			{
				const ILayerPtr& pLayer = it->first;
				objs->add(pLayer);
				++it;
			}

			return objs;
		}

		GisCommon::IEnumIDsPtr CSelection::GetFeatures(ILayer* layer) const
		{
			CommonLib::CSSection::scoped_lock lock(m_mutex);

			GisCommon::IEnumIDsPtr  enumids(new GisCommon::CSimpleEnum<int64, GisCommon::IEnumIDs>());
			TFeatureMap::const_iterator it = m_features.find(ILayerPtr(layer));
			if(it == m_features.end())
				return enumids;

			const TFeatureIDSet& ids = (*it).second;
			TFeatureIDSet::const_iterator it2 = ids.begin();
			while(it2 != ids.end())
			{
				enumids->add(*it2);
				++it2;
			}

			return enumids;
		}

		void CSelection::Draw(Display::IDisplay* display, GisCommon::ITrackCancel* trackCancel)
		{
			CommonLib::CSSection::scoped_lock lock(m_mutex);

			if(!m_pSymbol)
				return;



			for(TFeatureMap::const_iterator it = m_features.begin(); it != m_features.end(); ++it)
			{
				if(!trackCancel->Continue())
					break;

				IFeatureLayer* layer = (IFeatureLayer*)(it->first.get());
				if(!layer)
					continue;
				if(!layer->GetVisible() || !layer->IsValid() || !layer->GetSelectable())
					continue;

				const TFeatureIDSet& ids = (*it).second;
				if(ids.size() == 0)
					continue;


				GisCommon::IEnumIDsPtr  enumids(new GisCommon::CSimpleEnum<int64, GisCommon::IEnumIDs>());
				TFeatureIDSet::const_iterator it2 = ids.begin();
				while(it2 != ids.end())
				{
					enumids->add(*it2);
					++it2;
				}

				layer->DrawFeatures(DrawPhaseGeography, enumids.get(), display, trackCancel, m_pSymbol.get());
			}
		}

		Display::ISymbolPtr CSelection::GetSymbol() const
		{
			return m_pSymbol;
		}

		void CSelection::SetSymbol(Display::ISymbol* symbol)
		{
			m_pSymbol = symbol;
		}
	}
}