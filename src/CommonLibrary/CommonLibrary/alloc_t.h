#ifndef _LIB_COMMON_LIBRARY_ALLOC_H_
#define _LIB_COMMON_LIBRARY_ALLOC_H_
#include "GeneralTypes.h"
namespace CommonLib
{

class alloc_t
{
public:
  virtual void* alloc(uint32 size) = 0;
  virtual void  free(void* buf) = 0;
public:
  virtual ~alloc_t(){}
};


class simple_alloc_t : public alloc_t
{
public:
  virtual void* alloc(uint32 size);
  virtual void  free(void* buf);
  ~simple_alloc_t();
   simple_alloc_t();
private:
#ifdef _DEBUG
	int64 m_nTotalBalanceAllocMemory;
	int64 m_nCnt;
	int m_nAllCnt;
	int m_nFreeCnt;
	int64 m_nTotalAlloc;
	int64 m_nTotalFree;
	size_t m_nSizeMax;
#endif
};

}
#endif