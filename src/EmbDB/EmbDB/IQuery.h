#ifndef _EMBEDDED_DATABASE_I_QUERY_H_
#define _EMBEDDED_DATABASE_I_QUERY_H_

#include "CommonLibrary/str_t.h"
#include "IField.h"
namespace embDB
{

	class IQuery
	{
	public:
		IQuery(){}
		virtual ~IQuery(){}
		virtual bool setValue(const CommonLib::str_t& sValueName, IVariant* pVal) = 0;
		virtual IVariant* getValue(const CommonLib::str_t& sValueName)= 0;
		virtual IField* getField(uint32 nCount) = 0;
		virtual uint32 getFieldCount() const = 0;

	};
}
#endif