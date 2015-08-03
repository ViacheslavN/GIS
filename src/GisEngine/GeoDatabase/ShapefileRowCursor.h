#ifndef GIS_GEODATABASE_SHAPEFILEROWCURSOR_H
#define GIS_GEODATABASE_SHAPEFILEROWCURSOR_H

#include "GeoDatabase.h"
#include "ShapefileUtils.h"

namespace GisEngine
{
	namespace GeoDatabase
	{
		class CShapefileFeatureClass;
		class CShapefileRowCursor : ICursor
		{
		public:
			CShapefileRowCursor(IQueryFilter* filter, bool recycling, IFeatureClass* fclass);
			virtual ~CShapefileRowCursor();

		public:
			// IRowCursor
			virtual bool NextRow(IRowPtr* row);

		

		protected:


			bool NextRowEx(IRowPtr* row, IRow* rowCache);

		protected:

		
		};
	}
}

#endif