#ifndef _EMBEDDED_CONNECTION_DB_H_
#define _EMBEDDED_CONNECTION_DB_H_

 
#include "CommonLibrary/String.h"
#include "embDBInternal.h"
 
namespace embDB
{

	class CDatabase;
	class CConnection : public IDBConnection
	{
		public:

			CConnection(CDatabase* pDB, IDBShema* pSchema, const wchar_t *pszUserName, uint64 nUID, eUserGroup userGroup);
			virtual ~CConnection();

			virtual IDatabase* getDB() const {return (IDatabase*)m_pDB;}
			virtual ITransactionPtr startTransaction(eTransactionType trType);
			virtual bool closeTransaction(ITransaction* );
			virtual uint64 getUserUID() const;
			virtual const wchar_t *getUserName() const;
			virtual ISchemaPtr getSchema() const {return ISchemaPtr(m_pSchema.get());}
			virtual IDBStoragePtr getDBStorage() const;
			virtual bool getCheckCRC() const;
			virtual eUserGroup getUserGroup() const;
		private:
			CDatabase* m_pDB;
			CommonLib::CString m_sUserName;
			uint64 m_nUID;
			eUserGroup m_UserGroup;
			IDBShemaPtr m_pSchema;
	};
}

#endif