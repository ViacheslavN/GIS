#include "stdafx.h"
#include "XMLNode.h"
#include "XMLUtils.h"
namespace GisEngine
{
	namespace GisCommon
	{
		CXMLNode::CXMLNode(IXMLNode *pParent, const CommonLib::str_t& sName)
		{
			m_pParent = pParent;
			m_sName = sName;
		}
		CXMLNode::~CXMLNode()
		{
			 
		}
		IXMLNodePtr CXMLNode::GetParent() const
		{
			return m_pParent;
		}
		void   CXMLNode::AddChildNode(IXMLNode* child)
		{
			if(!child)
				return;

			m_Nodes.push_back(IXMLNodePtr(child));
			m_NodeByName.insert(std::make_pair(child->GetName(), m_Nodes.size() - 1));

		}
		IXMLNodePtr	CXMLNode::CreateChildNode(const CommonLib::str_t& sName)
		{
			if(sName.isEmpty())
			 return IXMLNodePtr();
			IXMLNodePtr pNode(new CXMLNode(this, sName));

			AddChildNode(pNode.get());
			return pNode;
		}
		uint32		CXMLNode::GetChildCnt() const
		{
			return m_Nodes.size();
		}
		IXMLNodePtr	CXMLNode::GetChild(uint32 nIndex) const
		{
			if(m_Nodes.size() > nIndex)
				return m_Nodes[nIndex];

			return IXMLNodePtr();
		}
		IXMLNodePtr	CXMLNode::GetChild(const wchar_t *pszName)
		{
			TNodesByName::iterator it = m_NodeByName.find(pszName);
			if(it == m_NodeByName.end())
				return IXMLNodePtr();
			 return GetChild(it->second);
		}

		const CommonLib::str_t&  CXMLNode::GetName() const
		{
			return m_sName;
		}
		void   CXMLNode::SetName( const CommonLib::str_t& name)
		{
			m_sName = name;
		}

		const CommonLib::str_t&  CXMLNode::GetText() const
		{
				return m_sText;
		}
		void   CXMLNode::SetText(const CommonLib::str_t& sText)
		{
			m_sText = sText;
		}

		const CommonLib::str_t&   CXMLNode::GetCDATA() const
		{
			return m_sCAData;
		}
		void   CXMLNode::SetCDATA(const   CommonLib::str_t& cdata)
		{
			m_sCAData = cdata;
		}

		void CXMLNode::GetBlobCDATA(CommonLib::CBlob& data) const
		{
			 string_to_blob(m_sCAData, data);
		}
		void   CXMLNode::SetBlobCDATA(const CommonLib::CBlob &data)
		{
			blob_to_string(data, m_sCAData);
		}


