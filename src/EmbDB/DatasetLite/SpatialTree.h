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
			virtual bool insert(ShapeLib::SHPObject* pObject) = 0;
			virtual IShapeCursorPtr search(const CommonLib::bbox& extent) = 0;
			virtual bool commit() = 0;
			virtual bool init() = 0;

	};

}


#endif