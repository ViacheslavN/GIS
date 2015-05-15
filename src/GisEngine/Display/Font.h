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
			CFont(bool bIsNull = false);
			CFont(const CommonLib::str_t& sFace, GUnits nSize, GUnits nHaloSize, );

			void save(CommonLib::IWriteStream *pStream) const;
			void load(CommonLib::IReadStream *pStream);

		private:


			CommonLib::str_t      m_sFace;
			GUnits         m_nSize;
			GUnits         m_nHaloSize;
			short          m_nCharSet;
			Color          m_color;
			int            m_nStyle;
			double         m_dOrientation;
			Color          m_bgColor;
			bool           m_bIstransparent;
			TextVAlignment m_vAlignment;
			TextHAlignment m_hAlignment;


		};
	}
}
#endif