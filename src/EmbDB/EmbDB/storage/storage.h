#ifndef _EMBEDDED_DATABASE_STORAGE_H_
#define _EMBEDDED_DATABASE_STORAGE_H_

#include "CommonLibrary/File.h"
#include "CommonLibrary/String.h"
#include "../embDBInternal.h"
//#define USE_FREE_PAGES
#ifdef  USE_FREE_PAGES
	#include "FreePageManage.h"
#endif
#include "StorageInfo.h"
#include "../utils/alloc/MemPageCache.h"
#include "../Crypto/PageCipher.h"
#include "../utils/CacheLRU.h"
#include "../utils/CacheLRU_2Q.h"
#include <iostream>
#include <memory>


#include "CommonLibrary/LockObject.h"

namespace embDB
{
	class CStorage : public IDBStorage
	{
	public:
		CStorage( CommonLib::alloc_t *pAlloc, int32 nCacheSize = 1000, bool bCheckCRC = true,
			bool bMultiThread = true);
		~CStorage();

		virtual FilePagePtr getFilePage(int64 nAddr, uint32 nSize, bool bRead = true, bool bNeedDecrypt = true, bool bAddInCache = true, bool bForChanghe = true);

		virtual bool dropFilePage(FilePagePtr pPage);
		virtual  bool dropFilePage(int64 nAddr);
		virtual  FilePagePtr getNewPage(uint32 nSize = 0, bool bWrite = false, bool bAddInCache = true);
		virtual  bool saveFilePage(CFilePage* pPage,  bool bChangeInCache = false);
		virtual  bool saveFilePage(FilePagePtr pPage, bool bChangeInCache = false);
		virtual bool saveNewPage(FilePagePtr pPage);
		virtual int64 getNewPageAddr(uint32 nSize = 0);

		virtual bool commit();
		virtual bool removeFromFreePage(int64 nAddr);


	
		virtual bool WriteRowData(const byte* pData, uint32 nSize, int64 nPos = -1);
		virtual bool ReadRowData(const byte* pData, uint32 nSize, int64 nPos = -1);
		virtual bool setFileSize(int64 nSize);




	//	virtual bool isLockWrite();
		virtual bool lockWrite(IDBTransaction *pTran = NULL);
		virtual bool try_lockWrite(){return false;}
		virtual bool unlockWrite(IDBTransaction *pTran = NULL);
		virtual void clearDirty();



		virtual bool isLock(){return false;}
		virtual bool lock(IDBTransaction *pTran = NULL) {return false;}
		virtual bool try_lock(){return false;}
		virtual bool unlock(IDBTransaction *pTran = NULL){return false;}

		virtual bool saveForUndoState(IDBTransaction *pTran);
		virtual bool undo(IDBTransaction *pTran);
		virtual bool reload();

		bool isValid() const;

		virtual bool saveState();

		virtual bool open(const wchar_t* pszName, bool bReadOnle, bool bNew, bool bCreate, bool bOpenAlways/*, uint32 nPageSize*/);
		virtual bool close();
		//virtual void setPageSize(uint32 nPageSize);
		//virtual uint32 getPageSize() const;
		virtual int64 getFileSize();
		virtual int64 getBeginFileSize() const;
		virtual bool isDirty() const;
		virtual const CommonLib::CString & getTranFileName() const;
		virtual eDBTransationType  getTranDBType() const;

		void setStoragePageInfo(int64 nStorageInfo);
		bool initStorage(int64 nStorageInfo);
		bool loadStorageInfo();
		bool saveStorageInfo();
		virtual void SetOffset(int64 nOffset);
		virtual int64 GetOffset() const;

		virtual bool getCheckCRC() const {return m_bCheckCRC;}

		virtual void error(const wchar_t *pszFormat, ...){}
		virtual void log(uint32 nLevel, const wchar_t *pszFormat, ...){}
		virtual void log_msg(uint32 nLevel, const wchar_t *pszMsg){}

		 void setPageChiper(CPageCipher *pCipher) {m_pPageChiper = pCipher;}
	private:
		 CommonLib::CFile m_pFile;
		 CommonLib::alloc_t *m_pAlloc;
		// typedef RBMap<int64, CFilePage*> TPageCache;
		 //typedef TList<int64> TPageList;
		 int32 m_nMaxPageBuf;
		// typedef TCacheLRU<int64, CFilePage> TNodesCache;


		 struct TPageFreeChecker
		 {
			 bool IsFree(FilePagePtr& pObj)
			 {

				
				 return  pObj->isRemovable(1);
				 //return pObj->IsFree();
			 }
		 };


		 typedef TCacheLRU_2Q<int64, FilePagePtr, TPageFreeChecker> TNodesCache;
		// typedef TSimpleCache<int64, CFilePage> TNodesCache;
		 TNodesCache m_Chache;
		 //TPageList   m_FreePageDisk;
		 uint32 m_nBasePageSize;
		 int64 m_nLastAddr;
#ifdef USE_FREE_PAGES
		 CFreePageManager m_FreePageManager;
#endif
		 //CStorageInfo m_StorageStateInfo;

		 bool m_bDirty;
		 CommonLib::CString m_sTranName;
		 eDBTransationType m_nTrantype;
		 int64 m_nBeginSize;
		 int64 m_nStorageInfo;
		 int64 m_nOffset;

		 bool m_bCommitState;
		 uint64 m_nCalcFileSize;
		// CMemPageCache m_MemCache;
		 CPageCipher *m_pPageChiper;
		 CommonLib::CBlob m_BufForChiper;
		 bool m_bCheckCRC;


		 CommonLib::ILockObject *m_pCommonLockObj;
		 CommonLib::CEmptyLockObject m_ComEmptyLock;
		 CommonLib::CSLockObject m_ComLock;


		 CommonLib::ILockObject *m_pWriteLockObj;
		 CommonLib::CEmptyLockObject m_WriteEmptyLock;
		 CommonLib::CSLockObject m_WriteLock;
	};
}


#endif //_EMBEDDED_DATABASE_STORAGE_H_