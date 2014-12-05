#ifndef _EMBEDDED_DATABASE_MEM_PAGE_CACHE_H_
#define _EMBEDDED_DATABASE_MEM_PAGE_CACHE_H_

#include "Commonlibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "simple_stack.h"
#include <algorithm>
namespace embDB
{
	class CMemPageCache : public CommonLib::alloc_t
	{
		public:

			struct sMemPage
			{
				sMemPage() : m_pBuf(0)
				{}
				sMemPage(void* pBuf) : m_pBuf(pBuf)
				{}
				void* m_pBuf;
			};

			struct sMemPageSortCompare
			{
				bool operator()(const sMemPage& lMP, const sMemPage&  rMP) const
				{
					return  lMP.m_pBuf < rMP.m_pBuf;
				}
			};


			struct sMemPageFindCompare
			{

				sMemPageFindCompare(void* pFindAddr) : m_pFindAddr(pFindAddr)
				{

				}
				void *m_pFindAddr;
				bool operator < (const sMemPage& Memobj) const
				{
					return  m_pFindAddr < Memobj.m_pBuf;
				}
			};
 
			CMemPageCache(CommonLib::alloc_t *pAlloc);
			~CMemPageCache();

			virtual void* alloc(size_t size);
			virtual void  free(void* buf);

			void* getMemPage();
			void  clear();
			void init(size_t nPageSize, size_t nPagePoolSize);
		private:
		

			typedef std::vector<sMemPage> TMemPages;
			typedef TSimpleStack<int> TFreeMemPages;
			CommonLib::alloc_t *m_pAlloc;
			CommonLib::simple_alloc_t m_simple_alloc;
			
			size_t m_nPagePoolSize;
			size_t m_nPageSize;
			TMemPages m_MemPages;
			TFreeMemPages m_FreePages;

			bool m_nInit;
	};
}

#endif