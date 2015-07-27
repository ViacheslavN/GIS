#ifndef _EMBEDDED_DATABASE_I_RECORDSET_H_
#define _EMBEDDED_DATABASE_I_RECORDSET_H_

#include "IField.h"
namespace embDB
{

	class IRecordset
	{
	public:
		IRecordset(){}
		virtual ~IRecordset(){};
		virtual int32 count() const = 0;

		virtual CommonLib::IVariant* value(int32 nNum) = 0;
		virtual bool set(CommonLib::IVariant*, int32 nNum) = 0;
	};

	class INameRecordset : public IRecordset
	{
	public:
		INameRecordset(){}
		virtual ~INameRecordset(){};

		virtual CommonLib::IVariant* value(const wchar_t* pszName) = 0;
		virtual bool set(const CommonLib::str_t& sName, CommonLib::IVariant*) = 0;
	};

}
#endif