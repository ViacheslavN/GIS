#include "stdafx.h"
#include "ShapefileRowCursor.h"
#include "ShapefileFeatureClass.h"
#include "FieldSet.h"
#include "Feature.h"
#include "GisGeometry/Envelope.h"

namespace GisEngine
{
	namespace GeoDatabase
	{
		CShapefileRowCursor::CShapefileRowCursor(IQueryFilter* pFilter, bool recycling, CShapefileFeatureClass* pFClass) :
			TBase(pFilter, recycling, (ITable*)pFClass),
				m_pShp(pFClass->GetSHP())
			, m_pDbf(pFClass->GetDBF())
			, m_pCacheObject(NULL)
			, m_pParentFC(pFClass)
			, m_bInvalidCursor(false)
			, m_nCurrentRowID(-1)
			//, m_bRecycling(recycling)
		{
		
			//m_bInvalidCursor = !m_pParentFC->reload(false);

			m_pShapeIndex = pFClass->GetShapeIndex();
			if(m_pShapeIndex.get())
			{
				m_pCursor = m_pShapeIndex->spatialQuery(m_pExtentSource->GetBoundingBox());
				
			}
			else
			if(!m_bInvalidCursor)
			{
				m_nCurrentRowID = m_vecOids.size() ? (int32)*m_RowIDIt : 0;
				ShapeLib::SHPGetInfo(m_pShp->file, &m_nRecordCount, NULL, NULL, NULL);
			
			}
		}

		CShapefileRowCursor::~CShapefileRowCursor()
		{
			if(m_pCacheObject)
				ShapeLib::SHPDestroyObject(m_pCacheObject);

			//if(!m_bInvalidCursor)
			//	m_pParentFC->close();
		}

		bool CShapefileRowCursor::NextRowEx(IRowPtr* row, IRow* rowCache)
		{
			if(rowCache || !m_pCurrentRow || !m_bRecycling)
			{
				if(rowCache)
					m_pCurrentRow = rowCache;
				else
				{
					m_pCurrentRow = new  CFeature(m_pFieldSet.get(), m_pSourceFields.get());
				}
				if(m_nShapeFieldIndex >= 0/* && IsFieldSelected(m_nShapeFieldIndex)*/)
				{
					IFeature* feature = (IFeature*)(m_pCurrentRow.get());
					if(feature)
					{
						m_pCacheShape = new CommonLib::CGeoShape();
						feature->SetShape(m_pCacheShape.get());
					}
				}
			}
		

			if(m_pCursor.get())
			{	
				bool recordGood = false;
				while(!recordGood)
				{
					if(m_pCursor->IsEnd())
						return false;

					GisBoundingBox bbox = m_pCursor->extent();

					if (m_bNeedTransform)
						m_pExtentSource->GetSpatialReference()->Project(m_pExtentOutput->GetSpatialReference().get(), bbox);
					GisBoundingBox& boxOutput = m_pExtentOutput->GetBoundingBox();
					if((bbox.type & CommonLib::bbox_type_normal) && (bbox.type & CommonLib::bbox_type_normal))
					{
						if (bbox.xMin > boxOutput.xMax || bbox.xMax < boxOutput.xMin || 
							bbox.yMin > boxOutput.yMax || bbox.yMax < boxOutput.yMin)
						{

						}
					}

					m_nCurrentRowID = m_pCursor->row();



					FillRow(m_pCurrentRow.get());

					*row = m_pCurrentRow.get();

					if(rowCache)
					{
						m_pCacheShape.reset();
						m_pCurrentRow.reset();
					}

					m_pCursor->next();
					recordGood  = true;
				}

				return true;
			}
				

			if(m_bInvalidCursor)
				return false;

			bool recordGood = false;

			while(!recordGood)
			{
				if(EOC())
				{
					m_pCurrentRow.reset();
					row->reset();
					return false;
				}
				recordGood = FillRow(m_pCurrentRow.get());
				SimpleNext();
				
			}

			*row = m_pCurrentRow.get();

			if(rowCache)
			{
				m_pCacheShape.reset();
				m_pCurrentRow.reset();
			}

			return true;
		}


		bool CShapefileRowCursor::NextRow(IRowPtr* row)
		{
			return NextRowEx(row, 0);
		}


