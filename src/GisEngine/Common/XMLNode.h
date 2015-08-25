#ifndef _LIB_GIS_ENGINE_COMMON_XML_NODE_H_
#define _LIB_GIS_ENGINE_COMMON_XML_NODE_H_

#include "GisEngineCommon.h"


namespace GisEngine
{
	namespace GisCommon
	{
		class CXMLNode : public IXMLNode
		{
		public:
			CXMLNode(IXMLNode *pParent = NULL, const CommonLib::str_t& sName = L"");
			virtual ~CXMLNode();


		 


			virtual void                   AddChildNode(IXMLNode* child);
			virtual IXMLNodePtr			   CreateChildNode(const CommonLib::str_t& sName);
			virtual uint32				   GetChildCnt() const;
			virtual IXMLNodePtr			   GetChild(uint32 nIndex) const;
			virtual IXMLNodePtr			   GetChild(const wchar_t *pszName);


			virtual IXMLNodePtr			  GetParent() const;

			virtual const CommonLib::str_t&  GetName() const;
			virtual void                   SetName( const CommonLib::str_t& name);

			virtual const CommonLib::str_t&  GetText() const;
			virtual void                   SetText(const CommonLib::str_t&);

			virtual const CommonLib::str_t&   GetCDATA() const;
			virtual void                   SetCDATA(const   CommonLib::str_t& cdata);

			virtual const CommonLib::CBlob&      GetBlobCDATA() const;
			virtual void                   SetBlobCDATA(const CommonLib::CBlob &data);


			virtual void                 AddProperty(const CommonLib::str_t& sName, const CommonLib::CVariant& val);
			virtual void				 AddPropertyInt16(const CommonLib::str_t &name, int16 value);
			virtual void				 AddPropertyInt16U(const CommonLib::str_t &name, uint16 value);
			virtual void				 AddPropertyInt32(const CommonLib::str_t &name, int32 value);
			virtual void				 AddPropertyInt32U(const CommonLib::str_t &name, uint32 value);
			virtual void				 AddPropertyInt64(const CommonLib::str_t &name, int64 value);
			virtual void				 AddPropertyIntU64(const CommonLib::str_t &name, uint64 value);
			virtual void				 AddPropertyDouble(const CommonLib::str_t &name, double value);
			virtual void                 AddPropertyBool(const CommonLib::str_t &name, bool value);
			virtual void				 AddPropertyString(const CommonLib::str_t &name, const CommonLib::str_t& value);


			virtual bool				PropertyExists(const CommonLib::str_t &name) const;
			virtual const CommonLib::str_t*   GetProperty(const CommonLib::str_t &name) const;
			virtual uint32				GetPropertyCnt() const;
			virtual const CommonLib::str_t*   GetProperty(uint32 nIndex) const;

			virtual int16				 GetPropertyInt16(const CommonLib::str_t &name, int16 defValue) const;
			virtual uint16				 GetPropertyInt16U(const CommonLib::str_t &name, uint16 defValue) const;
			virtual int32				 GetPropertyInt32(const CommonLib::str_t &name, int32 defValue) const;
			virtual uint32				 GetPropertyInt32U(const CommonLib::str_t &name, uint32 defValue) const;
			virtual int64				 GetPropertyInt64(const CommonLib::str_t &name, int64 defValue) const;
			virtual uint64				 GetPropertyIntU64(const CommonLib::str_t &name, uint64 defValue) const;
			virtual double               GetPropertyDouble(const CommonLib::str_t &name, double defValue) const;
			virtual bool                 GetPropertyBool(const CommonLib::str_t &name, bool defValue) const;
			virtual CommonLib::str_t	 GetPropertyString(const CommonLib::str_t &name, const CommonLib::str_t& defValue) const;

			void save(CommonLib::IWriteStream *pSteam);

		private:

			  CommonLib::str_t m_sName;
			  CommonLib::str_t m_sText;
			  CommonLib::str_t m_sCAData;
			  CommonLib::CBlob m_blob;

			  IXMLNodePtr m_pParent;
			  typedef std::vector<IXMLNodePtr> TNodes;
			  typedef std::multimap<CommonLib::str_t, uint32> TNodesByName;

			  typedef std::vector<std::pair<CommonLib::str_t, CommonLib::str_t> > TVecProp;
			  typedef std::map<CommonLib::str_t, uint32> TPropByName;

			  TNodes m_Nodes;
			  TNodesByName m_NodeByName;

			  TVecProp m_Props;
			  TPropByName m_PropsByName;
		};
	}
}
#endif