#ifndef _EMBEDDED_DATABASE_B_SPATIAL_INDEX_H_
#define _EMBEDDED_DATABASE_B_SPATIAL_INDEX_H_
#include "IDBField.h"
#include "CommonLibrary/BoundaryBox.h"

namespace embDB
{

	class SpatialIndexFiled : public IndexFiled
	{
	public:
		SpatialIndexFiled() {}
		virtual ~SpatialIndexFiled() {}
		virtual IndexIteratorPtr find(const CommonLib::bbox& bbox, SpatialQueryMode mode = sqmIntersect) = 0;
	};


}

#endif


 