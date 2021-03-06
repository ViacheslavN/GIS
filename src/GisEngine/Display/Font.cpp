#include "stdafx.h"
#include "Font.h"

namespace GisEngine
{
	namespace Display
	{

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
		CFont::CFont(const CommonLib::CString& sFace, GUnits nSize, short nCharSet, Color color, int  nStyle, double dOrientation,
			Color    bgColor, bool  bIsTransparent, eTextVAlignment vAlignmen, eTextHAlignment hAlignment, GUnits nHaloSize) :
			m_sFace(sFace), m_nSize(nSize), m_nCharSet(nCharSet), m_nStyle(nStyle), m_color(color), m_dOrientation(dOrientation),
				m_bgColor(bgColor), m_vAlignment(vAlignmen), m_hAlignment(hAlignment), m_bIsTransparent(m_bIsTransparent),
				m_nHaloSize(nHaloSize)
		{

		}


		void CFont::setFace(const CommonLib::CString& sFace)
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
		void CFont::setTextVAlignment(eTextVAlignment vAlignmen)
		{
			m_vAlignment = vAlignmen;
		}
		void CFont::setTextHAlignment(eTextHAlignment hAlignment)
		{
			m_hAlignment = hAlignment;
		}
		void CFont::setHaloSize(GUnits nHaloSize)
		{
			m_nHaloSize = nHaloSize;
		}


		const CommonLib::CString&  CFont::getFace() const
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
		GUnits CFont::getOrientation() const
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
		eTextVAlignment CFont::getTextVAlignment() const
		{
			return m_vAlignment;
		}
		eTextHAlignment CFont::getTextHAlignment() const
		{
			return m_hAlignment;
		}
		GUnits CFont::getHaloSize() const
		{
			return m_nHaloSize;
		}
 

		bool CFont::save(CommonLib::IWriteStream *pStream) const
		{
			CommonLib::CWriteMemoryStream stream;
			stream.write(m_sFace);
			stream.write(m_nSize);
			stream.write(m_nCharSet);
			m_color.save(&stream);
			stream.write(m_nStyle);
			stream.write(m_dOrientation);
			m_bgColor.save(&stream);
			stream.write(m_bIsTransparent);
			stream.write((byte)m_vAlignment);
			stream.write((byte)m_hAlignment);
			stream.write(m_nHaloSize);

			pStream->write(&stream);
			return true;
		}
		bool CFont::load(CommonLib::IReadStream *pStream)
		{
			CommonLib::FxMemoryReadStream stream;

			SAFE_READ(pStream->save_read(&stream, true))

			SAFE_READ(stream.save_read(m_sFace))
			SAFE_READ(stream.save_read(m_nSize))
			SAFE_READ(stream.save_read(m_nCharSet))
			SAFE_READ(m_color.load(&stream))
			SAFE_READ(stream.save_read(m_nStyle))
			SAFE_READ(stream.save_read(m_dOrientation))
			SAFE_READ(m_bgColor.load(&stream))
			SAFE_READ(stream.save_read(m_bIsTransparent))

			byte vAlignment = 0;
			byte hAlignment = 0;
			SAFE_READ(stream.save_read(vAlignment))
			SAFE_READ(stream.save_read(hAlignment))

			m_vAlignment = (eTextVAlignment)vAlignment;
			m_hAlignment = (eTextHAlignment)hAlignment;
			stream.save_read(m_nHaloSize);
			return true;
		}
		bool CFont::saveXML(GisCommon::IXMLNode* pXmlNode, const wchar_t *pszName) const
		{
			GisCommon::IXMLNodePtr pNode = pXmlNode->CreateChildNode(pszName);
			pNode->AddPropertyString("face", m_sFace);
			pNode->AddPropertyDouble("Size", m_nSize);
			pNode->AddPropertyInt16("CharSet", m_nCharSet);
			pNode->AddPropertyInt32U("color", m_color.GetRGBA());
			pNode->AddPropertyDouble("Orientation", m_dOrientation);
			pNode->AddPropertyInt32U("BgColor", m_bgColor.GetRGBA());
			pNode->AddPropertyBool("Transparent", m_bIsTransparent);
			pNode->AddPropertyInt16("vAlignment", m_vAlignment);
			pNode->AddPropertyInt16("hAlignment", m_hAlignment);
			pNode->AddPropertyDouble("HaloSize", m_nHaloSize);
			return true;
		}
		bool CFont::load(const GisCommon::IXMLNode* pXmlNode, const wchar_t *pszName)
		{
			GisCommon::IXMLNodePtr pNode = pXmlNode->GetChild(pszName);
			if(pNode.get())
				return false;

			m_sFace = pNode->GetPropertyString("face", m_sFace);
			m_nSize = pNode->GetPropertyDouble("Size", m_nSize);
			m_nCharSet = pNode->GetPropertyInt16("CharSet", m_nCharSet);
			m_color = pNode->GetPropertyInt32U("color", m_color.GetRGBA());
			m_dOrientation = pNode->GetPropertyDouble("Orientation", m_dOrientation);
			m_bgColor = pNode->GetPropertyInt32U("BgColor", m_bgColor.GetRGBA());
			m_bIsTransparent = pNode->GetPropertyBool("Transparent", m_bIsTransparent);
			m_vAlignment = (eTextVAlignment)pNode->GetPropertyInt16("vAlignment", m_vAlignment);
			m_hAlignment = (eTextHAlignment)pNode->GetPropertyInt16("hAlignment", m_hAlignment);
			m_nHaloSize = pNode->GetPropertyDouble("HaloSize", m_nHaloSize);
			return true;
		}
	}
}