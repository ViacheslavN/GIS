#ifndef _EMBEDDED_DATABASE_I_DB_H_
#define _EMBEDDED_DATABASE_I_DB_H_
#include "CommonLibrary/str_t.h"
#include "ITransactions.h"
#include "IShema.h"
namespace embDB
{

	enum DBTransactionMode
	{
		eTMSingleTransactions,
		eTMMultiTransactions
	};
	class IDatabase
	{
	public:
		IDatabase(){}
		virtual ~IDatabase(){}
		virtual bool open(const CommonLib::str_t& sDbName, const CommonLib::str_t& sWorkingPath = "")  = 0;
		virtual bool create(const CommonLib::str_t& sDbName, size_t nPageSize, const CommonLib::str_t& sWorkingPath = "")  = 0;
		virtual bool close()  = 0;
		virtual ITransactions* startTransaction(eTransactionsType trType) = 0;
		virtual bool closeTransaction(ITransactions* ) = 0;
	};
}
#endif