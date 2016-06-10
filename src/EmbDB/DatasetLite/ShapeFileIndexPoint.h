#ifndef _DATASET_LITE_I_SHAPE_FILE_INDEX_POINT_H_
#define _DATASET_LITE_I_SHAPE_FILE_INDEX_POINT_H_

#include "SpatialDataset.h"
#include "ShapeIndexBase.h"

namespace DatasetLite
{

	class CShapeFileIndexPoint : public TShapeIndexBase<IShapeFileIndexPoint>
	{			
	public:
		typedef TShapeIndexBase<IShapeFileIndexPoint> TBase;
		CShapeFileIndexPoint(CommonLib::alloc_t* pAlloc, uint32 nPageSize, const CommonLib::bbox& bbox, double dOffsetX, double dOffsetY, byte nScaleX, 
			byte nScaleY, GisEngine::GisCommon::Units units, embDB::eSpatialType type, int nShapeType );
		CShapeFileIndexPoint(CommonLib::alloc_t* pAlloc = NULL);
		~CShapeFileIndexPoint();
		bool Open(const CommonLib::CString& sDbName);
		bool Create(const CommonLib::CString& sDbName);
		virtual bool commit();
		virtual bool insert(double dX, double dY, int nShapeId);
	private:

	};

}


#endif