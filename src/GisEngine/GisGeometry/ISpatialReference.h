#ifndef GIS_ENGINE_GEOMETRY_I_SPATIAL_REFERENCE_H_
#define GIS_ENGINE_GEOMETRY_I_SPATIAL_REFERENCE_H_

#include "Common/Common.h"
#include "Common/Units.h"


namespace GisEngine
{
	namespace Geometry
	{
		class ISpatialReference
		{
			public:
				ISpatialReference(){};
				virtual ~ISpatialReference(){}

				virtual bool  IsValid() = 0;
				virtual void*  GetHandle() = 0;
				virtual bool Project(ISpatialReference* destSpatRef, CommonLib::CGeoShape* pShape) = 0;
				virtual bool Project(ISpatialReference* destSpatRef, GisBoundingBox& bbox) = 0;
				virtual bool Project(ISpatialReference *destSpatRef, GisXYPoint* pPoint) = 0;

				virtual const CommonLib::str_t& GetProjectionString() const = 0;
				virtual int   GetProjectionCode() const = 0;
				virtual bool  IsProjection() const = 0;

				virtual void save(CommonLib::IWriteStream *pStream) const = 0;
				virtual void load(CommonLib::IReadStream *pStream) = 0;
		};
	}
}

#endif