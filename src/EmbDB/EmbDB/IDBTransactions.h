#ifndef _EMBEDDED_DATABASE_I_DB_TRANSACTIONS_H_
#define _EMBEDDED_DATABASE_I_DB_TRANSACTIONS_H_
#include "ITransactions.h"
#include "FilePage.h"
#include "CommonLibrary/String.h"
 
namespace embDB
{

	enum eRestoreType
	{
		rtUndefined,
		rtUndo,
		rtRedo
	};
	class IDBStorage;
	class IDBBtree
	{
		public:
			virtual bool commit() = 0;
	};
	class IDBTransactions : public ITransactions
	{
	public:
		IDBTransactions(){}
		virtual ~IDBTransactions(){}


		virtual bool restore(bool Force = false) = 0;


		virtual FilePagePtr getFilePage(int64 nAddr, bool bRead = true) = 0;
		virtual void dropFilePage(FilePagePtr pPage) = 0;
		virtual void dropFilePage(int64 nAddr) = 0;
		virtual FilePagePtr getNewPage() = 0;
		virtual void saveFilePage(FilePagePtr pPage,  size_t nSize = 0,  bool bChandgeInCache = false) = 0;
		virtual size_t getPageSize() const = 0;


		virtual FilePagePtr getTranNewPage()= 0;
		virtual FilePagePtr getTranFilePage(int64 nAddr, bool bRead = true) = 0;
		virtual void saveTranFilePage(FilePagePtr pPage,  size_t nSize = 0,  bool bChandgeInCache = false) = 0;
		virtual void addUndoPage(FilePagePtr pPage, bool bReadFromDB = false) = 0;

		virtual void addInnerTransactions(IDBTransactions *pTran) = 0;

		virtual void error(const wchar_t *pszFormat, ...) = 0;
		virtual uint32 getLogLevel() const = 0;
		virtual void log(uint32 nLevel, const wchar_t *pszFormat, ...) = 0;

		virtual void addDBBTree(IDBBtree *pTree) = 0;
		virtual int64 getID() const = 0;
		virtual const CommonLib::CString& getFileTranName() const = 0;
		virtual bool isCompleted() const = 0;
		virtual void setDBStorage(IDBStorage *pStorage)  = 0;
		virtual void stop() = 0;
		virtual void wait() = 0;

	};

}
#endif