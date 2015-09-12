#ifndef _EMBEDDED_DATABASE_I_QUERY_H_
#define _EMBEDDED_DATABASE_I_QUERY_H_

#include "CommonLibrary/String.h"
#include "IField.h"
namespace embDB
{

	class IQuery
	{
	public:
		IQuery(){}
		virtual ~IQuery(){}
		virtual bool setValue(const CommonLib::CString& sValueName, IVariant* pVal) = 0;
		virtual IVariant* getValue(const CommonLib::CString& sValueName)= 0;
		virtual IField* getField(uint32 nCount) = 0;
		virtual uint32 getFieldCount() const = 0;

	};
}
#endif