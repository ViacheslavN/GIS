#ifndef GIS_ENGINE_DISPLAY_TEXT_SYMBOL_
#define GIS_ENGINE_DISPLAY_TEXT_SYMBOL_

#include "Display.h"
#include "SymbolBase.h"

namespace GisEngine
{
	namespace Display
	{

		class CTextSymbol : public CSymbolBase<ITextSymbol>
		{
			public:

				typedef CSymbolBase<ITextSymbol> TBase;
				CTextSymbol();
				virtual ~CTextSymbol();

				virtual GUnits GetAngle() const;
				virtual void   SetAngle( GUnits dAngle );
				virtual Color  GetColor() const;
				virtual void   SetColor(const Color &color );
				virtual const CFont& GetFont() const;
				virtual     CFont& GetFont() ;
				virtual void   SetFont(const  CFont& font );
				virtual void GetTextSize(IDisplay* display, const CommonLib::str_t& szText, GUnits *pxSize , GUnits *pySize, GUnits* baseLine) const;
				virtual GUnits GetSize() const;
				virtual void SetSize(GUnits size);
				virtual  const CommonLib::str_t& GetText() const;
				virtual void SetText(const CommonLib::str_t& szText );
				virtual ITextBackgroundPtr GetTextBackground() const;
				virtual void SetTextBackground( ITextBackground *bg );
				virtual int   GetTextDrawFlags() const;
				virtual void  SetTextDrawFlags(int flags);

				virtual bool CanDraw(const CommonLib::CGeoShape* geom) const;
				virtual void  DrawGeometryEx(IDisplay* pDisplay, const GPoint* points, const int* polyCounts, size_t polyCount);
				virtual void  QueryBoundaryRectEx(IDisplay* pDisplay, const GPoint* points, const int* polyCounts, size_t polyCount,   GRect &rect) const;
				virtual void  Prepare(IDisplay* pDisplay);


				//IStreamSerialize
				bool save(CommonLib::IWriteStream *pWriteStream) const;
				bool load(CommonLib::IReadStream* pReadStream);
			
				//IXMLSerialize
				bool saveXML(GisCommon::IXMLNode* pXmlNode) const;
				bool load(GisCommon::IXMLNode* pXmlNode);
				

			private:
				void PolygonCenterPoint(const CommonLib::CGeoShape* geom, GisXYPoint* pt);
				void QueryBoundaryRectEx1(IDisplay* pDisplay, const GPoint& point, GRect& rect) const;
				void draw_background(IDisplay* pDisplay, const GPoint& pt);
		private:
				CFont m_Font;
				CommonLib::str_t m_sText;
				ITextBackgroundPtr m_pTextBg;
				int   m_nTextDrawFlags;
		};
	}
}
#endif