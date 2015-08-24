#ifndef GIS_ENGINE_DISPLAY_PEN_H_
#define GIS_ENGINE_DISPLAY_PEN_H_
#include "GraphTypes.h"
#include "Bitmap.h"
#include <vector>
#include "Common/GisEngineCommon.h"
namespace GisEngine
{
	namespace Display
	{
		typedef std::vector<std::pair<GUnits, GUnits> > TPenTemplates;
		class CPen
		{
		public:
			CPen(bool bisNull = false);
			CPen(ePenType penType, const Color& color, GUnits nWidth, eCapType capType, eJoinType joinType,
				GUnits nOffset, CBitmap* pTexture, bool bRelease = false);
			~CPen();

			void setPenType(ePenType penType);
			void setColor(const Color& color); 
			void setWidth(GUnits nWidth);
			void setCapType(eCapType capType);
			void setJoinType(eJoinType joinType);
			void setTexture(CBitmap* pTexture, bool bRelease = false);
			void setOffset(GUnits nOffset);
			void addTemplate(GUnits par, GUnits tem);
			void clearTmplates();


			ePenType getPenType() const;
			const Color& getColor() const;
			GUnits getWidth() const;
			eCapType getCapType() const;
			eJoinType getJoinType() const;
			CBitmap* getTexture() const;
			GUnits getOffset() const;
			const TPenTemplates& getTemplates() const;


			bool save(CommonLib::IWriteStream *pStream) const;
			bool load(CommonLib::IReadStream *pStream);

			bool saveXML(GisCommon::IXMLNode* pXmlNode, const wchar_t *pszName = L"Pen") const;
			bool load(GisCommon::IXMLNode* pXmlNode, const wchar_t *pszName = L"Pen");


		private:

			ePenType  m_type;
			Color    m_color;
			GUnits   m_nWidth;
			eCapType  m_capType;
			eJoinType m_joinType;
			CBitmap*  m_pTexture;
			GUnits    m_nOffset;
			TPenTemplates m_vecTemplates;
			bool m_bRelease;
		};

	}
}
#endif