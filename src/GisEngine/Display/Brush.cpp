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
		CBrush::CBrush(eBrushType type, const Color& color, const Color& bgColor, CBitmap* pTexture, bool bRelease) :
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
		void CBrush::setType(eBrushType type)
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



		eBrushType CBrush::GetType() const
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
		CBitmap* CBrush::GetTexture() const
		{
			return m_pTexture;
		}


		bool CBrush::save(CommonLib::IWriteStream *pStream) const
		{

			CommonLib::CWriteMemoryStream stream;
			stream.write((byte)m_type);
			m_color.save(&stream);
			m_bgColor.save(&stream);
			if(m_pTexture && m_bRelease) 
			{
				stream.write(true);
				m_pTexture->save(&stream);
			}
			else
				stream.write(false);

			pStream->write(&stream);
			return true;
		}
		bool CBrush::load(CommonLib::IReadStream *pStream)
		{
			byte nType = 0;
			CommonLib::FxMemoryReadStream stream;
			SAFE_READ(pStream->save_read(&stream, true))
			SAFE_READ(stream.save_read(nType))
			m_type = (eBrushType)nType;
			SAFE_READ(m_color.load(&stream))
			SAFE_READ(m_bgColor.load(&stream))
			SAFE_READ(stream.save_read(m_bRelease))
			if(m_bRelease)
			{
				m_pTexture = new CBitmap();
				return m_pTexture->load(&stream);
			}
			return true;
		}



		bool  CBrush::saveXML(GisCommon::IXMLNode* pXmlNode, const wchar_t *pszName) const
		{
			GisCommon::IXMLNodePtr pBrushNode = pXmlNode->CreateChildNode(pszName);

			pBrushNode->AddPropertyInt16U(L"Type", m_type);
			m_color.saveXML(pBrushNode.get(), L"color");
			m_bgColor.saveXML(pBrushNode.get(), L"BgColor");
			if(m_pTexture && m_bRelease) 
			{
				pBrushNode->AddPropertyBool(L"Release", m_bRelease);
				return m_pTexture->saveXML(pBrushNode.get());
			}
			else
				pBrushNode->AddPropertyBool(L"Release", false);
			return true;
		}
		bool  CBrush::load(const GisCommon::IXMLNode* pXmlNode, const wchar_t *pszName)
		{
			GisCommon::IXMLNodePtr pBrushNode = pXmlNode->GetChild(pszName);
			if(!pBrushNode.get())
				return false;
			m_color.load(pBrushNode.get(), L"color");
			m_bgColor.load(pBrushNode.get(), L"BgColor");
			m_type = (eBrushType)pBrushNode->GetPropertyInt16(L"Type", BrushTypeNull);
			m_bRelease = pBrushNode->GetPropertyBool(L"Release", false);
			if(m_bRelease)
				return m_pTexture->load(pBrushNode.get());
			return true;
		}

	}
}