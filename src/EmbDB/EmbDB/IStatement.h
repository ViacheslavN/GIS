#ifndef _EMBEDDED_DATABASE_I_STATEMENT_H_
#define _EMBEDDED_DATABASE_I_STATEMENT_H_

#include "IField.h"
namespace embDB
{

	class IStatement
	{
	public:

		IStatement(){}
		virtual ~IStatement(){}
		virtual bool setValue(const wchar_t *pszValueName, IFieldVariant* pVal) = 0;
		virtual IFieldVariant* getValue(const wchar_t *pszValueName)= 0;
		virtual IField* getField(uint32 nCount) = 0;
		virtual uint32 getFieldCount() const = 0;
	};
}
#endif