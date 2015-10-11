#ifndef _EMBEDDED_DATABASE_STORAGE_H_
#define _EMBEDDED_DATABASE_STORAGE_H_

#include "CommonLibrary/File.h"
#include "CommonLibrary/String.h"
#include "RBMap.h"
#include "List.h"

#include "DBConfig.h"
#include "IDBStorage.h"
#include "FreePageManage.h"
#include "IDBTransactions.h"
#include "StorageInfo.h"
#include "MemPageCache.h"
#include "PageCrypto.h"
#include "CacheLRU.h"
//#define USE_FREE_PAGES
namespace embDB
{
	class CStorage : public IDBStorage
	{
	public:
		CStorage( CommonLib::alloc_t *pAlloc, int32 nCacheSize = 1000);
		~CStorage();

		virtual FilePagePtr getFilePage(int64 nAddr, bool bRead = true, uint32 nSize = 0);
		virtual bool dropFilePage(FilePagePtr pPage);
		virtual  bool dropFilePage(int64 nAddr);
		virtual  FilePagePtr getNewPage(bool bWrite = false, uint32 nSize = 0);
		virtual  bool saveFilePage(CFilePage* pPage, size_t nDataSize = 0,  bool bChandgeInCache = false);
		virtual  bool saveFilePage(FilePagePtr pPage, size_t nDataSize = 0,  bool bChandgeInCache = false);
		virtual bool saveNewPage(FilePagePtr pPage);
		virtual int64 getNewPageAddr(uint32* nType = NULL, uint32 nSize = 0);
		//virtual FilePagePtr createPage(int64 nAddr);
		virtual bool commit();
		virtual bool removeFromFreePage(int64 nAddr);

		virtual int64 getFileSize() ;
		virtual bool setFileSize(int64 nSize);

		virtual bool isLockWrite(){return false;}
		virtual bool lockWrite(IDBTransactions *pTran = NULL);
		virtual bool try_lockWrite(){return false;}
		virtual bool unlockWrite(IDBTransactions *pTran = NULL);
		virtual void clearDirty();

		virtual bool saveForUndoState(IDBTransactions *pTran);
		virtual bool undo(IDBTransactions *pTran);
		virtual bool reload();

		bool isValid() const;

		virtual bool saveState();

		bool open(const wchar_t* pszName, bool bReadOnle, bool bNew, bool bCreate, bool bOpenAlways, size_t nPageSize);
		bool close();
		void setPageSize(size_t nPageSize);
		size_t getPageSize() const;
		int64 getFileSzie();
		virtual int64 getBeginFileSize() const;
		virtual bool isDirty() const;
		virtual const CommonLib::CString & getTranFileName() const;

		void setStoragePageInfo(int64 nStorageInfo);
		bool initStorage(int64 nStorageInfo);
		bool loadStorageInfo();
		bool saveStorageInfo();



		 void error(const CommonLib::CString& sError){}
	private:
		 CommonLib::CFile m_pFile;
		 CommonLib::alloc_t *m_pAlloc;
		// typedef RBMap<int64, CFilePage*> TPageCache;
		 //typedef TList<int64> TPageList;
		 int32 m_nMaxPageBuf;
		 typedef TCacheLRU<int64, CFilePage> TNodesCache;
		// typedef TSimpleCache<int64, CFilePage> TNodesCache;
		 TNodesCache m_Chache;
		 //TPageList   m_FreePageDisk;
		 size_t m_nPageSize;
		 int64 m_nLastAddr;
#ifdef USE_FREE_PAGES
		 CFreePageManager m_FreePageManager;
#endif
		 //CStorageInfo m_StorageStateInfo;

		 bool m_bDirty;
		 CommonLib::CString m_sTranName;
		 int64 m_nBeginSize;
		 int64 m_nStorageInfo;

		 bool m_bCommitState;
		 uint64 m_nCalcFileSize;
		 CMemPageCache m_MemCache;
		 IPageCrypto *m_pPageCrypto;
		 std::auto_ptr<CFilePage> m_pBufPageCrypto; 
	};
}


#endif //_EMBEDDED_DATABASE_STORAGE_H_