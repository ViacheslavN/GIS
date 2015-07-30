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

		class IStreamSerialize
		{
		public:
			IStreamSerialize(void){}
			virtual ~IStreamSerialize(void){}
		public:
			virtual void save(CommonLib::IWriteStream *pWriteStream) const = 0;
			virtual void load(CommonLib::IReadStream* pReadStream) = 0;
		};

		class IPropertySet : public CommonLib::AutoRefCounter
		{
		public:
			IPropertySet(){}
			virtual ~IPropertySet(){}
			virtual int  count() const = 0;
			virtual bool  PropertyExists(const wchar_t *name) const = 0;
			virtual CommonLib::IVariant* GetProperty(const wchar_t *name) const = 0;
			virtual void  SetProperty(const wchar_t *name, CommonLib::IVariant* value) = 0;
			virtual void  RemoveProperty(const wchar_t *name) = 0;
			virtual void  RemoveAllProperties() = 0;

		};

		typedef CommonLib::IRefCntPtr<IPropertySet> IPropertySetPtr;
	}
}

#endif