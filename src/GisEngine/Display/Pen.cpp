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
		CPen::CPen(ePenType penType, const Color& color, GUnits nWidth, eCapType capType, eJoinType joinType,
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

		void CPen::setPenType(ePenType penType)
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
		void CPen::setCapType(eCapType capType)
		{
			m_capType = capType;
		}
		void CPen::setJoinType(eJoinType joinType)
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


		ePenType CPen::getPenType() const
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
		eCapType CPen::getCapType() const
		{
			return m_capType;
		}
		eJoinType CPen::getJoinType() const
		{
			return m_joinType;
		}
		CBitmap* CPen::getTexture() const
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


		bool CPen::save(CommonLib::IWriteStream *pStream) const
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
			return true;
		}
		bool CPen::load(CommonLib::IReadStream *pStream)
		{
			m_type = (ePenType)pStream->readByte();
			m_color.load(pStream);
			m_joinType = (eJoinType)pStream->readByte();
			pStream->read(m_nWidth);
			m_capType = (eCapType)pStream->readByte();
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
			return true;
		}


		bool CPen::saveXML(GisCommon::IXMLNode* pXmlNode, const wchar_t *pszName) const
		{
			pXmlNode->AddPropertyInt16U(L"PenType", uint16(m_type));
			pXmlNode->AddPropertyInt16U(L"JoinType", uint16(m_joinType));
			pXmlNode->AddProperty("Width", CommonLib::CVariant(m_nWidth));
			pXmlNode->AddPropertyInt16U(L"CapType", uint16(m_capType));

			m_color.saveXML(pXmlNode);

			if(m_pTexture && m_bRelease) 
			{
				pXmlNode->AddPropertyBool(L"Release", m_bRelease);
				m_pTexture->saveXML(pXmlNode);
			}
			else
				pXmlNode->AddPropertyBool(L"Release", false);

			if(m_vecTemplates.size())
			{
				CommonLib::MemoryStream stream;
				stream.write((uint32)m_vecTemplates.size());
				for (size_t i = 0; i < m_vecTemplates.size(); ++i)
				{
					stream.write(m_vecTemplates[i].first);
					stream.write(m_vecTemplates[i].second);
				}

				CommonLib::CBlob blob(stream.buffer(), stream.size(), true, NULL);

				GisCommon::IXMLNodePtr pBlobNode = pXmlNode->CreateChildNode(L"Templates");
				pBlobNode->SetBlobCDATA(blob);

			}
			
			return true;

		}
		bool CPen::load(GisCommon::IXMLNode* pXmlNode, const wchar_t *pszName)
		{
			GisCommon::IXMLNodePtr pPenNode = pXmlNode->GetChild(pszName);
			if(!pPenNode.get())
				return false;

			m_type = (ePenType)pPenNode->GetPropertyInt16U(L"PenType", PenTypeNull);
			m_joinType = (eJoinType)pPenNode->GetPropertyInt16U(L"JoinType", JoinTypeMiter);
			m_capType = (eCapType)pPenNode->GetPropertyInt16U(L"CapType", CapTypeButt);
			CommonLib::CVariant* pWidthVar =  pPenNode->GetProperty(L"Width");
			if(pWidthVar)
				m_nWidth = pWidthVar->Get<GUnits>();
			
			m_bRelease = pPenNode->GetPropertyBool(L"Release", false);
			if(m_bRelease) 
			{
				m_pTexture = new CBitmap();
				m_pTexture->load(pPenNode.get());
			}
	

			GisCommon::IXMLNodePtr pTemplatesNode = pXmlNode->GetChild(L"Templates");
			if(pTemplatesNode.get())
			{
				CommonLib::CBlob& blob = pTemplatesNode->GetBlobCDATA();
				CommonLib::FxMemoryReadStream stream;
				stream.attach(blob.buffer(), blob.size());

				CommonLib::FxMemoryReadStream *pStream = &stream;
				uint32 nSize = 0;
				SAFE_READ(pStream, nSize)
				if(nSize)
				{
					m_vecTemplates.reserve(nSize);
					for (size_t i = 0; i < nSize; ++i)
					{
						SAFE_READ(pStream, m_vecTemplates[i].first)
						SAFE_READ(pStream, m_vecTemplates[i].second)
					}
				}
			}
			return true;
		}
	}
}