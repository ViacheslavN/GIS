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
			virtual void save(CommonLib::IWriteStream *pWriteStream) const = 0;
			virtual void load(CommonLib::IReadStream* pReadStream) = 0;
		};

		struct IXMLSerialize
		{
			IXMLSerialize(){}
			virtual ~IXMLSerialize(){}
			virtual void save(IXMLNode* pXmlNode) const = 0;
			virtual void load(IXMLNode* pXmlNode) = 0;
		};


		struct IXMLNode : CommonLib::AutoRefCounter
		{
			IXMLNode(){}
			virtual ~IXMLNode(){}

			virtual void                   AddChildNode(IXMLNode* child) = 0;
			virtual IXMLNodePtr			   CreateChildNode() = 0;
			virtual uint32				   GetChildCnt() const = 0;
			virtual IXMLNodePtr			   GetChild(uint32 nIndex) const = 0;



			virtual const CommonLib::str_t&  GetName() const = 0;
			virtual void                   SetName( const CommonLib::str_t& name) = 0;
			virtual const CommonLib::str_t&  GetText() const = 0;
			virtual void                   SetText(const CommonLib::str_t&) = 0;
			virtual const CommonLib::str_t&   GetCDATA() const = 0;
			virtual void                   SetCDATA(const   CommonLib::str_t& cdata) = 0;
			virtual CommonLib::CBlob&      GetBlobCDATA() const = 0;

			virtual void                   SetBlobCDATA(const CommonLib::CBlob &data) = 0;
			virtual void                   AddProperty(const CommonLib::str_t& sName, const CommonLib::CVariant& val) = 0;
			virtual CommonLib::CVariant*   GetProperty(const CommonLib::str_t& sName) = 0;
			virtual uint32				   GetPropertyCnt() const = 0;
			virtual CommonLib::CVariant*   GetProperty(uint32 nIndex) = 0;
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