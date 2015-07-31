#ifndef _EMBEDDED_DATABASE_I_CURSOR_H_
#define _EMBEDDED_DATABASE_I_CURSOR_H_

#include "IField.h"
#include "IRow.h"

namespace embDB
{
	
	class ICursor : public CommonLib::AutoRefCounter
	{
	public:
		ICursor(){}
		virtual ~ICursor(){}
		virtual bool begin() = 0;
		virtual bool next(IRow * pRow) = 0;
		virtual IField* getField(uint32 nCount) = 0;
		virtual uint32 getFieldCount() const = 0;
		virtual CommonLib::IVariant* value(const wchar_t* pszName) = 0;
		virtual bool set(const CommonLib::str_t& sName, CommonLib::IVariant*) = 0;

		virtual IRowPtr createRow() = 0;

		
	};

	class IInsertCursor : public ICursor
	{
		IInsertCursor(){}
		virtual ~IInsertCursor(){}
		virtual int64 insert(IRow* pRow) = 0;
	};
	class IUpdateCursor : public ICursor
	{
		IUpdateCursor(){}
		virtual ~IUpdateCursor(){}
		virtual bool update(IRow* pRow) = 0;
	};
	class IDeleteCursor : public ICursor
	{
		IDeleteCursor(){}
		virtual ~IDeleteCursor(){}
		virtual bool remove(IRow* pRow) = 0;
	};

	typedef CommonLib::IRefCntPtr<ICursor> ICursorPtr;
	typedef CommonLib::IRefCntPtr<IInsertCursor> IInsertCursorPtr;
	typedef CommonLib::IRefCntPtr<IUpdateCursor> IUpdateCursorPtr;
	typedef CommonLib::IRefCntPtr<IDeleteCursor> IDeleteCursorPtr;
}
#endif