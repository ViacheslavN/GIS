#ifndef _EMBEDDED_DATABASE_I_DB_TABLE_H_
#define _EMBEDDED_DATABASE_I_DB_TABLE_H_

#include "ITable.h"
#include "IDBTransactions.h"
namespace embDB
{
	class IDBTable : public ITable
	{
	public:
		IDBTable(){}
		virtual ~IDBTable(){}

		virtual bool isLockWrite() = 0;
		virtual bool lockWrite() = 0;
		virtual bool try_lockWrite() = 0;
		virtual bool unlockWrite() = 0;


		virtual bool insert(IRecordset *pRecordSet, IDBTransactions *Tran = NULL) = 0;
		virtual bool insert(INameRecordset *pRecordSet, IDBTransactions *Tran = NULL) = 0;
	};
}
#endif



