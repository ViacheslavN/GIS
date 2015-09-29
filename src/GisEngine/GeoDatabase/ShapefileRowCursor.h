#ifndef GIS_ENGINE_GEO_DATABASE_SHAPEFILEROWCURSOR_H
#define GIS_ENGINE_GEO_DATABASE_SHAPEFILEROWCURSOR_H

#include "GeoDatabase.h"
#include "ShapefileUtils.h"
#include "CursorBase.h"
#include "../../EmbDB/DatasetLite/SpatialDataset.h"

namespace GisEngine
{
	namespace GeoDatabase
	{
		class CShapefileFeatureClass;
		class CShapefileRowCursor : ICursorBase<ICursor>
		{
		public:

			typedef ICursorBase<ICursor> TBase;
			CShapefileRowCursor(IQueryFilter* filter, bool recycling, CShapefileFeatureClass* fclass);
			virtual ~CShapefileRowCursor();

		public:
			// IRowCursor
			virtual bool NextRow(IRowPtr* row);

			/*virtual IFieldSetPtr GetFieldSet() const {return IFieldSetPtr();}
			virtual IFieldsPtr   GetSourceFields() const  {return IFieldsPtr();}
			virtual bool         IsFieldSelected(int index) const  {return false;}*/

		protected:


			bool NextRowEx(IRowPtr* row, IRow* rowCache);
			bool EOC();
			bool FillRow(IRow* row);
			void SimpleNext();
			bool AlterShape(CommonLib::CGeoShape* pShape) const;
		protected:
			//IQueryFilterPtr m_pFilter;
			int m_nCurrentRowID;
			ShapefileUtils::SHPGuard* m_pShp;
			ShapefileUtils::DBFGuard* m_pDbf;
			/*std::vector<int>           m_vecOids;
			std::vector<int>::iterator m_RowIDIt;
			IFieldsPtr                 m_pSourceFields;
			std::vector<int>           m_vecFieldsExists;
			std::vector<int>           m_vecActualFieldsIndexes;
			std::vector<eDataTypes>  m_vecActualFieldsTypes;

			IRowPtr                    m_pCurrentRow;*/
		 	ShapeLib::SHPObject*      m_pCacheObject;
			CommonLib::IGeoShapePtr   m_pCacheShape;
			int m_nRecordCount;
			/*GisBoundingBox m_Extent;*/
			CShapefileFeatureClass* m_pParentFC;
			bool m_bInvalidCursor;
			DatasetLite::IShapeFileIndexPtr m_pShapeIndex;
			DatasetLite::IShapeCursorPtr m_pCursorPtr;
			//bool m_bRecycling;


			// Spatial Queries
			/*GisGeometry::IEnvelopePtr  m_pExtentOutput;
			GisGeometry::IEnvelopePtr  m_pExtentSource;
			bool m_bNeedTransform;

			int                        m_nOidFieldIndex;
			int                        m_nShapeFieldIndex;
			int                        m_nAnnoFieldIndex;
			eSpatialRel				   m_spatialRel;*/
		};
	}
}

#endif