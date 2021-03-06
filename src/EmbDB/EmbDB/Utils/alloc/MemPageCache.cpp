#include "stdafx.h"
#include "MemPageCache.h"

namespace embDB
{

	CMemPageCache::CMemPageCache(CommonLib::alloc_t *pAlloc) :
		m_nPageSize(0), m_pAlloc(pAlloc), m_nPagePoolSize(0), m_nInit(false)
	{
		if(!m_pAlloc)
			m_pAlloc = &m_simple_alloc;
	 
		 
	}
	CMemPageCache::~CMemPageCache()
	{
		clear();
	}
	void  CMemPageCache::clear()
	{
		for (uint32 i = 0, sz = (uint32)m_MemPages.size(); i < sz; ++i)
		{
			m_pAlloc->free(m_MemPages[i].m_pBuf);
		}
		m_MemPages.clear(); 
	}
	void* CMemPageCache::alloc(uint32 size)
	{
		//assert(size == m_nPageSize);
		if(size != m_nPageSize)
		{
			return m_pAlloc->alloc(size);
		}
		return getMemPage();
	}
	void* CMemPageCache::getMemPage()
	{
		if(!m_FreePages.empty())
		{
			int nIdx = m_FreePages.top();
			return m_MemPages[nIdx].m_pBuf;
		}
		return m_pAlloc->alloc(m_nPageSize);
	}

	void CMemPageCache::init(uint32 nPageSize, uint32 nPagePoolSize)
	{
		if(m_nInit)
			return;

		m_nPageSize = nPageSize;
		m_nPagePoolSize = nPagePoolSize;
		m_MemPages.resize(m_nPagePoolSize);
		m_FreePages.reserve(m_nPagePoolSize);
		for (uint32 i = 0, sz = (uint32)m_MemPages.size(); i < sz; ++i)
		{
			sMemPage& memPage = m_MemPages[i];
			memPage.m_pBuf = m_pAlloc->alloc(m_nPageSize);
			m_FreePages.push(i);
		}
		std::sort(m_MemPages.begin(), m_MemPages.end(), sMemPageSortCompare());
		m_nInit = true;
	}

	void  CMemPageCache::free(void* pBuf)
	{
		TMemPages::iterator it = std::lower_bound(m_MemPages.begin(), m_MemPages.end(), sMemPage(pBuf), sMemPageSortCompare());
		if(it != m_MemPages.end() && it->m_pBuf == pBuf)
		{
			m_FreePages.push(int(it - m_MemPages.begin()));
		}
		else
			m_pAlloc->free(pBuf);
	}
}