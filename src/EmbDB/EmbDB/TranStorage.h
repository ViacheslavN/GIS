#ifndef _EMBEDDED_DATABASE_I_TRANSACTIONS_STORAGE_H_
#define _EMBEDDED_DATABASE_I_TRANSACTIONS_STORAGE_H_

#include "CommonLibrary/File.h"
#include "CommonLibrary/str_t.h"
#include "FilePage.h"
#include "CommonLibrary/alloc_t.h"
#include "TranPerfCounter.h"
namespace embDB
{
	class CTranStorage
	{
	public:
		CTranStorage(CommonLib::alloc_t *pAlloc, CTranPerfCounter *pCounter);
		~CTranStorage();
		bool open(const CommonLib::str_t& sTranName, size_t nPageSize, bool bNew);
		int64 saveFilePage(CFilePage* pPage, int64 nAddr = -1); //если nAddr = -1, то возвращаеться новый адрес
		CFilePage* getFilePage(int64 nAddr, bool bRead = true);
		CFilePage* getNewPage();
		int64 getNewPageAddr();
		bool close(bool bDelete = true);
		size_t getPageSize(){return m_nPageSize;}
		bool Flush();
		void error(const CommonLib::str_t& sError){}

	private:
		CommonLib::CFile m_pFile;
		CommonLib::alloc_t *m_pAlloc;
		int64 m_nLastAddr;
		size_t m_nPageSize;
		CommonLib::str_t m_sTranName;
		CTranPerfCounter *m_pCounter;
	};
}
#endif