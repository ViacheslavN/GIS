#ifndef _DATASET_LITE_POINT_INDEX_H_
#define _DATASET_LITE_POINT_INDEX_H_
 
#include "SpatialDataset.h"
#include "../EmbDB/storage.h"

namespace DatasetLite
{
	struct IPointIndex
	{
		IPointIndex();
		virtual ~IPointIndex();

		virtual bool insert(double dX, double dY, int32 nRowID) = 0;
		virtual IShapeCursorPtr spatialQuery(const CommonLib::bbox& extent) = 0;

	};


	template<class _TSPTree, class _TCoordType, class _TStorage>
	class TPointIndex : public IPointIndex
	{
		public:
			typedef _TSPTree TSpatialTree;
			typedef _TCoordType TCoordType;
			typedef _TStorage   TStorage;

			TPointIndex(CommonLib::alloc_t * pAlloc, int32 nCacheBPTreeSize = 1000) : m_pAlloc(pAlloc), m_nCacheBPTreeSize(nCacheBPTreeSize)
			{}

			~TPointIndex()
			{}

			virtual bool insert(_TCoordType nX, _TCoordType nY, int32 nRowID)
			{
				if(!m_pSPTree.get())
					return false;

				return m_pSPTree->insert(nX, nY, nRowID);
			}
			IShapeCursorPtr spatialQuery(const CommonLib::bbox& extent)
			{
				return IShapeCursorPtr();
			}

			bool open(TStorage *pStorage, int64 nRootPage)
			{
				 m_pSPTree.reset(new TSpatialTree(nRootPage, pStorage, m_pAlloc, m_nCacheBPTreeSize))

			}
		
		private:
			int64 m_nTreeRootPage;
			int64 m_nCacheBPTreeSize;
			std::auto_ptr<TSpatialTree> m_pSPTree;
			CommonLib::alloc_t* m_pAlloc;
	};
}


#endif