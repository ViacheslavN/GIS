#ifndef GIS_ENGINE_DISPLAY_PEN_H_
#define GIS_ENGINE_DISPLAY_PEN_H_
#include "GraphTypes.h"
#include "Bitmap.h"
#include <vector>
namespace GisEngine
{
	namespace Display
	{
		typedef std::vector<std::pair<GUnits, GUnits> > TPenTemplates;
		class CPen
		{
		public:
			CPen(bool bisNull = false);
			CPen(PenType penType, const Color& color, GUnits nWidth, CapType capType, JoinType joinType,
				GUnits nOffset, CBitmap* pTexture, bool bRelease = false);
			~CPen();

			void setPenType(PenType penType);
			void setColor(const Color& color); 
			void setWidth(GUnits nWidth);
			void setCapType(CapType capType);
			void setJoinType(JoinType joinType);
			void setTexture(CBitmap* pTexture, bool bRelease = false);
			void setOffset(GUnits nOffset);
			void addTemplate(GUnits par, GUnits tem);
			void clearTmplates();


			PenType getPenType() const;
			const Color& getColor() const;
			GUnits getWidth() const;
			CapType getCapType() const;
			JoinType getJoinType() const;
			CBitmap* getTexture() const;
			GUnits getOffset() const;
			const TPenTemplates& getTemplates() const;


			void save(CommonLib::IWriteStream *pStream) const;
			void load(CommonLib::IReadStream *pStream);

		private:

			PenType  m_type;
			Color    m_color;
			GUnits   m_nWidth;
			CapType  m_capType;
			JoinType m_joinType;
			CBitmap*  m_pTexture;
			GUnits    m_nOffset;
			TPenTemplates m_vecTemplates;
			bool m_bRelease;
		};

	}
}
#endif