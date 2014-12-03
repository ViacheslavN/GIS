#ifndef _EMBEDDED_DATABASE_PAGE_ALLOC_H_
#define _EMBEDDED_DATABASE_PAGE_ALLOC_H_

#include "Commonlibrary/general.h"
#include "Commonlibrary/alloc_t.h"

namespace embDB
{
	class CPageAlloc : public CommonLib::alloc_t
	{
		public:

			struct sMemPage
			{
				size_t m_nAddr;
				size_t m_nPos;
			};

			CPageAlloc(size_t nPageSize, size_t nPagePool);
			~CPageAlloc();

			virtual void* alloc(size_t size);
			virtual void  free(void* buf);
		private:
			CommonLib::alloc_t *m_pParent;
			CommonLib::simple_alloc_t m_simple_alloc;
			
	};
}

#endif