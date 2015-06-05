#ifndef _EMBEDDED_DATABASE_I_DB_H_
#define _EMBEDDED_DATABASE_I_DB_H_
#include "CommonLibrary/str_t.h"
#include "ITransactions.h"
#include "IShema.h"
#include "ITable.h"
namespace embDB
{

	enum DBTransactionMode
	{
		eTMSingleTransactions,
		eTMOneReadersManywriter,
		eTMMultiTransactions
	};
	class IDatabase
	{
	public:
		IDatabase(){}
		virtual ~IDatabase(){}
		virtual bool open(const wchar_t* pszName, DBTransactionMode mode = eTMMultiTransactions, const wchar_t* pszWorkingPath = NULL, const wchar_t* pszPassword = NULL)  = 0;
		virtual bool create(const wchar_t* pszDbName, size_t nPageSize, DBTransactionMode mode = eTMMultiTransactions, const wchar_t* pszWorkingPath = NULL, const wchar_t* pszPassword = NULL)  = 0;
		virtual bool close()  = 0;
		virtual ITransactions* startTransaction(eTransactionsType trType) = 0;
		virtual bool closeTransaction(ITransactions* ) = 0;
		virtual IShema* getShema() = 0;
 
	};
}
#endif