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

			CommonLib::CWriteMemoryStream stream;

			stream.write(byte(m_type));
			m_color.save(&stream);
			stream.write(byte(m_joinType));
			stream.write(m_nWidth);
			stream.write(byte(m_capType));
			if(m_pTexture && m_bRelease) 
			{
				stream.write(true);
				m_pTexture->save(&stream);
			}
			else
				stream.write(false);
			stream.write((uint32)m_vecTemplates.size());
			for (size_t i = 0; i < m_vecTemplates.size(); ++i)
			{
				stream.write(m_vecTemplates[i].first);
				stream.write(m_vecTemplates[i].second);
			}
			pStream->write(&stream);
			return true;
		}
		bool CPen::load(CommonLib::IReadStream *pStream)
		{
			CommonLib::FxMemoryReadStream stream;
	 		SAFE_READ(pStream->save_read(&stream, true))
		

			byte type = 0;
			byte joinType = 0;
			byte capType = 0;
			SAFE_READ(stream.save_read(type))
			m_type = (ePenType)type;
			SAFE_READ(m_color.load(&stream))
			SAFE_READ(stream.save_read(joinType))
			m_joinType = (eJoinType)joinType;
			SAFE_READ(stream.save_read(m_nWidth))
			SAFE_READ(stream.save_read(capType))
			m_capType = (eCapType)capType;
			SAFE_READ(stream.save_read(m_bRelease))
			if(m_bRelease)
			{
				m_pTexture = new CBitmap();
				m_pTexture->load(&stream);
			}
			uint32 nCount = 0;
			SAFE_READ(stream.save_read(nCount))
			for (uint32 i = 0; i < nCount; ++i)
			{
				GUnits val1, val2;
				SAFE_READ(stream.save_read(val1))
				SAFE_READ(stream.save_read(val2))
				m_vecTemplates.push_back(std::make_pair(val1, val2));
			}
			return true;
		}


		bool CPen::saveXML(GisCommon::IXMLNode* pNode, const wchar_t *pszName) const
		{

			GisCommon::IXMLNodePtr pPenNode = pNode->CreateChildNode(pszName);
			pPenNode->AddPropertyInt16U(L"PenType", uint16(m_type));
			pPenNode->AddPropertyInt16U(L"JoinType", uint16(m_joinType));
			pPenNode->AddPropertyDouble("Width", m_nWidth);
			pPenNode->AddPropertyInt16U(L"CapType", uint16(m_capType));

			m_color.saveXML(pPenNode.get());

			if(m_pTexture && m_bRelease) 
			{
				pPenNode->AddPropertyBool(L"Release", m_bRelease);
				m_pTexture->saveXML(pPenNode.get());
			}
			else
				pPenNode->AddPropertyBool(L"Release", false);

			if(m_vecTemplates.size())
			{
				CommonLib::CWriteMemoryStream stream;
				stream.write((uint32)m_vecTemplates.size());
				for (size_t i = 0; i < m_vecTemplates.size(); ++i)
				{
					stream.write(m_vecTemplates[i].first);
					stream.write(m_vecTemplates[i].second);
				}

				CommonLib::CBlob blob(stream.buffer(), stream.size(), true, NULL);

				GisCommon::IXMLNodePtr pBlobNode = pPenNode->CreateChildNode(L"Templates");
				pBlobNode->SetBlobCDATA(blob);

			}
			
			return true;

		}
		bool CPen::load(const GisCommon::IXMLNode* pXmlNode, const wchar_t *pszName)
		{
			GisCommon::IXMLNodePtr pPenNode = pXmlNode->GetChild(pszName);
			if(!pPenNode.get())
				return false;

			m_type = (ePenType)pPenNode->GetPropertyInt16U(L"PenType", PenTypeNull);
			m_joinType = (eJoinType)pPenNode->GetPropertyInt16U(L"JoinType", JoinTypeMiter);
			m_capType = (eCapType)pPenNode->GetPropertyInt16U(L"CapType", CapTypeButt);
			m_nWidth = (GUnits)pPenNode->GetPropertyDouble(L"Width", 1);
			m_bRelease = pPenNode->GetPropertyBool(L"Release", false);
			m_color.load(pPenNode.get());
			if(m_bRelease) 
			{
				m_pTexture = new CBitmap();
				m_pTexture->load(pPenNode.get());
			}
	

			GisCommon::IXMLNodePtr pTemplatesNode = pXmlNode->GetChild(L"Templates");
			if(pTemplatesNode.get())
			{
				CommonLib::CBlob blob; 
				pTemplatesNode->GetBlobCDATA(blob);
				CommonLib::FxMemoryReadStream stream;
				stream.attachBuffer(blob.buffer(), blob.size());

				CommonLib::FxMemoryReadStream *pStream = &stream;
				uint32 nSize = 0;
				SAFE_READ(pStream->save_read(nSize))
				if(nSize)
				{
					m_vecTemplates.reserve(nSize);
					for (size_t i = 0; i < nSize; ++i)
					{
						SAFE_READ(pStream->save_read(m_vecTemplates[i].first))
						SAFE_READ(pStream->save_read(m_vecTemplates[i].second))
					}
				}
			}
			return true;
		}
	}
}