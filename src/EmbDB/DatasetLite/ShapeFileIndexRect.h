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
		CShapeFileIndexRect(CommonLib::alloc_t* pAlloc, uint32 nPageSize, const CommonLib::bbox& bbox, double dOffsetX, double dOffsetY, double dScaleX, 
			double dScaleY, GisEngine::GisCommon::Units units, embDB::eSpatialType type, int nShapeType );
		CShapeFileIndexRect(CommonLib::alloc_t* pAlloc = NULL);
		~CShapeFileIndexRect();
		bool Open(const CommonLib::CString& sDbName);
		bool Create(const CommonLib::CString& sDbName);
		virtual bool commit();
		virtual bool insert(const CommonLib::bbox& extent, int nShapeId);
	private:
		
	};

}


#endif