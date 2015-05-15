#include "stdafx.h"
#include "Pen.h"

namespace GisEngine
{
	namespace Display
	{
		CPen::CPen(bool bIsNull)
		{
			m_type = bIsNull ? PenTypeNull : PenTypeSolid;
			m_color = Color::Black;
			m_nWidth = 1;
			m_capType = CapTypeRound;
			m_joinType = JoinTypeRound;
			m_pTexture = 0;
			m_nOffset = 0;
			m_bRelease = false;
		}
		CPen::CPen(PenType penType, const Color& color, GUnits nWidth, CapType capType, JoinType joinType,
			GUnits nOffset, CBitmap* pTexture,  bool bRelease) : m_type(penType), m_color(color), m_nWidth(nWidth), m_capType(capType),
			m_joinType(joinType), m_pTexture(pTexture), m_nOffset(nOffset), m_bRelease(bRelease)

		{
		}
		CPen::~CPen()
		{
			if(m_bRelease && m_pTexture)
			{
				delete m_pTexture;
				m_pTexture = NULL;
			}

		}

		void CPen::setPenType(PenType penType)
		{
			m_type = penType;
		}
		void CPen::setColor(const Color& color)
		{
			m_color = color;
		}
		void CPen::setWidth(GUnits nWidth)
		{
			m_nWidth = nWidth;
		}
		void CPen::setCapType(CapType capType)
		{
			m_capType = capType;
		}
		void CPen::setJoinType(JoinType joinType)
		{
			m_joinType = joinType;
		}
		void CPen::setTexture(CBitmap* pTexture, bool bRelease)
		{
			if(m_bRelease && m_pTexture)
			{
				delete m_pTexture;
				m_pTexture = NULL;
	 		}

			m_pTexture = pTexture;
			m_bRelease = bRelease;
		}
		void CPen::setOffset(GUnits nOffset)
		{
			m_nWidth = nOffset;
		}
		void CPen::addTemplate(GUnits par, GUnits tem)
		{
			m_vecTemplates.push_back(std::make_pair(par, tem));
		}
		void CPen::clearTmplates()
		{
			m_vecTemplates.clear();
		}


		PenType CPen::getPenType() const
		{
			return m_type;
		}
		const Color& CPen::getColor() const
		{
			return m_color;
		}
		GUnits CPen::getWidth() const
		{
			return m_nWidth;
		}
		CapType CPen::getCapType() const
		{
			return m_capType;
		}
		JoinType CPen::getJoinType() const
		{
			return m_joinType;
		}
		const CBitmap* CPen::getTexture() const
		{
			return m_pTexture;
		}
		GUnits CPen::getOffset() const
		{
			return m_nOffset;
		}
		const TPenTemplates& CPen::getTemplates() const
		{
			return m_vecTemplates;
		}


		void CPen::save(CommonLib::IWriteStream *pStream) const
		{
			pStream->write(byte(m_type));
			m_color.save(pStream);
			pStream->write(byte(m_joinType));
			pStream->write(m_nWidth);
			pStream->write(byte(m_capType));
			if(m_pTexture && m_bRelease) 
			{
				pStream->write(true);
				m_pTexture->save(pStream);
			}
			else
				pStream->write(false);
			pStream->write((uint32)m_vecTemplates.size());
			for (size_t i = 0; i < m_vecTemplates.size(); ++i)
			{
				pStream->write(m_vecTemplates[i].first);
				pStream->write(m_vecTemplates[i].second);
			}

		}
		void CPen::load(CommonLib::IReadStream *pStream)
		{
			m_type = (PenType)pStream->readByte();
			m_color.load(pStream);
			m_joinType = (JoinType)pStream->readByte();
			pStream->read(m_nWidth);
			m_capType = (CapType)pStream->readByte();
			m_bRelease = pStream->readBool();
			if(m_bRelease)
			{
				m_pTexture = new CBitmap();
				m_pTexture->load(pStream);
			}
			uint32 nSize = pStream->readIntu32();
			for (uint32 i = 0; i < nSize; ++i)
			{
				GUnits val1, val2;
				pStream->read(val1);
				pStream->read(val2);
				m_vecTemplates.push_back(std::make_pair(val1, val2));
			}
		}
	}
}