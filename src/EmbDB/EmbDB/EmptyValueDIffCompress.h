#ifndef _EMBEDDED_DATABASE_EMPTY_VALUE_DIFF_COMPRESS_H_
#define _EMBEDDED_DATABASE_EMPTY_VALUE_DIFF_COMPRESS_H_
#include "CompressorParams.h"
#include "EmptyValueCompress.h"
namespace embDB
{
	template <class _TValue>
	class TEmptyDiffValueCompress : public TEmptyValueCompress<_TValue>
	{
	public:

		typedef _TValue TValue;
		typedef TEmptyValueCompress<_TValue> TBase;
		TEmptyDiffValueCompress(CommonLib::alloc_t* pAlloc, uint32 nPageSize, CompressorParamsBaseImp *pParams) :
			TBase(pAlloc, nPageSize, pParams)
		{
			
		}
		~TEmptyDiffValueCompress()
		{

		}

		void AddDiffSymbol(const TValue& value)
		{
			this->m_nCount++;
		}
 
	};
}

#endif