#ifndef _EMBEDDED_DATABASE__I_DB_STORAGE_H_
#define _EMBEDDED_DATABASE__I_DB_STORAGE_H_
#include "FilePage.h"
#include "IDBTransactions.h"
namespace embDB
{
	class IDBStorage
	{
	public:
		IDBStorage(){}
		virtual ~IDBStorage(){};
		
		virtual FilePagePtr getFilePage(int64 nAddr, bool bRead = true) = 0;
		virtual bool dropFilePage(FilePagePtr pPage) = 0;
		virtual bool dropFilePage(int64 nAddr) = 0;
		virtual FilePagePtr getNewPage(bool bWrite = false) = 0;
		virtual bool saveFilePage(CFilePage* pPage, size_t nDataSize = 0,  bool ChandgeInCache = false) = 0;
		virtual bool saveFilePage(FilePagePtr pPage, size_t nDataSize = 0,  bool ChandgeInCache = false) = 0;
		virtual bool saveNewPage(FilePagePtr pPage) = 0;
		virtual size_t getPageSize() const = 0;
		virtual int64 getNewPageAddr() = 0;
		//virtual FilePagePtr createPage(int64 nAddr) = 0;
		virtual bool commit() = 0;
		virtual bool removeFromFreePage(int64 nAddr) = 0;
		//virtual bool saveChache() = 0;
		///для восстановления после сбоев, или отката транзакции
		virtual int64 getFileSize()  = 0;
		virtual bool setFileSize(int64 nSize) = 0;
		virtual int64 getBeginFileSize() const = 0;
		virtual bool isDirty() const = 0;
		virtual const CommonLib::str_t & getTranFileName() const = 0;
		virtual void clearDirty() = 0;

		//for write/save
		virtual bool isLockWrite() = 0;
		virtual bool lockWrite(IDBTransactions *pTran = NULL) = 0;
		virtual bool try_lockWrite() = 0;
		virtual bool unlockWrite(IDBTransactions *pTran = NULL) = 0;

		virtual bool saveForUndoState(IDBTransactions *pTran) = 0;
		virtual bool undo(IDBTransactions *pTran) = 0;
		virtual bool reload()= 0;
		//
		virtual bool saveState() = 0;
	};
}

#endif