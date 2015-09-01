#include "stdafx.h"
#include "LoaderLayers.h"

namespace GisEngine
{
	namespace Cartography
	{

		template<class TSerealizer>
		ILayerPtr LoadLayer(TSerealizer *pSerealizer, uint32 nLayerID);

		ILayerPtr LoadLayer(CommonLib::IReadStream *pStream)
		{
			uint32 nLayerID = UndefineLayerID;
			SAFE_READ(pStream, nLayerID);
			if(nLayerID == UndefineLayerID)
				return ILayerPtr();

			return LoadLayer<CommonLib::IReadStream>(pStream, nLayerID);
		}

		ILayerPtr LoadLayer(GisCommon::IXMLNode *pNode)
		{

			uint32 nLayerID = pNode->GetPropertyInt32U(L"LayerID", UndefineLayerID);
			if(nLayerID == UndefineLayerID)
				return ILayerPtr();

			return LoadLayer<GisCommon::IXMLNode>(pNode, nLayerID);
		}

		template<class TSerealizer>
		ILayerPtr LoadLayer(TSerealizer *pSerealizer, uint32 nLayerID)
		{
			ILayerPtr pLayer;
		
			return pLayer;
		}
	
	}
}