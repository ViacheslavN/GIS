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

				typedef CMarkerSymbolBase<ISimpleMarkerSymbol> TSymbolBase;

				CSimpleMarketSymbol();
				~CSimpleMarketSymbol();
				virtual void                 SetStyle(eSimpleMarkerStyle style);
				virtual void                 SetOutline(bool bOutline);
				virtual bool                 IsOutline() const;
				virtual Color                GetOutlineColor() const;
				virtual void                 SetOutlineColor( const Color &color);
				virtual double               GetOutlineSize() const;
				virtual void                 SetOutlineSize(double size);

				virtual bool CanDraw(CommonLib::CGeoShape* pShape) const;
				//IStreamSerialize
				virtual bool save(CommonLib::IWriteStream *pWriteStream) const;
				virtual bool load(CommonLib::IReadStream* pReadStream);


				//IXMLSerialize
				virtual bool saveXML(GisCommon::IXMLNode* pXmlNode) const;
				virtual bool load(const GisCommon::IXMLNode* pXmlNode);


				virtual void  DrawGeometryEx(IDisplay* pDisplay, const GPoint* points, const int* polyCounts, size_t polyCount);
				virtual void  QueryBoundaryRectEx(IDisplay* pDisplay, const GPoint* points, const int* polyCounts, size_t polyCount,   GRect &rect) const;
				virtual void  Prepare(IDisplay* pDisplay);
					virtual void DrawDirectly(IDisplay* display, const GPoint* lpPoints, const int *lpPolyCounts, int nCount );

			private:
				void DrawCircle(const GPoint& point, IGraphics* pGraphics);
				void DrawSquare(const GPoint& point, IGraphics* pGraphics);
				void DrawDiamond(const GPoint& point, IGraphics* pGraphics);
				void DrawX(const GPoint& point, IGraphics* pGraphics);
				void DrawCross(const GPoint& point, IGraphics* pGraphics);
			private:

				eSimpleMarkerStyle m_style;
				bool              m_bIsOutline;
				Color             m_OutlineColor;
				double            m_dOutlineSize;

				CPen  m_pen;
				CPen  m_outlinePen;
				CBrush m_brush;
		};
	}
}

#endif