#ifndef _EMBEDDED_DATABASE_I_SPATIAL_QUERY_H_
#define _EMBEDDED_DATABASE_I_SPATIAL_QUERY_H_

#include "IQuery.h"

namespace embDB
{


	class ISpatialQuery
	{
	public:
		ISpatialQuery(){}
		virtual ~ISpatialQuery(){}
		virtual eSpatialCoordinatesType getType() const = 0;
		
		
	};
}
#endif