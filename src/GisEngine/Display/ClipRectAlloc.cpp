#include "stdafx.h"
#include "Point.h"
#include "ClipRectAlloc.h"

namespace GisEngine
{
	namespace Display
	{
		CClipRectAlloc::CClipRectAlloc(CommonLib::alloc_t *pAlloc) : m_pAlloc(pAlloc)
		{
			if(!m_pAlloc)
				m_pAlloc = &m_alloc;
		}
		CClipRectAlloc::~CClipRectAlloc()
		{

		}

		template< typename TVal, typename TMap>
		TVal* getBuf(uint32 nSlot, uint32 nCount, TMap& map)
		{
			typename TMap::iterator it = map.find(nSlot);
			if(it != map.end())
			{
				if(it->second.size() < nCount)
					it->second.reserve(nCount);
				return it->second.begin();
			}
			else
			{
				if(nCount < 8192)
					nCount = 8192;
				map[nSlot].reserve(nCount);
				return map[nSlot].begin();
			}
		}


		int* CClipRectAlloc::getPartsBuf(uint32 nCount, uint32 nSlot)
		{
		return getBuf<int, TMapParts>(nSlot, nCount, m_mapParts);
		 
		}
		GPoint* CClipRectAlloc::getPointBuf(uint32 nCount, uint32 nSlot)
		{			 
			return getBuf<GPoint, TMapPoints>(nSlot, nCount, m_mapPoints);
		}
	}
}