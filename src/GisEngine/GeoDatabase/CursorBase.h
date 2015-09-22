#ifndef GIS_ENGINE_GEO_DATABASE_CURSOR_BASE_H
#define GIS_ENGINE_GEO_DATABASE_CURSOR_BASE_H

#include "GeoDatabase.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
		template<class I>
		class ICursorBase : public I
		{
			public:

				ICursorBase(IQueryFilter* pFilter, bool recycling, ITable* pTable) :
					  m_bRecycling(recycling)
					, m_spatialRel(srlUndefined)
					, m_nOidFieldIndex(-1)
					, m_nShapeFieldIndex(-1)
					, m_nAnnoFieldIndex(-1)
					, m_bNeedTransform(false)

				{
					assert(pTable);
					assert(pFilter);

					m_pTable = pTable;
					m_pFilter = pFilter;
					m_pFieldSet = pFilter->GetFieldSet();

					if(!m_pFilter->GetWhereClause().isEmpty())
					{
						//TO DO set fields
					}
					m_pSourceFields = m_pTable->GetFields();
					UpdateFields();
				

					

					IOIDSetPtr oidSet = m_pFilter->GetOIDSet();
					oidSet->Reset();

					int oid;
					while(oidSet->Next(&oid))
						m_vecOids.push_back(oid);

					std::sort(m_vecOids.begin(), m_vecOids.end());

					m_RowIDIt = m_vecOids.begin();

					// Spatial queries

					
					IFeatureClass *pFeatureClass =  dynamic_cast<IFeatureClass *>(m_pTable.get());
			 
					if(pFeatureClass)
					{
						GisGeometry::ISpatialReferencePtr spatRefOutput(m_pFilter->GetOutputSpatialReference());
						GisGeometry::ISpatialReferencePtr spatRefSource(pFeatureClass->GetSpatialReference());

						ISpatialFilter *spatFilter = dynamic_cast<ISpatialFilter *>(m_pFilter.get());
						if(spatFilter)
						{
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

						}
					}
					
				

				}

				ICursorBase(int64 nOId, IFieldSet *pFieldSet, ITable* pTable) :
					m_bRecycling(false)
					, m_spatialRel(srlUndefined)
					, m_nOidFieldIndex(-1)
					, m_nShapeFieldIndex(-1)
					, m_nAnnoFieldIndex(-1)
					, m_bNeedTransform(false)
				{
					m_pTable = pTable;
					m_pSourceFields = m_pTable->GetFields();
					m_vecOids.push_back(nOId);
					m_pFieldSet = pFieldSet;
					UpdateFields();

				}
				virtual ~ICursorBase(){}
				virtual IFieldSetPtr GetFieldSet() const
				{
					return m_pFieldSet;
				}
				virtual IFieldsPtr   GetSourceFields() const
				{
					return m_pSourceFields;
				}
				virtual bool         IsFieldSelected(int index) const
				{
					if(index < (int)m_vecFieldsExists.size() && index > -1)
						return m_vecFieldsExists[index] == 1;

					return false;
				}

				void UpdateFields()
				{
				
					int fieldCount = m_pSourceFields->GetFieldCount();
					m_vecFieldsExists.resize(fieldCount, 0);
					m_vecActualFieldsIndexes.clear();
 

					m_nOidFieldIndex = -1;
					m_nShapeFieldIndex = -1;
					m_nAnnoFieldIndex = -1;

					if(!m_pFieldSet.get())
					{
						m_pFieldSet = new CFieldSet();
						for (int i = 0, sz = m_pSourceFields->GetFieldCount(); i < sz; ++i)
						{
							IFieldPtr pField = m_pSourceFields->GetField(i);
							m_pFieldSet->Add(pField->GetName());
						}
					}

					m_pFieldSet->Reset();
					CommonLib::CString field;
					while(m_pFieldSet->Next(&field))
					{
						if(field == L"*")
						{
					//		if(m_pFilter.get())
							{
								m_pFieldSet->Clear();
								for(int i = 0; i < fieldCount; ++i)
								{
									IFieldPtr field = m_pSourceFields->GetField(i);
									m_pFieldSet->Add(field->GetName());
								}

								m_pFieldSet->Reset();
								m_vecActualFieldsIndexes.clear();
								m_vecActualFieldsTypes.clear();
								continue;
							}
						
						}
						int fieldIndex = m_pSourceFields->FindField(field);
						m_vecFieldsExists[fieldIndex] = 1;
						m_vecActualFieldsIndexes.push_back(fieldIndex);
						m_vecActualFieldsTypes.push_back(m_pSourceFields->GetField(fieldIndex)->GetType());

						if((m_vecActualFieldsTypes.back() == dtOid32 || m_vecActualFieldsTypes.back() == dtOid64)  && m_nOidFieldIndex < 0)
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
					m_pFieldSet = fieldSet;
					//m_pFilter->SetFieldSet(m_pFieldSet.get());
				}

			protected:
				IQueryFilterPtr m_pFilter;
				IFieldsPtr      m_pSourceFields;
				IFieldSetPtr	m_pFieldSet;
				std::vector<int>           m_vecFieldsExists;
				std::vector<int>           m_vecActualFieldsIndexes;
				std::vector<eDataTypes>  m_vecActualFieldsTypes;
				ITablePtr m_pTable;
				IRowPtr   m_pCurrentRow;
				bool m_bRecycling;
				std::vector<int64>           m_vecOids;
				std::vector<int64>::iterator m_RowIDIt;
				int m_nOidFieldIndex;
				int m_nShapeFieldIndex;
				int m_nAnnoFieldIndex;
				

				GisGeometry::IEnvelopePtr  m_pExtentOutput;
				GisGeometry::IEnvelopePtr  m_pExtentSource;
				bool m_bNeedTransform;
				eSpatialRel				   m_spatialRel;
		};
	}
}

#endif