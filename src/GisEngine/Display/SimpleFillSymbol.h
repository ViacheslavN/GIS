#ifndef GIS_ENGINE_DISPLAY_SIMPLE_FILL_SYMBOL_
#define GIS_ENGINE_DISPLAY_SIMPLE_FILL_SYMBOL_

#include "SymbolFillBase.h"
#include "Common/GisEngineCommon.h"

namespace GisEngine
{
	namespace Display
	{

		class CSimpleFillSymbol : public CSymbolFillBase<ISimpleFillSymbol>
		{
		public:


			typedef CSymbolFillBase<ISimpleFillSymbol> TSymbolBase;

			CSimpleFillSymbol();
			~CSimpleFillSymbol();

 
			//ISimpleFillSymbol
			virtual eSimpleFillStyle GetStyle() const;
			virtual void SetStyle(eSimpleFillStyle style);
			virtual Color GetBackgroundColor() const;
			virtual void  SetBackgroundColor( Color color );

			virtual void  DrawGeometryEx(IDisplay* pDisplay, const GPoint* points, const int* polyCounts, size_t polyCount);
			virtual void  QueryBoundaryRectEx(IDisplay* pDisplay, const GPoint* points, const int* polyCounts, size_t polyCount,   GRect &rect) const;
			virtual void  Prepare(IDisplay* pDisplay);

			virtual void FillRect(IDisplay* pDisplay, const Display::GRect& rect);
			virtual void DrawDirectly(IDisplay* display, const GPoint* lpPoints, const int *lpPolyCounts, int nCount );

			//IStreamSerialize
			virtual bool save(CommonLib::IWriteStream *pWriteStream) const;
			virtual bool load(CommonLib::IReadStream* pReadStream);


			//IXMLSerialize
			virtual bool saveXML(GisCommon::IXMLNode* pXmlNode) const;
			virtual bool load(const GisCommon::IXMLNode* pXmlNode);

		public:
			CPen m_Pen;
			eSimpleFillStyle m_FillStyle;
			std::auto_ptr<CBitmap> m_pBitmap;


		};
	}
}
#endif