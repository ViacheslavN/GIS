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


	//	virtual bool insert(IRecordset *pRecordSet, IDBTransactions *Tran = NULL) = 0;
	//	virtual bool insert(INameRecordset *pRecordSet, IDBTransactions *Tran = NULL) = 0;

		virtual IField* createField(SFieldProp& sFP) = 0;
		virtual bool deleteField(IField* pField) = 0;
		virtual bool createIndex(const CommonLib::str_t& sName, SIndexProp& ip) = 0;
		virtual bool createCompositeIndex(std::vector<CommonLib::str_t>& vecFields, SIndexProp& ip) = 0;
	};
}
#endif



