#ifndef _EMBEDDED_DATABASE_I_STATEMENT_H_
#define _EMBEDDED_DATABASE_I_STATEMENT_H_

#include "IField.h"
namespace embDB
{

	class IStatement : public CommonLib::AutoRefCounter
	{
	public:

		IStatement(){}
		virtual ~IStatement(){}
		virtual bool setValue(const wchar_t *pszValueName, CommonLib::IVariant* pVal) = 0;
		virtual CommonLib::IVariant* getValue(const wchar_t *pszValueName)= 0;
		virtual IField* getField(uint32 nCount) = 0;
		virtual uint32 getFieldCount() const = 0;
	};
	typedef CommonLib::IRefCntPtr<IStatement> IStatementPtr;
}
#endif