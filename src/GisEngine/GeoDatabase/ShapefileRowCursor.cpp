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
		
			m_bInvalidCursor = !m_pParentFC->reload(false);

			/*m_pFilter = filter;

			if(!m_pFilter->GetWhereClause().isEmpty())
			{
				 //TO DO set fields
			}
			m_pSourceFields = m_pParentFC->GetFields();

			m_bInvalidCursor = !m_pParentFC->reload(false);
			int fieldCount = m_pSourceFields->GetFieldCount();
			m_vecFieldsExists.resize(fieldCount, 0);
			m_vecActualFieldsIndexes.clear();


			CommonLib::CString field;
			m_pFilter->GetFieldSet()->Reset();

			m_nOidFieldIndex = -1;
			m_nShapeFieldIndex = -1;
			m_nAnnoFieldIndex = -1;

			while(m_pFilter->GetFieldSet()->Next(&field))
			{
				if(field == L"*")
				{
					m_pFilter->GetFieldSet()->Clear();
					for(int i = 0; i < fieldCount; ++i)
					{
						IFieldPtr field = m_pSourceFields->GetField(i);
						m_pFilter->GetFieldSet()->Add(field->GetName());
					}

					m_pFilter->GetFieldSet()->Reset();
					m_vecActualFieldsIndexes.clear();
					m_vecActualFieldsTypes.clear();
					continue;
				}
				int fieldIndex = m_pSourceFields->FindField(field);
				m_vecFieldsExists[fieldIndex] = 1;
				m_vecActualFieldsIndexes.push_back(fieldIndex);
				m_vecActualFieldsTypes.push_back(m_pSourceFields->GetField(fieldIndex)->GetType());

				if(m_vecActualFieldsTypes.back() == dtOid && m_nOidFieldIndex < 0)
					m_nOidFieldIndex = fieldIndex;
				if(m_vecActualFieldsTypes.back() == dtGeometry && (m_nShapeFieldIndex < 0 || m_nShapeFieldIndex > fieldIndex))
					m_nShapeFieldIndex = fieldIndex;
				if(m_vecActualFieldsTypes.back() == dtAnnotation && (m_nAnnoFieldIndex < 0 || m_nAnnoFieldIndex > fieldIndex))
					m_nAnnoFieldIndex = fieldIndex;
			}

			// Change fieldset to right names (from DB)
			int actualfieldCount = (int)m_vecActualFieldsIndexes.size();
			IFieldSetPtr fieldSet(new CFieldSet());
			for(int i = 0; i < actualfieldCount; ++i)
				fieldSet->Add(m_pSourceFields->GetField(m_vecActualFieldsIndexes[i])->GetName());
			fieldSet->Reset();
			m_pFilter->SetFieldSet(fieldSet.get());

			IOIDSetPtr oidSet = m_pFilter->GetOIDSet();
			oidSet->Reset();

			int oid;
			while(oidSet->Next(&oid))
				m_vecOids.push_back(oid);

			std::sort(m_vecOids.begin(), m_vecOids.end());

			m_RowIDIt = m_vecOids.begin();

			// Spatial queries
		
				GisGeometry::ISpatialReferencePtr spatRefOutput(m_pFilter->GetOutputSpatialReference());
				GisGeometry::ISpatialReferencePtr spatRefSource(pFClass->GetSpatialReference());

				ISpatialFilter* spatFilter = (ISpatialFilter*)m_pFilter.get();
				m_spatialRel =  srlUndefined;
				if(spatFilter)
					m_spatialRel = spatFilter->GetSpatialRel();

				if(m_spatialRel != srlUndefined)
				{
					if(spatFilter->GetBB().type == CommonLib::bbox_type_normal)
					{
						m_pExtentOutput = new GisGeometry::CEnvelope(spatFilter->GetBB(), spatRefOutput.get());
						m_pExtentSource = new GisGeometry::CEnvelope(spatFilter->GetBB(), spatRefOutput.get());
						m_pExtentOutput->Project(spatRefOutput.get());
						m_pExtentSource->Project(spatRefSource.get());
					}
					else
					{
						CommonLib::IGeoShapePtr pShape(spatFilter->GetShape());
						m_pExtentOutput = new GisGeometry::CEnvelope(pShape->getBB(), spatRefOutput.get());
						m_pExtentSource = new GisGeometry::CEnvelope(pShape->getBB(), spatRefOutput.get());
						m_pExtentOutput->Project(spatRefOutput.get());
						m_pExtentSource->Project(spatRefSource.get());
					}
					
				}
				else
				{
					m_pExtentOutput = new GisGeometry::CEnvelope(GisBoundingBox(), spatRefOutput.get());
					m_pExtentSource = new GisGeometry::CEnvelope(GisBoundingBox(), spatRefSource.get());
				}

				m_bNeedTransform = spatRefOutput != NULL 
					&& spatRefSource != NULL 
					&& !spatRefOutput->IsEqual(spatRefSource.get());
				*/

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

			if(!m_bInvalidCursor)
				m_pParentFC->close();
		}

		bool CShapefileRowCursor::NextRowEx(IRowPtr* row, IRow* rowCache)
		{
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

				if(rowCache || !m_pCurrentRow || !m_bRecycling)
				{
					if(rowCache)
						m_pCurrentRow = rowCache;
					else
					{
						m_pCurrentRow = new  CFeature(m_pFieldSet.get(), m_pSourceFields.get());
					}
					if(m_nShapeFieldIndex >= 0 && IsFieldSelected(m_nShapeFieldIndex))
					{
						IFeature* feature = (IFeature*)(m_pCurrentRow.get());
						if(feature)
						{
							m_pCacheShape = new CommonLib::CGeoShape();
							feature->SetShape(m_pCacheShape.get());
						}
					}
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
			for(int i = 0; i < (int)m_vecActualFieldsIndexes.size(); ++i)
			{
				int fieldIndex = m_vecActualFieldsIndexes[i];

				CommonLib::CVariant* pValue = row->GetValue(fieldIndex);

				if(m_vecActualFieldsTypes[i] == dtOid32) // OID
				{				
					*pValue = (int)m_nCurrentRowID;
					continue;
				}

				if(m_vecActualFieldsTypes[i] == dtGeometry) // Shape
				{
					continue;
				}

				int shpFieldIndex = fieldIndex/* - 2*/;
				CommonLib::CString strVal;
				double dblVal;
				int intVal;
		
				switch(m_vecActualFieldsTypes[i])
				{
					case dtString:
						strVal = ShapeLib::DBFReadStringAttribute(m_pDbf->file, m_nCurrentRowID, shpFieldIndex);
						*pValue  = strVal;
						break;
					case dtInteger8:
					case dtInteger16:
					case dtInteger32:
					case dtUInteger8:
					case dtUInteger16:
					case dtUInteger32:
						intVal = ShapeLib::DBFReadIntegerAttribute(m_pDbf->file, m_nCurrentRowID, shpFieldIndex);
						*pValue  = intVal;
						break;
					case dtDouble:
						dblVal = ShapeLib::DBFReadDoubleAttribute(m_pDbf->file, m_nCurrentRowID, shpFieldIndex);
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

				if(!AlterShape(m_pCacheShape.get()))
					return false;

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
		

			GisBoundingBox boxShape = pShape->getBB();
			GisBoundingBox boxOutput = m_pExtentOutput->GetBoundingBox();
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