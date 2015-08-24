#include "stdafx.h"
#include "XMLNode.h"
#include "TinyXML/tinyxml.h"
namespace GisEngine
{
	namespace GisCommon
	{
		CXMLNode::CXMLNode(TiXmlNode *pNode) : m_pTiNode(pNode), m_pXmlElement(0), m_pCurrentAttribute(0)
		{
			if(m_pTiNode)
			{
				m_pXmlElement = m_pTiNode->ToElement();

			}
		}
		CXMLNode::~CXMLNode()
		{
			 
		}

		void   CXMLNode::AddChildNode(IXMLNode* child)
		{
		 
		}
		IXMLNodePtr	CXMLNode::CreateChildNode(const wchar_t *pszName)
		{
			 
		}
		uint32		CXMLNode::GetChildCnt() const
		{

		}
		IXMLNodePtr	CXMLNode::GetChild(uint32 nIndex) const
		{

		}
		IXMLNodePtr	CXMLNode::GetChild(const wchar_t *pszName)
		{

		}



		const CommonLib::str_t&  CXMLNode::GetName() const
		{

		}
		void   CXMLNode::SetName( const CommonLib::str_t& name)
		{

		}

		const CommonLib::str_t&  CXMLNode::GetText() const
		{

		}
		void   CXMLNode::SetText(const CommonLib::str_t&)
		{

		}

		const CommonLib::str_t&   CXMLNode::GetCDATA() const
		{

		}
		void   CXMLNode::SetCDATA(const   CommonLib::str_t& cdata)
		{

		}

		CommonLib::CBlob&  CXMLNode::GetBlobCDATA() const
		{

		}
		void   CXMLNode::SetBlobCDATA(const CommonLib::CBlob &data)
		{

		}


		void	CXMLNode::AddProperty(const CommonLib::str_t& sName, const CommonLib::CVariant& val)
		{

		}
		void	CXMLNode::AddPropertyInt16(const wchar_t *name, int16 value)
		{

		}
		void	CXMLNode::AddPropertyInt16U(const wchar_t *name, uint16 value)
		{

		}
		void	CXMLNode::AddPropertyInt32(const wchar_t *name, int32 value)
		{

		}
		void	CXMLNode::AddPropertyInt32U(const wchar_t *name, uint32 value)
		{

		}
		void	CXMLNode::AddPropertyInt64(const wchar_t *name, int64 value)
		{

		}
		void	CXMLNode::AddPropertyIntU64(const wchar_t *name, uint64 value)
		{

		}
		void	CXMLNode::AddPropertyDouble(const wchar_t *name, double value)
		{

		}
		void    CXMLNode::AddPropertyBool(const wchar_t *name, bool value)
		{

		}
		void	CXMLNode::AddPropertyString(const wchar_t *name, const CommonLib::str_t& value)
		{

		}


		bool CXMLNode::PropertyExists(const wchar_t *name) const
		{

		}
		CommonLib::CVariant*  CXMLNode::GetProperty(const wchar_t *name)
		{

		}
		uint32	CXMLNode::GetPropertyCnt() const
		{

		}
		CommonLib::CVariant*   CXMLNode::GetProperty(uint32 nIndex)
		{

		}

		int16	CXMLNode::GetPropertyInt16(const wchar_t *name, int16 defValue) const
		{

		}
		uint16	CXMLNode::GetPropertyInt16U(const wchar_t *name, uint16 defValue) const
		{

		}
		int32	CXMLNode::GetPropertyInt32(const wchar_t *name, int32 defValue) const
		{

		}
		uint32	CXMLNode::GetPropertyInt32U(const wchar_t *name, uint32 defValue) const
		{

		}
		int64	CXMLNode::GetPropertyInt64(const wchar_t *name, int64 defValue) const
		{

		}
		uint64	CXMLNode::GetPropertyIntU64(const wchar_t *name, uint64 defValue) const
		{

		}
		double  CXMLNode::GetPropertyDouble(const wchar_t *name, double defValue) const
		{

		}
		bool    CXMLNode::GetPropertyBool(const wchar_t *name, bool defValue) const
		{

		}
		CommonLib::str_t CXMLNode::GetPropertyString(const wchar_t *name, const CommonLib::str_t& defValue) const
		{

		}
	}
}