#ifndef _EMBEDDED_DATABASE_I_TRANSACTIONS_STORAGE_H_
#define _EMBEDDED_DATABASE_I_TRANSACTIONS_STORAGE_H_

#include "CommonLibrary/File.h"
#include "CommonLibrary/String.h"
#include "../../storage/FilePage.h"
#include "CommonLibrary/alloc_t.h"
#include "TranPerfCounter.h"
#include "../../Crypto/PageCipher.h"
#include <memory>
namespace embDB
{
	class CTranStorage
	{
	public:
		CTranStorage(CommonLib::alloc_t *pAlloc, CTranPerfCounter *pCounter, bool bCheckCRC,
			CPageCipher* pPageCrypto = nullptr);
		~CTranStorage();
		bool open(const CommonLib::CString& sTranName, /*uint32 nPageSize,*/ bool bNew);
		int64 saveFilePageWithRetAddr(CFilePage* pPage, int64 nAddr = -1); //���� nAddr = -1, �� ������������� ����� �����
		FilePagePtr getFilePage(int64 nAddr, uint32 nSize, bool bRead = true, bool bDecrypt = true);
		FilePagePtr getNewPage(uint32 nSize);
		int64 getNewPageAddr(uint32 nSize);
		bool close(bool bDelete = true);
		uint32 getPageSize(){return m_nPageSize;}
		bool Flush();
		void error(const CommonLib::CString& sError){}

		bool saveFilePage(CFilePage* pPage, uint32 nDataSize = 0);
		bool saveFilePage(FilePagePtr pPage, uint32 nDataSize = 0);

	private:
		CommonLib::CFile m_pFile;
		CommonLib::alloc_t *m_pAlloc;
		int64 m_nLastAddr;
		uint32 m_nPageSize;
		CommonLib::CString m_sTranName;
		CTranPerfCounter *m_pCounter;
		CPageCipher* m_pPageChiper;
		 CommonLib::CBlob m_BufForChiper;
		bool m_bCheckCRC;
	};
}
#endif