#ifndef GIS_ENGINE_CLIP_RECT_ALLOC_H_
#define GIS_ENGINE_CLIP_RECT_ALLOC_H_

#include "CommonLibrary/alloc_t.h"
#include "CommonLibrary/PodVector.h"
#include <map>
namespace GisEngine
{
	namespace Display
	{
		class CClipRectAlloc
		{
			typedef CommonLib::TPodVector<int> TVecParts;
			typedef CommonLib::TPodVector<GPoint> TVecGPoints;

			typedef std::map<uint32, TVecParts> TMapParts;
			typedef std::map<uint32, TVecGPoints> TMapPoints;
		public:
			CClipRectAlloc(CommonLib::alloc_t *pAlloc);
			~CClipRectAlloc();
			int *getPartsBuf(uint32 nCount, uint32 nSlot);
			GPoint* getPointBuf(uint32 nCount, uint32 nSlot);

		private:
			CommonLib::alloc_t *m_pAlloc;
			CommonLib::simple_alloc_t m_alloc;
	 


			TMapParts m_mapParts;
			TMapPoints m_mapPoints;
	

		};
	}
}
#endif