#ifndef _EMBEDDED_DATABASE_C_DIRECT_TRANSACTIONS_H_
#define _EMBEDDED_DATABASE_C_DIRECT_TRANSACTIONS_H_

#include "IDBTransactions.h"
#include "IDBStorage.h"

namespace embDB
{
	class CDirectTransactions : public IDBTransactions
	{
	public:

		CDirectTransactions(CommonLib::alloc_t* pAlloc,  IDBStorage* pDBStorage);
		//for compatible tests
		CDirectTransactions(CommonLib::alloc_t* pAlloc, eRestoreType nRestoreType,
			eTransactionsType nTranType, const CommonLib::str_t& sFileName, IDBStorage* pDBStorage, int64 nID);
		~CDirectTransactions();

		//ITransactions
		virtual bool begin(){return true;}
		virtual bool commit(){return m_pDBStorage->commit();}
		virtual bool rollback(){return true;}
		virtual bool restore(){return true;}
		bool close(){return true;}


		virtual CFilePage* getFilePage(int64 nAddr, bool bRead = true);
		virtual void dropFilePage(CFilePage* pPage);
		virtual void dropFilePage(int64 nAddr);
		virtual CFilePage* getNewPage();
		virtual void saveFilePage(CFilePage* pPage);
		virtual size_t getPageSize() const;
		virtual eTransactionsType getType() const {return eTT_UNDEFINED;}

		virtual void addInnerTransactions(IDBTransactions *pTran){}


		virtual bool isError() const
		{
			return m_bError;
		}
		virtual size_t getErrorMessageSize() const
		{
			return 0;
		}
		virtual size_t getErroMessage(wchar_t * pBuf, size_t nSize) const
		{
			return 0;
		}

		virtual void error(const CommonLib::str_t& sError, uint32 nErrorID = 0)
		{
			m_bError = true;
		}
		virtual uint32 getLogLevel() const 
		{
			return 0;
		}
		virtual void log(uint32 nLevel, const CommonLib::str_t& sMessage)
		{

		}

		virtual void addDBBTree(IDBBtree *pTree){}

		virtual int64 getID() const {return 0;}
		virtual const CommonLib::str_t& getFileTranName() const {return m_sFileName;}
		virtual bool isCompleted() const {return true;}
		virtual void setDBStorage(IDBStorage *pStorage)  {m_pDBStorage = pStorage;}
		virtual void wait() {}
		virtual void stop() {}
	private:
		IDBStorage* m_pDBStorage;
		bool m_bError;
		CommonLib::str_t m_sFileName;
};
}
#endif