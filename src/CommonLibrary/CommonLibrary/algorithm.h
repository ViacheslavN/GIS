#ifndef _LIB_COMMON_ALGORITHM_H_
#define _LIB_COMMON_ALGORITHM_H_
#include "general.h"
 

namespace CommonLib
{

	template<class TValue>
	int32 lower_bound(const TValue* pData, uint32 nSize, const TValue& key)
	{
		if(nSize == 0)
			return -1;

		int32 nFirst = 0;
		int32 nIndex = 0;
		int32 nStep = 0;
		int32 nCount = nSize;
		while (nCount > 0)
		{
			nIndex = nFirst; 
			nStep = nCount >> 1;
			nIndex += nStep;
			if(pData[ nIndex ] < key)
			{
				nFirst = ++nIndex;
				nCount -= (nStep + 1);
			} 
			else nCount = nStep;
		}
		return nFirst;
	}


	template<class TValue>
	int32 upper_bound(const TValue* pData, uint32 nSize, const TValue& key)  
	{

		if(nSize == 0)
			return -1;

		int32 nFirst = 0;
		int32 nIndex = 0;
		int32 nStep = 0;
		int32 nCount = nSize;
		while (nCount > 0)
		{
			nIndex = nFirst; 
			nStep = nCount >> 1;
			nIndex += nStep;
			if(!(key < pData[ nIndex ]))
			{
				nFirst = ++nIndex;
				nCount -= (nStep + 1);
			} 
			else nCount = nStep;
		}
		return nFirst;
	}

	template<class TValue >
	int32 binary_search(const TValue* pData, uint32 nSize, const TValue& key)
	{
		if(nSize == 0)
			return -1;

		int32 nIndex = lower_bound(pData, nSize, key);
		if(nIndex != nSize && key == pData[nIndex])
			return nIndex;
		return - 1;
	}
}

#endif