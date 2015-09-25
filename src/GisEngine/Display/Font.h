#ifndef GIS_ENGINE_DISPLAY_FONT_H_
#define GIS_ENGINE_DISPLAY_FONT_H_
#include "GraphTypes.h"
#include "Bitmap.h"
namespace GisEngine
{
	namespace Display
	{
		class CFont
		{
		public:
			CFont();
			CFont(const CommonLib::CString& sFace, GUnits nSize, short nCharSet, Color color, int  nStyle, double dOrientation,
				Color    bgColor, bool  bIsTransparent, eTextVAlignment vAlignmen, eTextHAlignment hAlignment, GUnits nHaloSize);


			void setFace(const CommonLib::CString& sFace);
			void setSize(GUnits nSize);
			void setColor(const Color& color);
			void setCharSet(short nCharSet);
			void setStyle(int nStyle);
			void setOrientation(double dOrientation);
			void setBgColor(const Color& color);
			void setTransparent(bool bIsTransparent);
			void setTextVAlignment(eTextVAlignment vAlignmen);
			void setTextHAlignment(eTextHAlignment hAlignment);
			void setHaloSize(GUnits nHaloSize);


			const CommonLib::CString&  getFace() const;
			GUnits getSize() const;
			const Color&  getColor() const;
			short getCharSet() const;
			int getStyle() const;
			GUnits getOrientation() const;
			const Color& getBgColor() const;
			bool getTransparent() const;
			eTextVAlignment getTextVAlignment() const;
			eTextHAlignment getTextHAlignment() const;
			GUnits getHaloSize() const;


			bool save(CommonLib::IWriteStream *pStream) const;
			bool load(CommonLib::IReadStream *pStream);

			bool saveXML(GisCommon::IXMLNode* pXmlNode, const wchar_t *pszName = L"Font") const;
			bool load(const GisCommon::IXMLNode* pXmlNode, const wchar_t *pszName = L"Font");

		private:


			CommonLib::CString      m_sFace;
			GUnits         m_nSize;
			short          m_nCharSet;
			Color          m_color;
			int            m_nStyle;
			double         m_dOrientation;
			Color          m_bgColor;
			bool           m_bIsTransparent;
			eTextVAlignment m_vAlignment;
			eTextHAlignment m_hAlignment;
			GUnits         m_nHaloSize;


		};
	}
}
#endif