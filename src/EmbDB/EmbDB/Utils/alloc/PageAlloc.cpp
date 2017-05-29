#include "stdafx.h"
#include "PageAlloc.h"


namespace embDB
{
	CPageAlloc::CPageAlloc(CommonLib::alloc_t *pAlloc, uint32 nMemPageSize, uint32 nBeginPageCnt) :
		m_pAlloc(pAlloc), m_nMemPageSize(nMemPageSize), m_pCurrPage(0)
	{
		if(!m_pAlloc)
			m_pAlloc = &m_alloc;

		if(nBeginPageCnt == 0)
			nBeginPageCnt = 1;

		for (uint32 i = 0; i < nBeginPageCnt; ++i)
		{
			SMemPage* memPage = new  SMemPage();
			memPage->m_pBuf = m_pAlloc->alloc(m_nMemPageSize);
			if(i != 0)
				m_listFreePage.push_back(memPage);
			else
				m_pCurrPage = memPage;
	  		m_mapPage.insert(std::make_pair(memPage->m_pBuf, memPage));
		}
		

	}
	CPageAlloc::~CPageAlloc()
	{
		for (std::map<void*, SMemPage*>::iterator it = m_mapPage.begin(); it != m_mapPage.end(); ++it)
		{
			SMemPage* pPage = it->second;
			assert(pPage->m_nCnt == 0);
			m_pAlloc->free(pPage->m_pBuf);
			delete pPage;

		}
		m_mapPage.clear();
	}

	void* CPageAlloc::alloc(uint32 size)
	{
 
		if(size >= m_nMemPageSize)
			return m_pAlloc->alloc(size);
		
 
		int32 nFreeMem = m_nMemPageSize - (m_pCurrPage->m_nPos /*+ sizeof(uint32)*/);
		if((int32)size < nFreeMem && nFreeMem)
		{
			  return allocFromPage(m_pCurrPage, size);
		}
		else
		{
			if(m_listFreePage.empty())
			{
				SMemPage* memPage = new  SMemPage();
				memPage->m_pBuf = m_pAlloc->alloc(m_nMemPageSize);
				m_mapPage.insert(std::make_pair(memPage->m_pBuf , memPage));
				m_pCurrPage = memPage;
			}
			else
			{
				m_pCurrPage= m_listFreePage.front();
				m_listFreePage.pop_front();
			}
 
			return allocFromPage(m_pCurrPage, size);
		}
	}

	void* CPageAlloc::allocFromPage(SMemPage* page, uint32 nSize)
	{
		assert((page->m_nPos /*+sizeof(uint32)*/ + nSize) < m_nMemPageSize);
		/*memcpy((byte*)page->m_pBuf + page->m_nPos, &m_nCurrPage, sizeof(uint32));
		byte* pBuf = (byte*)page->m_pBuf + page->m_nPos +  sizeof(uint32);
		page->m_nPos += sizeof(uint32) + nSize;
		page->m_nCnt +=1;

		assert(page->m_nPos < m_nMemPageSize);*/
		byte* pBuf = (byte*)page->m_pBuf + page->m_nPos;
		page->m_nPos += sizeof(uint32) + nSize;
		page->m_nCnt +=1;
		return pBuf;

	}
	void  CPageAlloc::free(void* ptr)
	{
 
		/*uint32 nPageID = 0;
		byte* pBuf = (byte*)ptr;
		pBuf -= sizeof(uint32);
		memcpy(&nPageID, pBuf, sizeof(uint32));
		assert(nPageID < m_mapPage.size());*/

		std::map<void*, SMemPage*>::iterator it = m_mapPage.lower_bound(ptr);
		if(it == m_mapPage.end())
		{
			SMemPage* pPage = m_mapPage.rbegin()->second;
			if((uint64)ptr < ((uint64)pPage->m_pBuf + m_nMemPageSize))
			{
				free(pPage, ptr);
				return;
			}
			m_pAlloc->free(ptr);
			return;
		}

		

		SMemPage* pPage = it->second;

		if(ptr < pPage->m_pBuf)
		{
			if(it == m_mapPage.begin())
			{
				m_pAlloc->free(ptr);
				return;
			}
			--it;
			pPage = it->second;
		}

		free(pPage, ptr);

	}

	void CPageAlloc::free(SMemPage*pPage, void *ptr)
	{
		assert(pPage->m_nCnt != 0);
		pPage->m_nCnt -= 1;

		if(pPage->m_nCnt == 0)
		{
			pPage->m_nPos = 0;
			if(pPage != m_pCurrPage)
			{
				m_listFreePage.push_back(pPage);
			}
		}
	}
}
