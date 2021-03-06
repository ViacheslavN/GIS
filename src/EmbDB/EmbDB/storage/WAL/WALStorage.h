#pragma once

#include "CommonLibrary/File.h"
#include "CommonLibrary/String.h"
#include "../../embDBInternal.h"
#include "../FilePage.h"
#include "../../Crypto/PageCipher.h"
#include "../../utils/CacheLRU.h"
#include "../../utils/CacheLRU_2Q.h"

#include "Utils/streams/WriteStreamPage.h"
#include "Utils/streams/ReadStreamPage.h"

namespace embDB
{

	class CWALStorage : public IDBWALStorage
	{

		struct SHeader
		{
			SHeader() : m_nCheckPointList(-1)
			{}

			int64 m_nCheckPointList;
		};

		public:
		 

		public:
			CWALStorage(CommonLib::alloc_t *pAlloc, int32 nCacheSize = 1000, bool bCheckCRC = true, bool bMultiThread = true);
			~CWALStorage();

			virtual FilePagePtr getFilePage(int64 nAddr, uint32 nSize = 0, bool bRead = true, bool bNeedDecrypt = true,  bool bForChanghe = true);
			virtual FilePagePtr getNewPage(uint32 nSize = 0, bool bWrite = false);
			virtual bool saveFilePage(CFilePage* pPage,  bool ChandgeInCache = false);
			virtual bool saveFilePage(FilePagePtr pPage,  bool ChandgeInCache = false);

			virtual void MoveAllPage();
			virtual IDBStoragePtr GetMainStorage() {return m_pDBStorage;}
			virtual IDBStoragePtr GetTranLogStorage() { return m_pDBLogStorage; }

			virtual bool open(const wchar_t* pszName, bool bReadOnle, bool bNew, bool bCreate, bool bOpenAlways);
			virtual bool close();
			virtual bool commit(IDBWALTransaction *pTran);

			void setPageChiper(CPageCipher *pCipher) { m_pPageChiper = pCipher; }
			bool intit(IDBStorage *pDBStorage, IDBStorage *pTranLogStorage, bool bNew);

			virtual FilePagePtr getNewTranLogPage(uint32 nSize, bool bWrite = false, bool bAddCache = false);
			virtual FilePagePtr getTranLogPage(int64 nAddr, uint32 nSize, bool bRead = true, bool bNeedDecrypt = true, bool bAddInCache = true);
			virtual int64 getNewTranLogPageAddr(uint32 nSize = 0);



			virtual TCheckPointPages& getCheckPoint(int64 nAddr);
			virtual  void UpdateCheckPoint(int64 nAddr);

			void SetOffset(uint32 nLogTranOffset, uint32 nDBFileOffset);

		private:

			void stopCopy();
			void copyPage();
			bool IsCopy();
		private:
			IDBStoragePtr m_pDBStorage;
			IDBStoragePtr m_pDBLogStorage;
 


			typedef std::map<int64, int64> TPageAddrs;
			TPageAddrs m_PageAddrs;
			CPageCipher *m_pPageChiper;
			 
			TCheckPoints m_CheckPoints;
			bool m_bMultiThread;
			typedef TWriteStreamPage<IFilePage> TCheckPointStream;
			std::unique_ptr<TCheckPointStream> m_pCheckPointStream;

		
	};
}