		void	CXMLNode::AddProperty(const CommonLib::str_t& sName, const CommonLib::CVariant& val)
		{
			CommonLib::ToStringVisitor vis;
			CommonLib::apply_visitor<CommonLib::ToStringVisitor>(val, vis);
			AddPropertyString(sName, vis); 

		}
		void	CXMLNode::AddPropertyInt16(const CommonLib::str_t& sName, int16 value)
		{
			CommonLib::ToStringVisitor vis;
			vis.Visit(value);
			AddPropertyString(sName, vis); 
		}
		void	CXMLNode::AddPropertyInt16U(const CommonLib::str_t& sName, uint16 value)
		{
			CommonLib::ToStringVisitor vis;
			vis.Visit(value);
			AddPropertyString(sName, vis); 
		}
		void	CXMLNode::AddPropertyInt32(const CommonLib::str_t& sName, int32 value)
		{
			CommonLib::ToStringVisitor vis;
			vis.Visit(value);
			AddPropertyString(sName, vis); 
		}
		void	CXMLNode::AddPropertyInt32U(const CommonLib::str_t& sName, uint32 value)
		{
			CommonLib::ToStringVisitor vis;
			vis.Visit(value);
			AddPropertyString(sName, vis); 
		}
		void	CXMLNode::AddPropertyInt64(const CommonLib::str_t& sName, int64 value)
		{
			CommonLib::ToStringVisitor vis;
			vis.Visit(value);
			AddPropertyString(sName, vis); 
		}
		void	CXMLNode::AddPropertyIntU64(const CommonLib::str_t& sName, uint64 value)
		{
			CommonLib::ToStringVisitor vis;
			vis.Visit(value);
			AddPropertyString(sName, vis); 
		}
		void	CXMLNode::AddPropertyDouble(const CommonLib::str_t& sName, double value)
		{
			CommonLib::ToStringVisitor vis;
			vis.Visit(value);
			AddPropertyString(sName, vis); 
		}
		void    CXMLNode::AddPropertyBool(const CommonLib::str_t& sName, bool value)
		{
			CommonLib::ToStringVisitor vis;
			vis.Visit(value);
			AddPropertyString(sName, vis); 
		}
		void	CXMLNode::AddPropertyString(const CommonLib::str_t&sName , const CommonLib::str_t& sValue)
		{
			TPropByName::iterator it = m_PropsByName.find(sName);
			if(it == m_PropsByName.end())
			{
				m_Props.push_back(std::make_pair(sName, sValue));
				m_PropsByName.insert(std::make_pair(sName, m_Props.size()-1));
			}
			else
			{
				m_Props[it->second].second = sValue;
			}
		}


		bool CXMLNode::PropertyExists(const CommonLib::str_t& sName) const
		{
			return m_PropsByName.find(sName) != m_PropsByName.end();
		}
		const CommonLib::str_t*  CXMLNode::GetProperty(const CommonLib::str_t& sName) const
		{
			TPropByName::const_iterator c_it = m_PropsByName.find(sName);
			if(c_it != m_PropsByName.end())
				return &m_Props[c_it->second].second;

			return NULL;

		}
		uint32	CXMLNode::GetPropertyCnt() const
		{
			return m_Props.size();
		}
		const CommonLib::str_t*   CXMLNode::GetProperty(uint32 nIndex) const
		{
			if(m_Props.size() > nIndex)
				return &m_Props[nIndex].second;
			return NULL;
		}

		int16	CXMLNode::GetPropertyInt16(const CommonLib::str_t& sName, int16 defValue) const
		{
			const CommonLib::str_t* pStr =  GetProperty(sName);
			if(!pStr)
				return defValue;

			wchar_t *pEnd;
			return (int16)wcstol(pStr->cwstr(), &pEnd, 10);
		}
		uint16	CXMLNode::GetPropertyInt16U(const CommonLib::str_t& sName, uint16 defValue) const
		{
			const CommonLib::str_t* pStr =  GetProperty(sName);
			if(!pStr)
				return defValue;

			wchar_t *pEnd;
			return (uint16)wcstol(pStr->cwstr(), &pEnd, 10);
		}
		int32	CXMLNode::GetPropertyInt32(const CommonLib::str_t& sName, int32 defValue) const
		{
			const CommonLib::str_t* pStr =  GetProperty(sName);
			if(!pStr)
				return defValue;

			wchar_t *pEnd;
			return (int32)wcstol(pStr->cwstr(), &pEnd, 10);
		}
		uint32	CXMLNode::GetPropertyInt32U(const CommonLib::str_t& sName, uint32 defValue) const
		{
			const CommonLib::str_t* pStr =  GetProperty(sName);
			if(!pStr)
				return defValue;

			wchar_t *pEnd;
			return (uint32)wcstol(pStr->cwstr(), &pEnd, 10);
		}
		int64	CXMLNode::GetPropertyInt64(const CommonLib::str_t& sName, int64 defValue) const
		{
			const CommonLib::str_t* pStr =  GetProperty(sName);
			if(!pStr)
				return defValue;
			wchar_t *pEnd;
#ifdef _WIN32
		 (int64)_wcstoi64(pStr->cwstr(), &pEnd, 10);
#else
		 (int64)wcstoll(pStr->cwstr(), &pEnd, 10);
#endif
			
			return defValue;
		}
		uint64	CXMLNode::GetPropertyIntU64(const CommonLib::str_t& sName, uint64 defValue) const
		{
			const CommonLib::str_t* pStr =  GetProperty(sName);
			if(!pStr)
				return defValue;

			wchar_t *pEnd;
#ifdef _WIN32
			return (uint64)_wcstoi64(pStr->cwstr(), &pEnd, 10);
#else
			return (uint64)wcstoll(pStr->cwstr(), &pEnd, 10);
#endif

		}
		double  CXMLNode::GetPropertyDouble(const CommonLib::str_t& sName, double defValue) const
		{
			const CommonLib::str_t* pStr =  GetProperty(sName);
			if(!pStr)
				return defValue;


			wchar_t *pEnd;
			return wcstod(pStr->cwstr(), &pEnd);
		}
		bool    CXMLNode::GetPropertyBool(const CommonLib::str_t& sName, bool defValue) const
		{
			const CommonLib::str_t* pStr =  GetProperty(sName);
			if(!pStr)
				return defValue;

			return  *pStr == L"true" ? true : false;
		}
		CommonLib::str_t CXMLNode::GetPropertyString(const CommonLib::str_t& sName, const CommonLib::str_t& defValue) const
		{
			const CommonLib::str_t* pStr =  GetProperty(sName);
			if(!pStr)
				return defValue;

			return *pStr;
		}

