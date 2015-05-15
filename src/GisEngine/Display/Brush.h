#ifndef GIS_ENGINE_DISPLAY_BRUSH_H_
#define GIS_ENGINE_DISPLAY_BRUSH_H_
#include "GraphTypes.h"
#include "Bitmap.h"
namespace GisEngine
{
	namespace Display
	{
		class CBrush
		{
		public:
			CBrush(bool bIsNull = false);
			CBrush(BrushType type, const Color& color, const Color& bgColor, CBitmap* pTexture, bool bRelease = false);
			~CBrush();
		
			void setType(BrushType type);
			void setColor(const Color& color);
			void setBgColor(const Color& color);
			void setTexture(CBitmap* pTexture, bool bRelease = false);


			BrushType GetType() const;
			const Color& GetColor() const;
			const Color& GetBgColor() const;
			const CBitmap* GetTexture() const;


			void save(CommonLib::IWriteStream *pStream) const;
			void load(CommonLib::IReadStream *pStream);

		private:
			BrushType  m_type;
			Color      m_color;
			Color      m_bgColor;
			CBitmap*    m_pTexture;
			bool m_bRelease;
			
		};
	}
}
#endif