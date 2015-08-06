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


		COMMON_LIB_REFPTR_TYPEDEF(IPropertySet);

		struct IStreamSerialize
		{
			IStreamSerialize(void){}
			virtual ~IStreamSerialize(void){}
			virtual void save(CommonLib::IWriteStream *pWriteStream) const = 0;
			virtual void load(CommonLib::IReadStream* pReadStream) = 0;
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
	}
}

#endif