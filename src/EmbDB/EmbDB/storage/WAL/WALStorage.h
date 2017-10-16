#pragma once

#include "CommonLibrary/File.h"
#include "CommonLibrary/String.h"
#include "../../embDBInternal.h"
#include "../FilePage.h"
#include "../../Crypto/PageCipher.h"
#include "../../utils/CacheLRU.h"
#include "../../utils/CacheLRU_2Q.h"
namespace embDB
{

	class CWALStorage : public IDBWALStorage
	{
		public:
			struct STranPageInfo
			{
				int64 m_nLogTranAddr;
				int64 m_nDBAddr;
				uint32 m_nSize;


				STranPageInfo() : m_nLogTranAddr(-1), m_nDBAddr(-1), m_nSize(0)
				{}
			};


		public:
			CWALStorage(CommonLib::alloc_t *pAlloc, int32 nCacheSize = 1000, bool bCheckCRC = true);
			~CWALStorage();

			virtual FilePagePtr getFilePage(int64 nAddr, uint32 nSize, bool bRead = true, bool bNeedDecrypt = true);
			virtual FilePagePtr getNewPage(uint32 nSize, bool bWrite = false);
			virtual bool saveFilePage(CFilePage* pPage, uint32 nDataSize = 0, bool ChandgeInCache = false);
			virtual bool saveFilePage(FilePagePtr pPage, uint32 nDataSize = 0, bool ChandgeInCache = false);

			virtual void MoveAllPage();
			virtual IDBStoragePtr GetMainStorage();

			virtual bool open(const wchar_t* pszName, bool bReadOnle, bool bNew, bool bCreate, bool bOpenAlways);
			virtual bool close();

			void setPageChiper(CPageCipher *pCipher) { m_pPageChiper = pCipher; }
			void setStorage(IDBStorage *pStorage) { m_pDBStorage = pStorage; }
		private:

			void stopCopy();
			void copyPage();
			bool IsCopy();
		private:
			IDBStoragePtr m_pDBStorage;
			IDBStoragePtr m_pDBLogStorage;
			typedef std::map<int64, std::pair<int64, uint32> > TPageAddrs;
			TPageAddrs m_PageAddrs;
			CPageCipher *m_pPageChiper;

			typedef std::vector<STranPageInfo> TTranPage;
			typedef std::map<int64, TTranPage> TTransactions;

			TTransactions m_Transactions;


		
	};
}