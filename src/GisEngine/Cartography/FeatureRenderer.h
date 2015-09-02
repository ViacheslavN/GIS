#ifndef GIS_ENGINE_CARTOGRAPHY_FEATURE_RENDERER_H_
#define GIS_ENGINE_CARTOGRAPHY_FEATURE_RENDERER_H_


#include "FeatureRendererBase.h"


namespace GisEngine
{
	namespace Cartography
	{

		class CFeatureRenderer : public CFeatureRendererBase<IFeatureRenderer>
		{
		public:
			typedef CFeatureRendererBase<IFeatureRenderer> TBase;
			CFeatureRenderer();
			~CFeatureRenderer();
			//IFeatureRenderer

			virtual bool                   CanRender(GeoDatabase::IFeatureClass* cls, Display::IDisplay* display) const;
			virtual void                   PrepareFilter(GeoDatabase::IFeatureClass* cls, GeoDatabase::IQueryFilter* filter) const;
			virtual Display::ISymbolPtr    GetSymbolByFeature(GeoDatabase::IFeature* feature) const;
			virtual ISymbolAssignerPtr		GetSymbolAssigner() const;
			virtual  void					SetSymbolAssigner(ISymbolAssigner* assigner);
			virtual void DrawFeature(Display::IDisplay* display, GeoDatabase::IFeature* feature, Display::ISymbol* customSymbol = 0) = 0;

			virtual bool save(CommonLib::IWriteStream *pWriteStream) const;
			virtual bool load(CommonLib::IReadStream* pReadStream);
			virtual bool saveXML(GisCommon::IXMLNode* pXmlNode) const;
			virtual bool load(GisCommon::IXMLNode* pXmlNode);
			
		private:
			ISymbolAssignerPtr m_pSymbolAssigner;
		};
	}
}

#endif