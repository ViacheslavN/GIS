#include "stdafx.h"
#include "RandomGenerator.h"
#include <time.h>
namespace embDB
{
	bool CRandomGenerator::GetRandomValues(byte *pData, uint32 nSize)
	{
		if(!nSize)
			return true;

		srand( time(0) );
		for (uint32 i = 0; i < nSize; ++i)
		{
			pData[i] = rand()%0xFF;
		}
		return true;
	}
}