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
			CBrush(eBrushType type, const Color& color, const Color& bgColor, CBitmap* pTexture, bool bRelease = false);
			~CBrush();
		
			void setType(eBrushType type);
			void setColor(const Color& color);
			void setBgColor(const Color& color);
			void setTexture(CBitmap* pTexture, bool bRelease = false);


			eBrushType GetType() const;
			const Color& GetColor() const;
			const Color& GetBgColor() const;
			CBitmap* GetTexture() const;


			bool save(CommonLib::IWriteStream *pStream) const;
			bool load(CommonLib::IReadStream *pStream);


			bool saveXML(GisCommon::IXMLNode* pXmlNode, const wchar_t *pszName = L"Brush") const;
			bool load(const  GisCommon::IXMLNode* pXmlNode, const wchar_t *pszName = L"Brush");


		private:
			eBrushType  m_type;
			Color      m_color;
			Color      m_bgColor;
			CBitmap*    m_pTexture;
			bool m_bRelease;
			
		};
	}
}
#endif