#include "stdafx.h"
#include "LoaderLayers.h"
#include "FeatureClassLoader.h"
 
namespace GisEngine
{
	namespace Cartography
	{

		template<class TSerealizer>
		GeoDatabase::IFeatureClassPtr LoadFeatureClassT(TSerealizer *pSerealizer, uint32 nDatasetType);

		GeoDatabase::IFeatureClassPtr  LoaderFeatureClass::LoadFeatureClass(CommonLib::IReadStream *pStream)
		{
		 
			uint32 nDatasetType = pStream->readIntu32();
			if(nDatasetType == GeoDatabase::dtUndefined)
				return GeoDatabase::IFeatureClassPtr();

			return LoadFeatureClassT<CommonLib::IReadStream>(pStream, nDatasetType);
		}

		GeoDatabase::IFeatureClassPtr  LoaderFeatureClass::LoadFeatureClass(GisCommon::IXMLNode *pNode)
		{

			uint32 nDatasetType = pNode->GetPropertyInt32U(L"DatasetType",  GeoDatabase::dtUndefined);
			if(nDatasetType ==  GeoDatabase::dtUndefined)
				return GeoDatabase::IFeatureClassPtr();

			return LoadFeatureClassT<GisCommon::IXMLNode>(pNode, nDatasetType);
		}

		template<class TSerealizer>
		GeoDatabase::IFeatureClassPtr LoadFeatureClassT(TSerealizer *pSerealizer, uint32 nRendererID)
		{
			GeoDatabase::IFeatureClassPtr peatureClass;
			switch(nRendererID)
			{
			case GeoDatabase::dtFeatureClass :
				{
					
				}
				break;
			}

			return peatureClass;
		}

	}
}