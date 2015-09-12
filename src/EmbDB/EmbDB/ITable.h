#ifndef _EMBEDDED_DATABASE_I_TABLE_H_
#define _EMBEDDED_DATABASE_I_TABLE_H_

#include "IField.h"

namespace embDB
{
	struct SFieldProp
	{
		CommonLib::CString sFieldName;
		CommonLib::CString sFieldAlias;
		eDataTypes dataType;
		eDataTypesExt dateTypeExt;
	};

	struct SIndexProp
	{
		indexTypes indexType;
	};
	class ITable
	{
	public:
		ITable(){}
		virtual ~ITable(){}
		virtual bool getOIDFieldName(CommonLib::CString& sOIDName) = 0;
		virtual bool setOIDFieldName(const CommonLib::CString& sOIDName) = 0;
		virtual const CommonLib::CString& getName() const  = 0;
		virtual IField* getField(const CommonLib::CString& sName) const = 0;
		virtual size_t getFieldCnt() const = 0;
		virtual IField* getField(size_t nIdx) const = 0;
		



	};
}
#endif