#include "stdafx.h"
#include "Font.h"

namespace GisEngine
{
	namespace Display
	{

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

		CFont::CFont()
		{
			m_sFace = L"Arial";
			m_nCharSet = 0;
			m_color = Color::Black;
			m_nStyle = FontStyleRegular;
			m_dOrientation = 0;
			m_bgColor = Color::Transparent;
			m_vAlignment = TextVAlignmentTop;
			m_hAlignment = TextHAlignmentLeft;
			m_bIsTransparent = true;
			m_nSize = 8;
			m_nHaloSize = 0;
		}
		CFont::CFont(const CommonLib::str_t& sFace, GUnits nSize, short nCharSet, Color color, int  nStyle, double dOrientation,
			Color    bgColor, bool  bIsTransparent, TextVAlignment vAlignmen, TextHAlignment hAlignment, GUnits nHaloSize) :
			m_sFace(sFace), m_nSize(nSize), m_nCharSet(nCharSet), m_nStyle(nStyle), m_color(color), m_dOrientation(dOrientation),
				m_bgColor(bgColor), m_vAlignment(vAlignmen), m_hAlignment(hAlignment), m_bIsTransparent(m_bIsTransparent),
				m_nHaloSize(nHaloSize)
		{

		}


		void CFont::setFace(const CommonLib::str_t& sFace)
		{
			m_sFace = sFace;
		}
		void CFont::setSize(GUnits nSize)
		{
			m_nSize = nSize;
		}
		void CFont::setColor(const Color& color)
		{
			m_color = color;
		}
		void  CFont::setCharSet(short nCharSet)
		{

		}
		void CFont::setStyle(int nStyle)
		{
			m_nStyle = nStyle;
		}
		void CFont::setOrientation(double dOrientation)
		{
			m_dOrientation = dOrientation;
		}
		void CFont::setBgColor(const Color& color)
		{
			m_bgColor = color;
		}
		void CFont::setTransparent(bool bIsTransparent)
		{
			m_bIsTransparent = bIsTransparent;
		}
		void CFont::setTextVAlignment(TextVAlignment vAlignmen)
		{
			m_vAlignment = vAlignmen;
		}
		void CFont::setTextHAlignment(TextHAlignment hAlignment)
		{
			m_hAlignment = hAlignment;
		}
		void CFont::setHaloSize(GUnits nHaloSize)
		{
			m_nHaloSize = nHaloSize;
		}


		const CommonLib::str_t&  CFont::getFace() const
		{
			return m_sFace;
		}
		GUnits CFont::getSize() const
		{
			return m_nSize;
		}
		const Color&  CFont::getColor() const
		{
			return m_color;
		}
		int CFont::getStyle() const
		{
			return m_nStyle;
		}
		short CFont::getCharSet() const
		{
			return m_nCharSet;
		}
		double CFont::getOrientation() const
		{
			return m_dOrientation;
		}
		const Color& CFont::getBgColor() const
		{
			return m_bgColor;
		}
		bool CFont::getTransparent() const
		{
			return m_bIsTransparent;
		}
		TextVAlignment CFont::getTextVAlignment() const
		{
			return m_vAlignment;
		}
		TextHAlignment CFont::getTextHAlignment() const
		{
			return m_hAlignment;
		}
		GUnits CFont::getHaloSize() const
		{
			return m_nHaloSize;
		}
 

		void CFont::save(CommonLib::IWriteStream *pStream) const
		{
			pStream->write(m_sFace);
			pStream->write(m_nSize);
			pStream->write(m_nCharSet);
			m_color.save(pStream);
			pStream->write(m_nStyle);
			pStream->write(m_dOrientation);
			m_bgColor.save(pStream);
			pStream->write(m_bIsTransparent);
			pStream->write((byte)m_vAlignment);
			pStream->write((byte)m_hAlignment);
			pStream->write(m_nHaloSize);
		}
		void CFont::load(CommonLib::IReadStream *pStream)
		{
			pStream->read(m_sFace);
			pStream->read(m_nSize);
			pStream->read(m_nCharSet);
			m_color.load(pStream);
			pStream->read(m_nStyle);
			pStream->read(m_dOrientation);
			m_bgColor.load(pStream);
			pStream->read(m_bIsTransparent);
			m_vAlignment = (TextVAlignment)pStream->readByte();
			m_hAlignment = (TextHAlignment)pStream->readByte();
			pStream->read(m_nHaloSize);
		}

	}
}