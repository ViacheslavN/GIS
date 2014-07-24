#include "stdafx.h"
#include "alloc_t.h"
#include "general.h"

static const size_t nMemSymbol = 12345;
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

void* simple_alloc_t::alloc(size_t size)
{
#ifdef _DEBUG
#ifdef _CHECK_MEM
	byte* pBuf =  (byte*)::malloc(size + 3 *sizeof(size_t));
#else
	byte* pBuf =  (byte*)::malloc(size + sizeof(size_t));
#endif
	assert(pBuf);
	m_nTotalBalanceAllocMemory += size;
	m_nTotalAlloc += size;
	m_nCnt++;
	m_nAllCnt++;
	if(m_nSizeMax < size)
		m_nSizeMax = size;
#ifdef _CHECK_MEM
	  memcpy(pBuf, &nMemSymbol, sizeof(size_t));
	  memcpy(pBuf + sizeof(size_t), &size, sizeof(size_t));
	  size_t nSym = nMemSymbol + size;
	  memcpy(pBuf + 2 *sizeof(size_t) + size, &nSym, sizeof(size_t));
	  return pBuf + 2 * sizeof(size_t);
#else
	  memcpy(pBuf, &size, sizeof(size_t));
	  return pBuf + sizeof(size_t);
#endif
	

#else
	return ::malloc(size + sizeof(size_t));
#endif
}
void  simple_alloc_t::free(void* ptr)
{
	byte* pBuf = (byte*)ptr;
#ifdef _DEBUG
	size_t size = 0;
#ifdef _CHECK_MEM
	size_t nSymbol = 0;
	pBuf -= 2* sizeof(size_t);
	memcpy(&nSymbol, pBuf, sizeof(size_t));
	assert(nSymbol == nMemSymbol);
	memcpy(&size, pBuf +  sizeof(size_t), sizeof(size_t));
	assert(size > 0 && size <= m_nSizeMax);
	memcpy(&nSymbol, pBuf +  2* sizeof(size_t) + size, sizeof(size_t));
	assert((nSymbol - size) == nMemSymbol);
#else
	pBuf -= sizeof(size_t);
	memcpy(&size, pBuf, sizeof(size_t));
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