#ifndef _LIB_GIS_ENGINE_COMMON_PROPERTY_SET_H_
#define _LIB_GIS_ENGINE_COMMON_PROPERTY_SET_H_

#include "GisEngineCommon.h"

namespace GisEngine
{
	namespace GisCommon
	{
		class CPropertySet : public IPropertySet
		{
		public:
			CPropertySet();
			~CPropertySet();
			virtual int  count() const;
			virtual bool  PropertyExists(const wchar_t *name) const;
			virtual const CommonLib::CVariant* GetProperty(const wchar_t *name) const;
			virtual void  SetProperty(const wchar_t *name, const CommonLib::CVariant& value);
			virtual void  RemoveProperty(const wchar_t *name);
			virtual void  RemoveAllProperties();
		private:
			void clear();
		private:
			typedef std::map<CommonLib::str_t, CommonLib::CVariant> TMapProp;
			TMapProp m_mapProp;

		};
	}
}
#endif