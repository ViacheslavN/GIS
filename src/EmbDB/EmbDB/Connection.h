#ifndef _EMBEDDED_CONNECTION_DB_H_
#define _EMBEDDED_CONNECTION_DB_H_

 
#include "CommonLibrary/String.h"
#include "embDBInternal.h"
 
namespace embDB
{

	class CDatabase;
	class CConnection : public IConnection
	{
		public:

			CConnection(CDatabase *pDB, const wchar_t *pszUserName, uint64 nUID);
			virtual ~CConnection();

			virtual ITransactionPtr startTransaction(eTransactionType trType);
			virtual bool closeTransaction(ITransaction* );
			virtual uint64 getUserUID() const;
			virtual const wchar_t *getUserName() const;

		private:
			CDatabase *m_pDB;
			CommonLib::CString m_sUserName;
			uint64 m_nUID;
	};
}

#endif