#ifndef GIS_ENGINE_DISPLAY_SIMPLE_MARKET_SYMBOL_
#define GIS_ENGINE_DISPLAY_SIMPLE_MARKET_SYMBOL_

#include "MarkerSymbolBase.h"

namespace GisEngine
{
	namespace Display
	{

		class CSimpleMarketSymbol : public CMarkerSymbolBase<ISimpleMarkerSymbol>
		{
			public:

				CSimpleMarketSymbol();
				~CSimpleMarketSymbol();
				virtual void                 SetStyle(eSimpleMarkerStyle style);
				virtual void                 SetOutline(bool bOutline);
				virtual bool                 IsOutline() const;
				virtual Color                GetOutlineColor() const;
				virtual void                 SetOutlineColor( const Color &color);
				virtual double               GetOutlineSize() const;
				virtual void                 SetOutlineSize(double size);


				//IStreamSerialize
				virtual bool save(CommonLib::IWriteStream *pWriteStream) const;
				virtual bool load(CommonLib::IReadStream* pReadStream);


				//IXMLSerialize
				virtual bool saveXML(GisCommon::IXMLNode* pXmlNode) const;
				virtual bool load(const GisCommon::IXMLNode* pXmlNode);


				virtual void  DrawGeometryEx(IDisplay* pDisplay, const GPoint* points, const int* polyCounts, size_t polyCount);
				virtual void  QueryBoundaryRectEx(IDisplay* pDisplay, const GPoint* points, const int* polyCounts, size_t polyCount,   GRect &rect) const;
				virtual void  Prepare(IDisplay* pDisplay);


			private:

				eSimpleMarkerStyle m_style;
				bool              isOutline_;
				Color             outlineColor_;
				double            outlineSize_;

				CPen  m_pen;
				CPen  m_outlinePen;
				Brush m_brush;
		};
	}
}

#endif