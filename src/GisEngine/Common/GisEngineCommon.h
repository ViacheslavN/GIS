#ifndef GIS_ENGINE_COMMON_INTERFACE_H_
#define GIS_ENGINE_COMMON_INTERFACE_H_

#include "CommonLibrary/Variant.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/MemoryStream.h"
#include "CommonLibrary/IRefCnt.h"
#include "CommonLibrary/blob.h"
#include "Common.h"
 
namespace GisEngine
{
	namespace GisCommon
	{

		struct IPropertySet;
		struct IXMLWriter;
		struct IXMLReader;
		struct IXMLNode;

		COMMON_LIB_REFPTR_TYPEDEF(IPropertySet);
		COMMON_LIB_REFPTR_TYPEDEF(IXMLNode);

		struct IStreamSerialize
		{
			IStreamSerialize(){}
			virtual ~IStreamSerialize(){}
			virtual bool save(CommonLib::IWriteStream *pWriteStream) const = 0;
			virtual bool load(CommonLib::IReadStream* pReadStream) = 0;
		};

		struct IXMLSerialize
		{
			IXMLSerialize(){}
			virtual ~IXMLSerialize(){}
			virtual bool saveXML(IXMLNode* pXmlNode) const = 0;
			virtual bool load(IXMLNode* pXmlNode) = 0;
		};


		struct IXMLNode : CommonLib::AutoRefCounter
		{
			IXMLNode(){}
			virtual ~IXMLNode(){}

			virtual void                   AddChildNode(IXMLNode* child) = 0;
			virtual IXMLNodePtr			   CreateChildNode(const CommonLib::str_t& sName) = 0;
			virtual uint32				   GetChildCnt() const = 0;
			virtual IXMLNodePtr			   GetChild(uint32 nIndex) const = 0;
			virtual IXMLNodePtr			   GetChild(const wchar_t *pszName) = 0;

			virtual IXMLNodePtr			  GetParent() const= 0;



			virtual const CommonLib::str_t&  GetName() const = 0;
			//virtual void                   SetName( const CommonLib::str_t& name) = 0;

			virtual const CommonLib::str_t&  GetText() const = 0;
			virtual void                   SetText(const CommonLib::str_t&) = 0;

			virtual const CommonLib::str_t&   GetCDATA() const = 0;
			virtual void                   SetCDATA(const   CommonLib::str_t& cdata) = 0;

			virtual	void				   GetBlobCDATA(CommonLib::CBlob& data) const = 0;
			virtual void                   SetBlobCDATA(const CommonLib::CBlob &data) = 0;
			
	
			virtual void                 AddProperty(const CommonLib::str_t& sName, const CommonLib::CVariant& val) = 0;
			virtual void				 AddPropertyInt16(const CommonLib::str_t& sName, int16 value ) = 0;
			virtual void				 AddPropertyInt16U(const CommonLib::str_t& sName, uint16 value)  = 0;
			virtual void				 AddPropertyInt32(const CommonLib::str_t& sName, int32 value)  = 0;
			virtual void				 AddPropertyInt32U(const CommonLib::str_t& sName, uint32 value)  = 0;
			virtual void				 AddPropertyInt64(const CommonLib::str_t& sName, int64 value)  = 0;
			virtual void				 AddPropertyIntU64(const CommonLib::str_t& sName, uint64 value)  = 0;
			virtual void				 AddPropertyDouble(const CommonLib::str_t& sName, double value)  = 0;
			virtual void                 AddPropertyBool(const CommonLib::str_t& sName, bool value)  = 0;
			virtual void				 AddPropertyString(const CommonLib::str_t& sName, const CommonLib::str_t& value)  = 0;

		
			virtual bool				   PropertyExists(const CommonLib::str_t& sName) const = 0;
			virtual const CommonLib::str_t*   GetProperty(const CommonLib::str_t& sName) const= 0;
			virtual uint32				   GetPropertyCnt() const = 0;
			virtual const CommonLib::str_t*   GetProperty(uint32 nIndex) const= 0;
						
			virtual int16				 GetPropertyInt16(const CommonLib::str_t& sName, int16 defValue) const = 0;
			virtual uint16				 GetPropertyInt16U(const CommonLib::str_t& sName, uint16 defValue) const = 0;
			virtual int32				 GetPropertyInt32(const CommonLib::str_t& sName, int32 defValue) const = 0;
			virtual uint32				 GetPropertyInt32U(const CommonLib::str_t& sName, uint32 defValue) const = 0;
			virtual int64				 GetPropertyInt64(const CommonLib::str_t& sName, int64 defValue) const = 0;
			virtual uint64				 GetPropertyIntU64(const CommonLib::str_t& sName, uint64 defValue) const = 0;
			virtual double               GetPropertyDouble(const CommonLib::str_t& sName, double defValue) const = 0;
			virtual bool                 GetPropertyBool(const CommonLib::str_t& sName, bool defValue) const = 0;
			virtual CommonLib::str_t	 GetPropertyString(const CommonLib::str_t& sName, const CommonLib::str_t& defValue) const = 0;

		};

		struct IXMLDoc
		{
			IXMLDoc(){}
			virtual ~IXMLDoc(){}

			virtual bool  Open(const CommonLib::str_t& xml) = 0;
			virtual bool  Open(CommonLib::IReadStream* pStream) = 0;

			virtual bool  Save(const CommonLib::str_t& xml) = 0;
			virtual bool  Save(CommonLib::IWriteStream* pStream) = 0;

			virtual IXMLNodePtr			   GetNodes() const = 0;
			virtual void	Clear() = 0;
			virtual const CommonLib::str_t& GetError() const = 0;


		};

		struct IPropertySet : public CommonLib::AutoRefCounter
		{
			IPropertySet(){}
			virtual ~IPropertySet(){}
			virtual int  count() const = 0;
			virtual bool  PropertyExists(const wchar_t *name) const = 0;
			virtual const CommonLib::CVariant* GetProperty(const wchar_t *name) const = 0;
			virtual void  SetProperty(const wchar_t *name, const CommonLib::CVariant& value) = 0;
			virtual void  RemoveProperty(const wchar_t *name) = 0;
			virtual void  RemoveAllProperties() = 0;

		};

		struct ITrackCancel
		{
			ITrackCancel(){}
			virtual ~ITrackCancel(){}
			virtual void Cancel() = 0;
			virtual bool Continue() = 0; 
		};

		struct IXMLWriter
		{

			IXMLWriter(){}
			virtual ~IXMLWriter(){}
		};
	}
}

#endif