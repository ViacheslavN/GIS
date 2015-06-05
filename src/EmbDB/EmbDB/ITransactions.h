#ifndef _EMBEDDED_DATABASE_I_TRANSACTIONS_H_
#define _EMBEDDED_DATABASE_I_TRANSACTIONS_H_
#include "IStatement.h"
#include "IRecordset.h"
namespace embDB
{

	enum eTransactionsType
	{
		eTT_UNDEFINED=1,  //��������������� ��� ��� ����� ���� �����
		eTT_INSERT = 2,
		eTT_DELETE = 4,
		eTT_SELECT = 8,
		eTT_DDL = 16  
	};

class ITransactions
{
	public:
		ITransactions(){}
		virtual ~ITransactions(){}


		virtual eTransactionsType getType() const = 0;
		virtual bool begin() = 0;
		virtual bool commit() = 0;
		virtual bool rollback() = 0;
		virtual bool isError() const = 0 ;
		virtual size_t getErrorMessageSize() const = 0;
		virtual size_t getErroMessage(wchar_t * pBuf, size_t nSize) const = 0;

		virtual IStatement* createStatement(const wchar_t *pszSQLQuery) = 0;
		virtual IRecordset* executeQuery(IStatement* pStatement) = 0;
		virtual IRecordset* executeQuery(const wchar_t* pszQuery = NULL) = 0;

};

}
#endif