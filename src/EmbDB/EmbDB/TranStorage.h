#ifndef _EMBEDDED_DATABASE_I_TRANSACTIONS_STORAGE_H_
#define _EMBEDDED_DATABASE_I_TRANSACTIONS_STORAGE_H_

#include "CommonLibrary/File.h"
#include "CommonLibrary/String.h"
#include "FilePage.h"
#include "CommonLibrary/alloc_t.h"
#include "TranPerfCounter.h"
#include "PageCipher.h"
#include <memory>
namespace embDB
{
	class CTranStorage
	{
	public:
		CTranStorage(CommonLib::alloc_t *pAlloc, CTranPerfCounter *pCounter, bool bCheckCRC );
		~CTranStorage();
		bool open(const CommonLib::CString& sTranName, /*uint32 nPageSize,*/ bool bNew);
		int64 saveFilePage(CFilePage* pPage, int64 nAddr = -1); //если nAddr = -1, то возвращаеться новый адрес
		CFilePage* getFilePage(int64 nAddr, uint32 nSize, bool bRead = true, bool bDecrypt = true);
		CFilePage* getNewPage(uint32 nSize);
		int64 getNewPageAddr(uint32 nSize);
		bool close(bool bDelete = true);
		uint32 getPageSize(){return m_nPageSize;}
		bool Flush();
		void error(const CommonLib::CString& sError){}

	private:
		CommonLib::CFile m_pFile;
		CommonLib::alloc_t *m_pAlloc;
		int64 m_nLastAddr;
		uint32 m_nPageSize;
		CommonLib::CString m_sTranName;
		CTranPerfCounter *m_pCounter;
		IPageCipher* m_pPageCrypto;
		bool m_bCheckCRC;
	};
}
#endif