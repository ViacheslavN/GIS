#ifndef _DATASET_LITE_I_SHAPE_FILE_INDEX_RECT_H_
#define _DATASET_LITE_I_SHAPE_FILE_INDEX_RECT_H_

#include "SpatialDataset.h"
#include "ShapeIndexBase.h"

namespace DatasetLite
{
	
	class CShapeFileIndexRect : public TShapeIndexBase<IShapeFileIndexRect>
	{			
	public:
		typedef TShapeIndexBase<IShapeFileIndexRect> TBase;
		CShapeFileIndexRect(CommonLib::alloc_t* pAlloc = NULL);
		~CShapeFileIndexRect();
		bool Open(const CommonLib::CString& sDbName);
		bool Create(const CommonLib::CString& sDbName, size_t nPageSize, const CommonLib::CString& sShapeFileName);
	private:
		
	};

}


#endif