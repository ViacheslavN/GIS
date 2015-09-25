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
			CXMLNode(IXMLNode *pParent = NULL, const CommonLib::CString& sName = L"");
			virtual ~CXMLNode();


		 


			virtual void                   AddChildNode(IXMLNode* child);
			virtual IXMLNodePtr			   CreateChildNode(const CommonLib::CString& sName);
			virtual uint32				   GetChildCnt() const;
			virtual IXMLNodePtr			   GetChild(uint32 nIndex) const;
			virtual IXMLNodePtr			   GetChild(const wchar_t *pszName) const;


			virtual IXMLNodePtr			  GetParent() const;

			virtual const CommonLib::CString&  GetName() const;
			virtual void                   SetName( const CommonLib::CString& name);

			virtual const CommonLib::CString&  GetText() const;
			virtual void                   SetText(const CommonLib::CString&);

			virtual const CommonLib::CString&   GetCDATA() const;
			virtual void                   SetCDATA(const   CommonLib::CString& cdata);

			virtual void			       GetBlobCDATA(CommonLib::CBlob& data) const;
			virtual void                   SetBlobCDATA(const CommonLib::CBlob &data);


			virtual void                 AddProperty(const CommonLib::CString& sName, const CommonLib::CVariant& val);
			virtual void				 AddPropertyInt16(const CommonLib::CString &name, int16 value);
			virtual void				 AddPropertyInt16U(const CommonLib::CString &name, uint16 value);
			virtual void				 AddPropertyInt32(const CommonLib::CString &name, int32 value);
			virtual void				 AddPropertyInt32U(const CommonLib::CString &name, uint32 value);
			virtual void				 AddPropertyInt64(const CommonLib::CString &name, int64 value);
			virtual void				 AddPropertyIntU64(const CommonLib::CString &name, uint64 value);
			virtual void				 AddPropertyDouble(const CommonLib::CString &name, double value);
			virtual void                 AddPropertyBool(const CommonLib::CString &name, bool value);
			virtual void				 AddPropertyString(const CommonLib::CString &name, const CommonLib::CString& value);


			virtual bool				PropertyExists(const CommonLib::CString &name) const;
			virtual const CommonLib::CString*   GetProperty(const CommonLib::CString &name) const;
			virtual uint32				GetPropertyCnt() const;
			virtual const CommonLib::CString*   GetProperty(uint32 nIndex) const;

			virtual int16				 GetPropertyInt16(const CommonLib::CString &name, int16 defValue) const;
			virtual uint16				 GetPropertyInt16U(const CommonLib::CString &name, uint16 defValue) const;
			virtual int32				 GetPropertyInt32(const CommonLib::CString &name, int32 defValue) const;
			virtual uint32				 GetPropertyInt32U(const CommonLib::CString &name, uint32 defValue) const;
			virtual int64				 GetPropertyInt64(const CommonLib::CString &name, int64 defValue) const;
			virtual uint64				 GetPropertyIntU64(const CommonLib::CString &name, uint64 defValue) const;
			virtual double               GetPropertyDouble(const CommonLib::CString &name, double defValue) const;
			virtual bool                 GetPropertyBool(const CommonLib::CString &name, bool defValue) const;
			virtual CommonLib::CString	 GetPropertyString(const CommonLib::CString &name, const CommonLib::CString& defValue) const;

			void save(CommonLib::IWriteStream *pSteam);
			void clear();
		private:
			void writeUtf16(CommonLib::IWriteStream *pSteam, const CommonLib::CString& str);
		private:

			  CommonLib::CString m_sName;
			  CommonLib::CString m_sText;
			  CommonLib::CString m_sCAData;

			  IXMLNodePtr m_pParent;
			  typedef std::vector<IXMLNodePtr> TNodes;
			  typedef std::multimap<CommonLib::CString, uint32> TNodesByName;

			  typedef std::vector<std::pair<CommonLib::CString, CommonLib::CString> > TVecProp;
			  typedef std::map<CommonLib::CString, uint32> TPropByName;

			  TNodes m_Nodes;
			  TNodesByName m_NodeByName;

			  TVecProp m_Props;
			  TPropByName m_PropsByName;
		};
	}
}
#endif