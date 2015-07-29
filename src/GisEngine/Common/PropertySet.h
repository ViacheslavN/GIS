#ifndef _LIB_GIS_ENGINE_COMMON_PROPERTY_SET_H_
#define _LIB_GIS_ENGINE_COMMON_PROPERTY_SET_H_

#include "GisEngineCommon.h"

namespace GisEngine
{
	namespace Common
	{
		class CPropertySet : public IPropertySet
		{
		public:
			CPropertySet();
			~CPropertySet();
			virtual int  count() const;
			virtual bool  PropertyExists(const wchar_t *name) const;
			virtual CommonLib::IVariant* GetProperty(const wchar_t *name) const;
			virtual void  SetProperty(const wchar_t *name, CommonLib::IVariant* value);
			virtual void  RemoveProperty(const wchar_t *name);
			virtual void  RemoveAllProperties();
		private:
			void clear();
		private:
			typedef std::map<CommonLib::str_t, CommonLib::IVariant*> TMapProp;
			TMapProp m_mapProp;

		};
	}
}
#endif