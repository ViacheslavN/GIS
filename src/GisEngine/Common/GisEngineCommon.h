#ifndef GIS_ENGINE_COMMON_INTERFACE_H_
#define GIS_ENGINE_COMMON_INTERFACE_H_

#include "CommonLibrary/Variant.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/MemoryStream.h"
#include "CommonLibrary/IRefCnt.h"
#include "Common.h"
 
namespace GisEngine
{
	namespace Common
	{

		struct IPropertySet;
		struct IXMLWriter;
		struct IXMLReader;

		COMMON_LIB_REFPTR_TYPEDEF(IPropertySet);

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
			virtual void save(IXMLWriter* pWriteXML) const = 0;
			virtual void load(IXMLReader* pReadXML) = 0;
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