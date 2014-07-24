#ifndef _EMBEDDED_DATABASE_I_SIMPLE_TRANSACTIONS_STORE_H_
#define _EMBEDDED_DATABASE_I_SIMPLE_TRANSACTIONS_STORE_H_

#include "CommonLibrary/File.h"
#include "CommonLibrary/str_t.h"
#include "FilePage.h"
namespace embDB
{
	class CSimpleTranStorage
	{
	public:
		CSimpleTranStorage(CommonLib::alloc_t *pAlloc);
		~CSimpleTranStorage();
		bool open(const CommonLib::str_t& sDbName, size_t nPageSize);
		int64 saveFilePage(CFilePage* pPage, int64 nAddr = -1); //если nAddr = -1, то возвращаеться новый адрес
		CFilePage* getFilePage(int64 nAddr);
		bool close();
		size_t getPageSize(){return m_nPageSize;}
	private:
		CommonLib::CFile m_pFile;
		CommonLib::alloc_t *m_pAlloc;
		int64 m_nLastAddr;
		 size_t m_nPageSize;
	};
}
#endif