#ifndef _EMBEDDED_DATABASE_I_ROW_H_
#define _EMBEDDED_DATABASE_I_ROW_H_

#include "IField.h"
#include "CommonLibrary/Variant.h"
namespace embDB
{

	class IRow : public CommonLib::RefCounter
	{
	public:
		IRow(){}
		virtual ~IRow(){};
		virtual int32 count() const = 0;

		virtual CommonLib::CVariant* value(int32 nNum) = 0;
		virtual bool set(CommonLib::CVariant* pValue, int32 nNum) = 0;
	};

	class INameIRow : public IRow
	{
	public:
		INameIRow(){}
		virtual ~INameIRow(){};

		virtual CommonLib::CVariant* value(const wchar_t* pszName) = 0;
		virtual bool set(const CommonLib::CString& sName,  CommonLib::CVariant* pValue) = 0;
	};

	typedef CommonLib::IRefCntPtr<IRow> IRowPtr;

}
#endif