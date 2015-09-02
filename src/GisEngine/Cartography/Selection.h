#ifndef GIS_ENGINE_CARTOGRAPHY_SELECTION_H_
#define GIS_ENGINE_CARTOGRAPHY_SELECTION_H_

#include "Cartography.h"
 

namespace GisEngine
{
	namespace Cartography
	{
		class CSelection : public ISelection
		{
		public:
			CSelection();
			virtual ~CSelection();

		private:
			CSelection(const CSelection&);
			CSelection& operator=(const CSelection&);

		public:
			// ISelection
			virtual void                       AddFeature(ILayer* layer, int featureID);
			virtual void                       Clear();
			virtual void                       ClearForLayer(ILayer* layer);
			virtual void                       RemoveFeature(ILayer* layer, int featureID);
			virtual IEnumLayersPtr			   GetLayers() const;
			virtual GisCommon::IEnumIDsPtr     GetFeatures(ILayer* layer) const;
			virtual void                       Draw(Display::IDisplay* display, GisCommon::ITrackCancel* trackCancel);
			virtual Display::ISymbolPtr			GetSymbol() const;
			virtual void                       SetSymbol(Display::ISymbol* symbol);
			virtual bool                       IsEmpty() const;


			virtual void SetOnSelectChange(OnSelectChange* pFunck, bool bAdd);
		private:

			CommonLib::Event				OnSelectChangeEvent;

			typedef std::set<int> TFeatureIDSet;
			typedef std::map<ILayerPtr, TFeatureIDSet> TFeatureMap;

			TFeatureMap            m_features;
			Display::ISymbolPtr m_pSymbol;
			mutable CommonLib::CSSection m_mutex;

		
		};
	}

}

#endif