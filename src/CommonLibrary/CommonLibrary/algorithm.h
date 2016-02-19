#ifndef _EMBEDDED_DATABASE_ALGORITHM_H_
#define _EMBEDDED_DATABASE_ALGORITHM_H_
#include "general.h"
 

namespace CommonLib
{

	template<class TValue>
	int32 lower_bound(TValue* pData, uint32 nSize, const TValue& key)
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
	int32 upper_bound(TValue* pData, uint32 nSize, const TValue& key)  
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
}

#endif