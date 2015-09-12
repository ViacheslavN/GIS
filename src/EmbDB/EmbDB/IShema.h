#ifndef _EMBEDDED_DATABASE_I_SHEMA_H_
#define _EMBEDDED_DATABASE_I_SHEMA_H_
#include "CommonLibrary/String.h"
#include "ITable.h"

namespace embDB
{
	class IShema
	{
		public:
			IShema(){}
			virtual ~IShema(){}
			virtual size_t getTableCnt() const = 0;
			virtual ITable* getTable(size_t nIndex) const = 0;
			virtual ITable* getTableByName(const wchar_t* pszTableName) const = 0;
	};
}

#endif