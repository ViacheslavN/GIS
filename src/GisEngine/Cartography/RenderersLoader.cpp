#include "stdafx.h"
#include "LoaderLayers.h"
#include "FeatureRenderer.h"

namespace GisEngine
{
	namespace Cartography
	{

		template<class TSerealizer>
		IFeatureRendererPtr LoadRendererT(TSerealizer *pSerealizer, uint32 nLayerID);

		IFeatureRendererPtr LoadLayer(CommonLib::IReadStream *pStream)
		{
			//uint32 nRendererID = UndefineFeatureRendererID;
			uint32 nRendererID = pStream->readIntu32();
			if(nRendererID == UndefineFeatureRendererID)
				return IFeatureRendererPtr();

			return LoadRendererT<CommonLib::IReadStream>(pStream, nRendererID);
		}

		IFeatureRendererPtr LoadLayer(GisCommon::IXMLNode *pNode)
		{

			uint32 nRendererID = pNode->GetPropertyInt32U(L"RenderID", UndefineFeatureRendererID);
			if(nRendererID == UndefineFeatureRendererID)
				return IFeatureRendererPtr();

			return LoadRendererT<GisCommon::IXMLNode>(pNode, nRendererID);
		}

		template<class TSerealizer>
		IFeatureRendererPtr LoadRendererT(TSerealizer *pSerealizer, uint32 nRendererID)
		{
			IFeatureRendererPtr pRenderer;
			switch(nRendererID)
			{
			case SimpleFeatureRendererID:
				{
					CFeatureRenderer *pFeatureRenderer = new CFeatureRenderer();
					pFeatureRenderer->load(pSerealizer);
					pRenderer = pFeatureRenderer;
				}
				break;
			}

			return pRenderer;
		}

	}
}