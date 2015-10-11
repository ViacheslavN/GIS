#ifndef _EMBEDDED_DATABASE_I_TRANSACTIONS_STORAGE_H_
#define _EMBEDDED_DATABASE_I_TRANSACTIONS_STORAGE_H_

#include "CommonLibrary/File.h"
#include "CommonLibrary/String.h"
#include "FilePage.h"
#include "CommonLibrary/alloc_t.h"
#include "TranPerfCounter.h"
#include "PageCrypto.h"
namespace embDB
{
	class CTranStorage
	{
	public:
		CTranStorage(CommonLib::alloc_t *pAlloc, CTranPerfCounter *pCounter);
		~CTranStorage();
		bool open(const CommonLib::CString& sTranName, size_t nPageSize, bool bNew);
		int64 saveFilePage(CFilePage* pPage, int64 nAddr = -1); //���� nAddr = -1, �� ������������� ����� �����
		CFilePage* getFilePage(int64 nAddr, bool bRead = true, bool bDecrypt = true, uint32 nSize = 0);
		CFilePage* getNewPage(uint32 nSize = 0);
		int64 getNewPageAddr(uint32 nSize = 0);
		bool close(bool bDelete = true);
		size_t getPageSize(){return m_nPageSize;}
		bool Flush();
		void error(const CommonLib::CString& sError){}

	private:
		CommonLib::CFile m_pFile;
		CommonLib::alloc_t *m_pAlloc;
		int64 m_nLastAddr;
		size_t m_nPageSize;
		CommonLib::CString m_sTranName;
		CTranPerfCounter *m_pCounter;
		IPageCrypto* m_pPageCrypto;
		std::auto_ptr<CFilePage> m_pBufPageCrypto;
	};
}
#endif