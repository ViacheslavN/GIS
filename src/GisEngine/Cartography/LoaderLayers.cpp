#include "stdafx.h"
#include "LoaderLayers.h"
#include "FeatureLayer.h"

namespace GisEngine
{
	namespace Cartography
	{

		template<class TSerealizer>
		ILayerPtr LoadLayerT(TSerealizer *pSerealizer, uint32 nLayerID);

		ILayerPtr LoaderLayers::LoadLayer(CommonLib::IReadStream *pStream)
		{
			uint32 nLayerID = pStream->readIntu32();
			if(nLayerID == UndefineLayerID)
				return ILayerPtr();

			return LoadLayerT<CommonLib::IReadStream>(pStream, nLayerID);
		}

		ILayerPtr LoaderLayers::LoadLayer(GisCommon::IXMLNode *pNode)
		{

			uint32 nLayerID = pNode->GetPropertyInt32U(L"LayerID", UndefineLayerID);
			if(nLayerID == UndefineLayerID)
				return ILayerPtr();

			return LoadLayerT<GisCommon::IXMLNode>(pNode, nLayerID);
		}

		template<class TSerealizer>
		ILayerPtr LoadLayerT(TSerealizer *pSerealizer, uint32 nLayerID)
		{
			ILayerPtr pLayer;

			switch(nLayerID)
			{
				case FeatureLayerID:
					{
						CFeatureLayer *pFeatureLayer = new CFeatureLayer();
						pFeatureLayer->load(pSerealizer);
						pLayer = pFeatureLayer;
					}
					break;
			}
		
			return pLayer;
		}
	
	}
}