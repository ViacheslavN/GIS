#ifndef _DATASET_LITE_SHAPE_FILE_INDEX_H_
#define _DATASET_LITE_SHAPE_FILE_INDEX_H_
#include "SpatialDataset.h"
#include "SpatialTree.h"
namespace DatasetLite
{
	class CShapeFileIndexRect : public IShapeFileIndexRect
	{			
		public:
			CShapeFileIndexRect(const wchar_t *pszFileName);
			~CShapeFileIndexRect();
			const CommonLib::bbox& extent() const;
			IShapeCursorPtr spatialQuery(const CommonLib::bbox& extent);
	 	private:
			CommonLib::bbox m_extent;
			std::auto_ptr<IStatialTree> m_SpTree;
	};
}

#endif