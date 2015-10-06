#include "stdafx.h"
#include "PageAlloc.h"


namespace embDB
{
	CPageAlloc::CPageAlloc(CommonLib::alloc_t *pAlloc, uint32 nMemPageSize, uint32 nBeginPageCnt) :
		m_pAlloc(pAlloc), m_nMemPageSize(nMemPageSize), m_nCurrPage(0)
	{
		if(!m_pAlloc)
			m_pAlloc = &m_alloc;

		if(nBeginPageCnt == 0)
			nBeginPageCnt = 1;

		for (size_t i = 0; i < nBeginPageCnt; ++i)
		{
			SMemPage memPage;
			memPage.m_pBuf = m_pAlloc->alloc(m_nMemPageSize);
			if(i != 0)
				m_listFreePage.push_back(i);
			m_vecPage.push_back(memPage);
		}
		m_nCurrPage = 0;

	}
	CPageAlloc::~CPageAlloc()
	{
		for (size_t i = 0, sz = m_vecPage.size(); i < sz; ++i)
		{
			SMemPage& page = m_vecPage[i];
			assert(page.m_nCnt == 0);
			m_pAlloc->free(page.m_pBuf);
		}
		m_vecPage.clear();
	}

	void* CPageAlloc::alloc(size_t size)
	{
		if(size >= m_nMemPageSize)
			return NULL;

		SMemPage& page = m_vecPage[m_nCurrPage];
		int32 nFreeMem = m_nMemPageSize - (page.m_nPos + sizeof(uint32));
		if((int32)size < nFreeMem && nFreeMem)
		{
			  return allocFromPage(&page, size);
		}
		else
		{
			if(m_listFreePage.empty())
			{
				SMemPage memPage;
				memPage.m_pBuf = m_pAlloc->alloc(m_nMemPageSize);
				m_vecPage.push_back(memPage);
				m_nCurrPage = m_vecPage.size() -1;
			}
			else
			{
				m_nCurrPage = m_listFreePage.front();
				m_listFreePage.pop_back();
			}
			SMemPage& page = m_vecPage[m_nCurrPage];
			return allocFromPage(&page, size);
		}
	}

	void* CPageAlloc::allocFromPage(SMemPage* page, uint32 nSize)
	{
		assert((page->m_nPos +sizeof(uint32) + nSize) < m_nMemPageSize);
		memcpy((byte*)page->m_pBuf + page->m_nPos, &m_nCurrPage, sizeof(uint32));
		byte* pBuf = (byte*)page->m_pBuf + page->m_nPos +  sizeof(uint32);
		page->m_nPos += sizeof(uint32) + nSize;
		page->m_nCnt +=1;

		assert(page->m_nPos < m_nMemPageSize);
		return pBuf;

	}
	void  CPageAlloc::free(void* ptr)
	{
		uint32 nPageID = 0;
		byte* pBuf = (byte*)ptr;
		pBuf -= sizeof(size_t);
		memcpy(&nPageID, pBuf, sizeof(size_t));
		assert(nPageID < m_vecPage.size());

		SMemPage& page = m_vecPage[nPageID];
		page.m_nCnt -= 1;
		assert(page.m_nCnt >= 0);
		if(page.m_nCnt == 0)
		{
			page.m_nPos = 0;
			if(nPageID != m_nCurrPage)
				m_listFreePage.push_back(nPageID);
		}
	}
}
