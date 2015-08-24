#ifndef _LIB_GIS_ENGINE_COMMON_XML_NODE_H_
#define _LIB_GIS_ENGINE_COMMON_XML_NODE_H_

#include "GisEngineCommon.h"

class TiXmlNode;
class TiXmlElement;
class TiXmlAttribute;
namespace GisEngine
{
	namespace GisCommon
	{
		class CXMLNode : public IXMLNode
		{
		public:
			CXMLNode(TiXmlNode *pNode);
			virtual ~CXMLNode();

			virtual void                   AddChildNode(IXMLNode* child);
			virtual IXMLNodePtr			   CreateChildNode(const wchar_t *pszName = L"");
			virtual uint32				   GetChildCnt() const;
			virtual IXMLNodePtr			   GetChild(uint32 nIndex) const;
			virtual IXMLNodePtr			   GetChild(const wchar_t *pszName);



			virtual const CommonLib::str_t&  GetName() const;
			virtual void                   SetName( const CommonLib::str_t& name);

			virtual const CommonLib::str_t&  GetText() const;
			virtual void                   SetText(const CommonLib::str_t&);

			virtual const CommonLib::str_t&   GetCDATA() const;
			virtual void                   SetCDATA(const   CommonLib::str_t& cdata);

			virtual CommonLib::CBlob&      GetBlobCDATA() const;
			virtual void                   SetBlobCDATA(const CommonLib::CBlob &data);


			virtual void                 AddProperty(const CommonLib::str_t& sName, const CommonLib::CVariant& val);
			virtual void				 AddPropertyInt16(const wchar_t *name, int16 value);
			virtual void				 AddPropertyInt16U(const wchar_t *name, uint16 value);
			virtual void				 AddPropertyInt32(const wchar_t *name, int32 value);
			virtual void				 AddPropertyInt32U(const wchar_t *name, uint32 value);
			virtual void				 AddPropertyInt64(const wchar_t *name, int64 value);
			virtual void				 AddPropertyIntU64(const wchar_t *name, uint64 value);
			virtual void				 AddPropertyDouble(const wchar_t *name, double value);
			virtual void                 AddPropertyBool(const wchar_t *name, bool value);
			virtual void				 AddPropertyString(const wchar_t *name, const CommonLib::str_t& value);


			virtual bool				   PropertyExists(const wchar_t *name) const;
			virtual CommonLib::CVariant*   GetProperty(const wchar_t *name);
			virtual uint32				   GetPropertyCnt() const;
			virtual CommonLib::CVariant*   GetProperty(uint32 nIndex);

			virtual int16				 GetPropertyInt16(const wchar_t *name, int16 defValue) const;
			virtual uint16				 GetPropertyInt16U(const wchar_t *name, uint16 defValue) const;
			virtual int32				 GetPropertyInt32(const wchar_t *name, int32 defValue) const;
			virtual uint32				 GetPropertyInt32U(const wchar_t *name, uint32 defValue) const;
			virtual int64				 GetPropertyInt64(const wchar_t *name, int64 defValue) const;
			virtual uint64				 GetPropertyIntU64(const wchar_t *name, uint64 defValue) const;
			virtual double               GetPropertyDouble(const wchar_t *name, double defValue) const;
			virtual bool                 GetPropertyBool(const wchar_t *name, bool defValue) const;
			virtual CommonLib::str_t	 GetPropertyString(const wchar_t *name, const CommonLib::str_t& defValue) const;

		private:
			 TiXmlNode* m_pTiNode;
			 TiXmlElement* m_pXmlElement;
			 TiXmlAttribute* m_pCurrentAttribute;
		};
	}
}
#endif