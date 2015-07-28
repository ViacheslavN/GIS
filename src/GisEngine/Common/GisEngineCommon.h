#ifndef GIS_ENGINE_COMMON_INTERFACE_H_
#define GIS_ENGINE_COMMON_INTERFACE_H_

#include "CommonLibrary/Variant.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/MemoryStream.h"
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
			virtual void Save(CommonLib::IWriteStream *pWriteStream) const = 0;
			virtual void Load(CommonLib::IReadStream* pReadStream) = 0;
		};

		class IPropertySet
		{
		public:
			IPropertySet(){}
			virtual ~IPropertySet(){}
			virtual int  Count() const = 0;
			virtual bool  PropertyExists(const wchar_t *name) const = 0;
			virtual CommonLib::IVariant* GetProperty(const wchar_t *name) const = 0;
			virtual void  SetProperty(const wchar_t *name, CommonLib::IVariant* value) = 0;
			virtual void  RemoveProperty(const wchar_t *name) = 0;
			virtual void  RemoveAllProperties() = 0;

		};
	}
}