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
			CFont(const CommonLib::str_t& sFace, GUnits nSize, short nCharSet, Color color, int  nStyle, double dOrientation,
				Color    bgColor, bool  bIsTransparent, TextVAlignment vAlignmen, TextHAlignment hAlignment, GUnits nHaloSize);


			void setFace(const CommonLib::str_t& sFace);
			void setSize(GUnits nSize);
			void setColor(const Color& color);
			void setCharSet(short nCharSet);
			void setStyle(int nStyle);
			void setOrientation(double dOrientation);
			void setBgColor(const Color& color);
			void setTransparent(bool bIsTransparent);
			void setTextVAlignment(TextVAlignment vAlignmen);
			void setTextHAlignment(TextHAlignment hAlignment);
			void setHaloSize(GUnits nHaloSize);


			const CommonLib::str_t&  getFace() const;
			GUnits getSize() const;
			const Color&  getColor() const;
			short getCharSet() const;
			int getStyle() const;
			double getOrientation() const;
			const Color& getBgColor() const;
			bool getTransparent() const;
			TextVAlignment getTextVAlignment() const;
			TextHAlignment getTextHAlignment() const;
			GUnits getHaloSize() const;


			void save(CommonLib::IWriteStream *pStream) const;
			void load(CommonLib::IReadStream *pStream);

		private:


			CommonLib::str_t      m_sFace;
			GUnits         m_nSize;
			short          m_nCharSet;
			Color          m_color;
			int            m_nStyle;
			double         m_dOrientation;
			Color          m_bgColor;
			bool           m_bIsTransparent;
			TextVAlignment m_vAlignment;
			TextHAlignment m_hAlignment;
			GUnits         m_nHaloSize;


		};
	}
}
#endif