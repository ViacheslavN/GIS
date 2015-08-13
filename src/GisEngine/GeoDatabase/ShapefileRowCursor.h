#ifndef GIS_ENGINE_GEO_DATABASE_SHAPEFILEROWCURSOR_H
#define GIS_ENGINE_GEO_DATABASE_SHAPEFILEROWCURSOR_H

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
			CShapefileRowCursor(IQueryFilter* filter, bool recycling, CShapefileFeatureClass* fclass);
			virtual ~CShapefileRowCursor();

		public:
			// IRowCursor
			virtual bool NextRow(IRowPtr* row);

		

		protected:


			bool NextRowEx(IRowPtr* row, IRow* rowCache);
			bool EOC();
			bool FillRow(IRow* row);
			void SimpleNext();
			bool AlterShape(CommonLib::CGeoShape* pShape) const;
		protected:
			IQueryFilterPtr filter_;
			int currentRowID_;
			ShapefileUtils::SHPGuard* shp_;
			ShapefileUtils::DBFGuard* dbf_;
			std::vector<int>           oids_;
			std::vector<int>::iterator rowIDIt_;
			IFieldsPtr                 sourceFields_;
			std::vector<int>           fieldsExists_;
			std::vector<int>           actualFieldsIndexes_;
			std::vector<eDataTypes>  actualFieldsTypes_;

			IRowPtr                    currentRow_;
		 	ShapeLib::SHPObject*      cacheObject_;
			CommonLib::IGeoShapePtr   cacheShape_;
			int recordCount_;
			GisBoundingBox extent_;
			CShapefileFeatureClass* parentFC_;
			bool invalidCursor_;
			bool recycling_;


			// Spatial Queries
			Geometry::IEnvelopePtr  extentOutput_;
			Geometry::IEnvelopePtr  extentSource_;
			bool needTransform_;

			int                        oidFieldIndex_;
			int                        shapeFieldIndex_;
			int                        annoFieldIndex_;
			eSpatialRel				   spatialRel_;
		};
	}
}

#endif