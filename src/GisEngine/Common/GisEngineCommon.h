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
			virtual bool save(IXMLNode* pXmlNode) const = 0;
			virtual bool load(IXMLNode* pXmlNode) = 0;
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
			
	
			virtual void                 AddProperty(const CommonLib::str_t& sName, const CommonLib::CVariant& val) = 0;
			virtual void				 AddPropertyInt16(const wchar_t *name, int16 value) const;
			virtual void				 AddPropertyInt16U(const wchar_t *name, uint16 value) const;
			virtual void				 AddPropertyInt32(const wchar_t *name, int32 value) const;
			virtual void				 AddPropertyInt32U(const wchar_t *name, uint32 value) const;
			virtual void				 AddPropertyInt64(const wchar_t *name, int64 value) const;
			virtual void				 AddPropertyIntU64(const wchar_t *name, uint64 value) const;
			virtual void				 AddPropertyDouble(const wchar_t *name, double value) const;
			virtual void                 AddPropertyBool(const wchar_t *name, bool value) const;
			virtual void				 AddPropertyString(const wchar_t *name, const CommonLib::str_t& value) const;

		
			virtual bool				   PropertyExists(const wchar_t *name) const = 0;
			virtual CommonLib::CVariant*   GetProperty(const wchar_t *name) = 0;
			virtual uint32				   GetPropertyCnt() const = 0;
			virtual CommonLib::CVariant*   GetProperty(uint32 nIndex) = 0;
						
			virtual int16				 GetPropertyInt16(const wchar_t *name, int16 defValue) const;
			virtual uint16				 GetPropertyInt16U(const wchar_t *name, uint16 defValue) const;
			virtual int32				 GetPropertyInt32(const wchar_t *name, int32 defValue) const;
			virtual uint32				 GetPropertyInt32U(const wchar_t *name, uint32 defValue) const;
			virtual int64				 GetPropertyInt64(const wchar_t *name, int64 defValue) const;
			virtual uint64				 GetPropertyIntU64(const wchar_t *name, uint64 defValue) const;
			virtual double               GetPropertyDouble(const wchar_t *name, double defValue) const;
			virtual bool                 GetPropertyBool(const wchar_t *name, bool defValue) const;
			virtual CommonLib::str_t	 GetPropertyString(const wchar_t *name, const CommonLib::str_t& defValue) const;

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