		bool CShapefileRowCursor::FillRow(IRow* row)
		{
			//for(int i = 0; i < (int)m_vecActualFieldsIndexes.size(); ++i)
			for(size_t i = 0, sz = m_vecFieldInfo.size(); i < sz ;++i)
			{
				const sFieldInfo& fi = m_vecFieldInfo[i];

				CommonLib::CVariant* pValue = row->GetValue(fi.m_nRowIndex);

				if(fi.m_nType == dtOid32) // OID
				{				
					*pValue = (int)m_nCurrentRowID;
					continue;
				}

				if(fi.m_nType == dtGeometry) // Shape
				{
					continue;
				}

				//int shpFieldIndex = fieldIndex/* - 2*/;
				CommonLib::CString strVal;
				double dblVal;
				int intVal;
		
				switch(fi.m_nType)
				{
					case dtString:
						strVal = ShapeLib::DBFReadStringAttribute(m_pDbf->file, m_nCurrentRowID, fi.m_nDataSetIndex);
						*pValue  = strVal;
						break;
					case dtInteger8:
					case dtInteger16:
					case dtInteger32:
					case dtUInteger8:
					case dtUInteger16:
					case dtUInteger32:
						intVal = ShapeLib::DBFReadIntegerAttribute(m_pDbf->file, m_nCurrentRowID, fi.m_nDataSetIndex);
						*pValue  = intVal;
						break;
					case dtDouble:
						dblVal = ShapeLib::DBFReadDoubleAttribute(m_pDbf->file, m_nCurrentRowID, fi.m_nDataSetIndex);
						*pValue  = dblVal;
						break;
					default:
						return false;
				}
			}

			if(m_nShapeFieldIndex >= 0) // Shape
			{
				m_pCacheObject = ShapeLib::SHPReadObject(m_pShp->file, m_nCurrentRowID);
				if(!m_pCacheShape.get())
					m_pCacheShape = new CommonLib::CGeoShape();
				ShapefileUtils::SHPObjectToGeometry(m_pCacheObject, *m_pCacheShape);
				if ( m_pCacheObject )
				{
					ShapeLib::SHPDestroyObject(m_pCacheObject);
					m_pCacheObject = 0;
				}
				// end bb changes
				if(!m_pCursor.get())
				{
					if(!AlterShape(m_pCacheShape.get()))
						return false;
				}
			

				if(IFeature *pFeature = (IFeature *)row)
				{
					//CommonLib::IGeoShapePtr pShape = pFeature->GetShape();
					pFeature->SetShape(m_pCacheShape.get());
				}
				else
				{
					CommonLib::CVariant* pShapeVar = row->GetValue(m_nShapeFieldIndex);
					*pShapeVar  = CommonLib::IRefObjectPtr(m_pCacheShape.get());
				}
			
			}

			return true;
		}

		 
		bool CShapefileRowCursor::AlterShape(CommonLib::CGeoShape* pShape) const
		{
			if(!pShape)
				return !(m_pExtentOutput->GetBoundingBox().type & CommonLib::bbox_type_normal);

			if (m_bNeedTransform&& pShape)
				m_pExtentSource->GetSpatialReference()->Project(m_pExtentOutput->GetSpatialReference().get(), pShape);
		

			GisBoundingBox& boxShape = pShape->getBB();
			GisBoundingBox& boxOutput = m_pExtentOutput->GetBoundingBox();
			if((boxShape.type & CommonLib::bbox_type_normal) && (boxOutput.type & CommonLib::bbox_type_normal))
			{
				if (boxShape.xMin > boxOutput.xMax || boxShape.xMax < boxOutput.xMin || 
					boxShape.yMin > boxOutput.yMax || boxShape.yMax < boxOutput.yMin)
				{
					return false;
				}
			}

			return true;
		}


		bool CShapefileRowCursor::EOC()
		{
			if(m_bInvalidCursor)
				return true;

			if(m_nCurrentRowID >= m_nRecordCount || (m_vecOids.size() > 0 && m_RowIDIt == m_vecOids.end()))
				return true;

			return false;
		}

		void CShapefileRowCursor::SimpleNext()
		{
			if(m_vecOids.size())
			{
				++m_RowIDIt;
				if(m_RowIDIt != m_vecOids.end())
					m_nCurrentRowID = (int32)*m_RowIDIt;
			}
			else
				++m_nCurrentRowID;
		}

	}
}