#ifndef _EMBEDDED_DATABASE_I_SIMPLE_TRANSACTIONS_H_
#define _EMBEDDED_DATABASE_I_SIMPLE_TRANSACTIONS_H_
#include "IDBTransactions.h"
#include "SimpleTranStorage.h"
#include "RBMap.h"
#include "TranCache.h"
#include "IDBStorage.h"

//for debug and testing
namespace embDB
{
	class SimpleTransactions : public IDBTransactions
	{
		public:
			SimpleTransactions(CommonLib::alloc_t *pAlloc, IDBStorage* pDBStorage);
			~SimpleTransactions();
			//ITransactions
			virtual bool begin();
			virtual bool commit();
			virtual bool rollback();

			virtual bool restore() 
			{
				return false;
			}

			virtual CFilePage* getFilePage(int64 nAddr, bool bRead = true);
			virtual void dropFilePage(CFilePage* pPage);
			virtual void dropFilePage(int64 nAddr);
			virtual CFilePage* getNewPage();
			virtual void saveFilePage(CFilePage* pPage,   size_t nSize = 0, bool bChandgeInCache = false);
			virtual size_t getPageSize() const;
			virtual eTransactionsType getType() const {return (eTransactionsType)m_nTranType;}
			void setType(eTransactionsType type); //hack

			virtual bool isError() const;
			virtual size_t getErrorMessageSize() const;
			virtual size_t getErroMessage(wchar_t * pBuf, size_t nSize) const;

			virtual void addInnerTransactions(IDBTransactions *pTran){}
			virtual void error(const CommonLib::str_t& sError, uint32 nErrorID = 0);
			virtual uint32 getLogLevel() const;
			virtual void log(uint32 nLevel, const CommonLib::str_t& sMessage);
			virtual void addDBBTree(IDBBtree *pTree);
			virtual int64 getID() const {return 1;}
			virtual const CommonLib::str_t& getFileTranName() const {return m_sFileName;}
			virtual bool  isCompleted(void) const {return true;}
			virtual void setDBStorage(IDBStorage *pStorage)  {m_pDBStorage = pStorage;}
			virtual void wait() {}
			virtual void stop() {}
		private:
			//CSimpleTranStorage m_Storage;
			IDBStorage* m_pDBStorage;
			typedef RBMap<int64, int> TPages; //все страницы
			TPages m_pages;
			//TSimpleTranCache m_PageChache;
			CommonLib::alloc_t *m_pAlloc;
			//int64 m_nFakeAddr;
			bool m_bCommit;
			int m_nTranType;
			bool m_bError;
			CommonLib::str_t m_sFileName;

	};
}

#endif