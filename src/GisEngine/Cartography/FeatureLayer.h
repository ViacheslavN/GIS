#ifndef GIS_ENGINE_CARTOGRAPHY_FEATURE_LAYER_H_
#define GIS_ENGINE_CARTOGRAPHY_FEATURE_LAYER_H_

#include "LayerBase.h"

namespace GisEngine
{
	namespace Cartography
	{
		class  CFeatureLayer : public CLayerBase<IFeatureLayer> 
		{
			public:
				CFeatureLayer();
				~CFeatureLayer();

				virtual GisGeometry::IEnvelopePtr GetExtent() const;
				virtual eDrawPhase                GetSupportedDrawPhases() const;
				virtual bool                      IsValid() const;
				virtual bool                      IsActiveOnScale(double scale) const;

				// IFeatureLayer
				virtual const CommonLib::str_t&          GetDisplayField() const;
				virtual void                             SetDisplayField(const  CommonLib::str_t& sField);
				virtual GeoDatabase::IFeatureClassPtr    GetFeatureClass() const;
				virtual void                             SetFeatureClass(GeoDatabase::IFeatureClass* featureClass);
				virtual bool                             GetSelectable() const;
				virtual void                             SetSelectable(bool flag);
				virtual int								 GetRendererCount() const;
				virtual IFeatureRendererPtr				 GetRenderer(int index) const;
				virtual void							 AddRenderer(IFeatureRenderer* renderer);
				virtual void							 RemoveRenderer(IFeatureRenderer* renderer);
				virtual void							 ClearRenders();

				virtual void DrawFeatures(eDrawPhase phase, GisCommon::IEnumIDs* ids, Display::IDisplay* display, GisCommon::ITrackCancel* trackCancel, Display::ISymbol* customSymbol) const;
	
				

				void DrawEx(eDrawPhase phase, Display::IDisplay* display, GisCommon::ITrackCancel* trackCancel);

				virtual bool save(CommonLib::IWriteStream *pWriteStream) const;
				virtual bool load(CommonLib::IReadStream* pReadStream);
				virtual bool saveXML(GisCommon::IXMLNode* pXmlNode) const;
				virtual bool load(GisCommon::IXMLNode* pXmlNode);
		private:
			void CalcBB(Display::IDisplay* pDisplay, GisBoundingBox& bb);
		private:
			typedef std::vector<IFeatureRendererPtr> TFeatureRenderer;

			CommonLib::str_t  m_sDisplayField;
			TFeatureRenderer m_vecRenderers;
			bool m_bSelectable;
			bool m_hasReferenceScale;
			GeoDatabase::IFeatureClassPtr m_pFeatureClass;

			double                        m_dDrawingWidth;
			bool                          m_bDrawingWidthScaleDependent;

		};
	}
}

#endif