		void CXMLNode::save(CommonLib::IWriteStream *pSteam)
		{
			CommonLib::str_t sName;
			 sName.format(L"<%ls", m_sName.wstr());
			if(m_Props.empty() && m_sText.isEmpty() && m_sCAData.isEmpty() && m_Nodes.empty())
			{
			  sName += L"/>\n";
			  pSteam->write(sName.cstr());
			  return;
			}
			pSteam->write(sName.cstr());
			if(!m_Props.empty())
			{				
				CommonLib::str_t sProps;
				for (size_t i = 0, sz = m_Props.size(); i < sz; ++i)
				{
					 pSteam->write(" ");
					 pSteam->write(m_Props[i].first.cstr());
					 pSteam->write("=\"");
					 writeUtf16(pSteam, m_Props[i].second);
					 pSteam->write("\"");
				}

			}
			if(m_sText.isEmpty() && m_sCAData.isEmpty() && m_Nodes.empty())
			{
				pSteam->write("/>\n");
				return;
			}
			bool bClose = false;
			if(!m_Nodes.empty())
			{
				bClose = true;
				pSteam->write(">\n");
				for (size_t i = 0, sz = m_Nodes.size(); i < sz; ++i)
				{
					((CXMLNode*)(m_Nodes[i].get()))->save(pSteam);
				}
			}
			if(!m_sText.isEmpty())
			{
				if(!bClose)
				{
					pSteam->write(">");
					bClose = true;
				}
				writeUtf16(pSteam, m_sText);
			}

			if(!m_sCAData.isEmpty())
			{
				if(!bClose)
				{
					pSteam->write(">");
					bClose = true;
				}

				pSteam->write("<![CDATA[ ");
				pSteam->write(m_sCAData.cstr());
				pSteam->write(" ]]>");
			}

			
			pSteam->write("</");
			pSteam->write(m_sName.cstr());
			pSteam->write(">\n");
		}


		void CXMLNode::writeUtf16(CommonLib::IWriteStream *pSteam, const CommonLib::str_t& str)
		{
			std::vector<char> vecUtf8;
			utf16_to_utf8(str, vecUtf8);
			if(!vecUtf8.empty())
			{
				pSteam->write((byte*)&vecUtf8[0], vecUtf8.back() == 0 ? vecUtf8.size() - 1 : vecUtf8.size());
			}
		}

		void CXMLNode::clear()
		{
			m_sName.clear();
			m_sText.clear();
			m_sCAData.clear();
			m_Nodes.clear();
			m_NodeByName.clear();
			m_Props.clear();
			m_PropsByName.clear();
		}
	}

}