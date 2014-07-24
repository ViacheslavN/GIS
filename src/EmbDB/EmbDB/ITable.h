#ifndef _EMBEDDED_DATABASE_I_TABLE_H_
#define _EMBEDDED_DATABASE_I_TABLE_H_

#include "IField.h"

namespace embDB
{
	struct SFieldProp
	{
		CommonLib::str_t sFieldName;
		CommonLib::str_t sFieldAlias;
		eDataTypes dataType;
		indexTypes indexType;
		eDataTypesExt dateTypeExt;
	};
	class ITable
	{
	public:
		ITable(){}
		virtual ~ITable(){}
		virtual const CommonLib::str_t& getName() const  = 0;
		virtual IField* getField(const CommonLib::str_t& sName) const = 0;
		virtual size_t getFieldCnt() const = 0;
		virtual IField* getField(size_t nIdx) const = 0;
		virtual IField* createField(SFieldProp& sFP) = 0;
		virtual bool deleteField(IField* pField) = 0;



	};
}
#endif