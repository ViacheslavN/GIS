#include "stdafx.h"
#include "Brush.h"


namespace GisEngine
{
	namespace Display
	{
		CBrush::CBrush(bool bIsNull)
		{
			m_type = bIsNull ? BrushTypeNull : BrushTypeSolid;
			m_color = Color::White;
			m_bgColor = Color::Transparent;
			m_pTexture = NULL;
			m_bRelease = false;
		}
		CBrush::CBrush(BrushType type, const Color& color, const Color& bgColor, CBitmap* pTexture, bool bRelease) :
			m_type(type), m_color(color), m_bgColor(bgColor), m_pTexture(pTexture), m_bRelease(bRelease)
		{

		}
		CBrush::~CBrush()
		{
			if(m_bRelease && m_pTexture)
			{
				delete m_pTexture;
				m_pTexture = NULL;
			}
		}
		void CBrush::setType(BrushType type)
		{
			m_type = type;
		}
		void CBrush::setColor(const Color& color)
		{
			m_color = color;
		}
		void CBrush::setBgColor(const Color& color)
		{
			m_bgColor = color;
		}
		void CBrush::setTexture(CBitmap* pTexture, bool bRelease)
		{
			if(m_bRelease && m_pTexture)
			{
				delete m_pTexture;
			}
			m_pTexture = pTexture;
			m_bRelease = bRelease;
		}



		BrushType CBrush::GetType() const
		{
			return m_type;
		}
		const Color& CBrush::GetColor() const
		{
			return m_color;
		}
		const Color& CBrush::GetBgColor() const
		{
			return m_bgColor;
		}
		const CBitmap* CBrush::GetTexture() const
		{
			return m_pTexture;
		}


		void CBrush::save(CommonLib::IWriteStream *pStream) const
		{
			pStream->write((byte)m_type);
			m_color.save(pStream);
			m_bgColor.save(pStream);
			if(m_pTexture && m_bRelease) 
			{
				pStream->write(true);
				m_pTexture->save(pStream);
			}
			else
				pStream->write(false);
		}
		void CBrush::load(CommonLib::IReadStream *pStream)
		{
			m_type = (BrushType)pStream->readByte();
			m_color.load(pStream);
			m_bgColor.load(pStream);
			m_bRelease = pStream->readBool();
			if(m_bRelease)
			{
				m_pTexture = new CBitmap();
				m_pTexture->load(pStream);
			}
		}
	}
}