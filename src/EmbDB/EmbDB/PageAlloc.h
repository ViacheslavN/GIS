#ifndef _EMBEDDED_DATABASE_POOL_ALLOC_H_
#define _EMBEDDED_DATABASE_POOL_ALLOC_H_

#include "Commonlibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include <algorithm>
#include "list.h"
#include <map>

namespace embDB
{
	class CPageAlloc : CommonLib::alloc_t
	{

		struct SMemPage
		{
			SMemPage() :
				m_pBuf(0), m_nPos(0), m_nCnt(0)
				{}

			SMemPage(void *pBuf, uint32 nPos, uint32 nCnt) :
				m_pBuf(pBuf), m_nPos(nPos), m_nCnt(nCnt)
			{}
			void *m_pBuf;
			uint32 m_nPos;
			uint32 m_nCnt;

		};
		public:
			CPageAlloc(CommonLib::alloc_t *pAlloc, uint32 nMemPageSize, uint32 nBeginPageCnt);
			~CPageAlloc();

			virtual void* alloc(uint32 size);
			virtual void  free(void* buf);
		private:
			void* allocFromPage(SMemPage* page, uint32 nSize);
			void free(SMemPage*pPage, void *ptr);
		private:
			CommonLib::alloc_t* m_pAlloc;
			CommonLib::simple_alloc_t m_alloc;
			std::map<void*, SMemPage*> m_mapPage;
			std::list<SMemPage*> m_listFreePage;
			uint32 m_nMemPageSize;
			SMemPage *m_pCurrPage;

	};
}

#endif