#ifndef GIS_ENGINE_CLIP_RECT_ALLOC_H_
#define GIS_ENGINE_CLIP_RECT_ALLOC_H_

#include "CommonLibrary/alloc_t.h"
#include "CommonLibrary/PodVector.h"
namespace GisEngine
{
	namespace Display
	{
		class CClipRectAlloc
		{
			typedef CommonLib::TPodVector<uint32> TVecParts;
			typedef CommonLib::TPodVector<GPoint> TVecGPoints;
		public:
			CClipRectAlloc(CommonLib::alloc_t *pAlloc);

			uint32 *getPartsBuf(bool bIn);
			GPoint* getPointBuf(bool bIb);

		private:
			CommonLib::alloc_t *m_pAlloc;

			TVecParts m_vecInParts;
			TVecParts m_vecOutParts;
			TVecGPoints m_vecInPoints;
			TVecGPoints m_vecOutPoints;

		};
	}
}
#endif