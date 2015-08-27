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
			return true;
		}
		bool CBrush::load(CommonLib::IReadStream *pStream)
		{
			byte nType = 0;
			SAFE_READ_RES(pStream, nType);

			m_type = (eBrushType)nType;
			if(!m_color.load(pStream))
				return false;
			if(!m_bgColor.load(pStream))
				return false;
			SAFE_READ_RES(pStream, m_bRelease);
			if(m_bRelease)
			{
				m_pTexture = new CBitmap();
				return m_pTexture->load(pStream);
			}
			return true;
		}



		bool  CBrush::saveXML(GisCommon::IXMLNode* pXmlNode, const wchar_t *pszName) const
		{
			GisCommon::IXMLNodePtr pBrushNode = pXmlNode->CreateChildNode(pszName);

			pBrushNode->AddPropertyInt16U(L"Type", m_type);
			m_color.saveXML(pBrushNode.get());
			m_bgColor.saveXML(pBrushNode.get());
			if(m_pTexture && m_bRelease) 
			{
				pBrushNode->AddPropertyBool(L"Release", m_bRelease);
				return m_pTexture->saveXML(pBrushNode.get());
			}
			else
				pBrushNode->AddPropertyBool(L"Release", false);
			return true;
		}
		bool  CBrush::load(GisCommon::IXMLNode* pXmlNode, const wchar_t *pszName)
		{
			GisCommon::IXMLNodePtr pBrushNode = pXmlNode->GetChild(pszName);
			if(!pBrushNode.get())
				return false;
			m_color.load(pBrushNode.get());
			m_bgColor.load(pBrushNode.get());
			m_type = (eBrushType)pBrushNode->GetPropertyInt16(L"Type", BrushTypeNull);
			m_bRelease = pBrushNode->GetPropertyBool(L"Release", false);
			if(m_bRelease)
				return m_pTexture->load(pBrushNode.get());
			return true;
		}

	}
}