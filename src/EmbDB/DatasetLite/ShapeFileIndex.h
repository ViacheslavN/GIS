#ifndef _DATASET_LITE_SHAPE_FILE_INDEX_H_
#define _DATASET_LITE_SHAPE_FILE_INDEX_H_
#include "SpatialDataset.h"

namespace DatasetLite
{
	class CShapeFileIndex : public IShapeFileIndex
	{
		public:
			const CommonLib::bbox& extent() const;
			IShapeCursorPtr spatialQuery(const CommonLib::bbox& extent);
			CShapeFileIndex();
			~CShapeFileIndex();
		private:

	};
}

#endif