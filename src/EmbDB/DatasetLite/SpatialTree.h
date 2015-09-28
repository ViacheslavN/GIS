#ifndef _DATASET_LITE_I_SPATIAL_TREE_H_
#define _DATASET_LITE_I_SPATIAL_TREE_H_

#include "SpatialDataset.h"


namespace DatasetLite
{
	class IStatialTree
	{
		public:
			IStatialTree(){}
			virtual ~IStatialTree(){}

			virtual embDB::eDataTypes GetType() = 0;
			virtual bool insert(ShapeLib::SHPObject* pObject);
			IShapeCursorPtr search(CommonLib::bbox& extent);

	};

}


#endif