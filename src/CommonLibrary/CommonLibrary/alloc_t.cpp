#include "stdafx.h"
#include "alloc_t.h"
#include "general.h"

static const size_t nMemSymbol = 12345;

#define MAX_ALLOC_SIZE  1024*1024*50 
#define CHECK_MAX_ALLOC_SIZE  


namespace CommonLib
{

simple_alloc_t::~simple_alloc_t()
{

}


simple_alloc_t::simple_alloc_t() 
#ifdef _DEBUG
	: m_nTotalBalanceAllocMemory(0), m_nCnt(0),  m_nAllCnt(0),  m_nFreeCnt(0),  m_nTotalAlloc(0)
	,  m_nTotalFree(0), m_nSizeMax(0)
#endif
{

}

void* simple_alloc_t::alloc(uint32 size)
{
#ifdef _DEBUG

#ifdef CHECK_MAX_ALLOC_SIZE
	if(size > MAX_ALLOC_SIZE)
	{
		assert(false);
		return NULL;
	}
#endif


#ifdef _CHECK_MEM
	byte* pBuf =  (byte*)::malloc(size + 3 *sizeof(uint32));
#else
	byte* pBuf =  (byte*)::malloc(size + sizeof(uint32));
#endif
	assert(pBuf);
	m_nTotalBalanceAllocMemory += size;
	m_nTotalAlloc += size;
	m_nCnt++;
	m_nAllCnt++;
	if(m_nSizeMax < size)
		m_nSizeMax = size;
#ifdef _CHECK_MEM
	  memcpy(pBuf, &nMemSymbol, sizeof(uint32));
	  memcpy(pBuf + sizeof(uint32), &size, sizeof(uint32));
	  size_t nSym = nMemSymbol + size;
	  memcpy(pBuf + 2 *sizeof(uint32) + size, &nSym, sizeof(uint32));
	  return pBuf + 2 * sizeof(uint32);
#else
	  memcpy(pBuf, &size, sizeof(uint32));
	  return pBuf + sizeof(uint32);
#endif
	

#else
	return ::malloc(size + sizeof(uint32));
#endif
}
void  simple_alloc_t::free(void* ptr)
{
	byte* pBuf = (byte*)ptr;
#ifdef _DEBUG
	uint32 size = 0;
#ifdef _CHECK_MEM
	uint32 nSymbol = 0;
	pBuf -= 2* sizeof(uint32);
	memcpy(&nSymbol, pBuf, sizeof(uint32));
	assert(nSymbol == nMemSymbol);
	memcpy(&size, pBuf +  sizeof(uint32), sizeof(uint32));
	assert(size > 0 && size <= m_nSizeMax);
	memcpy(&nSymbol, pBuf +  2* sizeof(uint32) + size, sizeof(uint32));
	assert((nSymbol - size) == nMemSymbol);
#else
	pBuf -= sizeof(uint32);
	memcpy(&size, pBuf, sizeof(uint32));
#endif
	m_nTotalBalanceAllocMemory -= size;
	m_nTotalFree += size;
	 m_nCnt--;
   m_nFreeCnt++;
  ::free(pBuf);
#else
	::free(pBuf);
#endif
}

}