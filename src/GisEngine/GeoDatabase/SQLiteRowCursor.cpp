#include "stdafx.h"
#include "FieldSet.h"
#include "SQLiteRowCursor.h"

 
#include "Feature.h"
#include "CommonLibrary/MemoryStream.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "GisGeometry/Envelope.h"
#include "SQLiteFeatureClass.h"

namespace GisEngine
{
	namespace GeoDatabase
	{
		CSQLiteRowCursor::CSQLiteRowCursor(IQueryFilter* pFilter, bool bRecycling, ITable *pTable, SQLiteUtils::CSQLiteDB* pDB) :
			TBase(pFilter, bRecycling, pTable), m_pDB(pDB)
		{
			CommonLib::CString sSQL = "SELECT ";

			for (size_t i = 0, sz = m_vecFieldInfo.size(); i < sz; ++i)
			{
				IFieldPtr pField =  m_pSourceFields->GetField(m_vecFieldInfo[i].m_nDataSetIndex);
				assert(pField.get());
				if(i != 0)
					sSQL += L", ";
				sSQL += pField->GetName();
			}

			sSQL += L" from " + pTable->GetDatasetName();


			CommonLib::CString sWhere;
			CommonLib::CString sSpatialWhere;

		 
			
			if(m_pExtentSource.get())
			{
				CSQLiteFeatureClass *pFeatureClass = (CSQLiteFeatureClass *)pTable;

				assert(pFeatureClass);
				CommonLib::CString sSpatialOp;
				GisBoundingBox bbox = m_pExtentSource->GetBoundingBox();
		 		sSpatialWhere.format(L" %s IN (SELECT feature_id from %s where  ",
						pTable->HasOIDField() ? pTable->GetOIDFieldName().cwstr() : L"rowid", pFeatureClass->GetRTReeIndexName().cwstr());
		
				if(m_spatialRel == srlUndefined || m_spatialRel == srlIntersects)
				{
					sSpatialOp.format(L" maxX>=%f AND minX<=%f AND maxY>=%f AND minY<=%f)",
						bbox.xMin, bbox.xMax, bbox.yMin, bbox.yMax);
				}
				else
				{
					sSpatialOp.format(L" minX>=%f AND maxX<=%f AND minY>=%f AND maxY<=%f)",
						bbox.xMin, bbox.xMax, bbox.yMin, bbox.yMax);
				}
			
				sSpatialWhere += sSpatialOp;


			//	sSpatialWhere.format(L" %s IN (SELECT feature_id from %s)",
			//		pTable->HasOIDField() ? pTable->GetOIDFieldName().cwstr() : L"rowid", pFeatureClass->GetRTReeIndexName().cwstr());


			
			}

			if(!sSpatialWhere.isEmpty() || !pFilter->GetWhereClause().isEmpty() || !m_vecOids.empty())
			{
				sWhere += L" WHERE ";
				if(!sSpatialWhere.isEmpty())
					sWhere += sSpatialWhere;

				if(!pFilter->GetWhereClause().isEmpty())
				{
					if(!sWhere.isEmpty())
					{
						sWhere += L" AND ";
					}
					sWhere += pFilter->GetWhereClause();
				}
				if(!m_vecOids.empty())
				{
					if(!sWhere.isEmpty())
						sWhere += L" AND ";

					sWhere.format(L" %s IN (",
						pTable->HasOIDField() ? pTable->GetOIDFieldName().cwstr() : L"rowid");

					CommonLib::CString sOIDs;
					for (size_t i = 0, sz = m_vecOids.size();  i < sz; ++i)
					{
						if(i != 0)
							sWhere +=L",";
						sWhere += sOIDs.format(L" %I64d", m_vecOids[i]);
					}

					sWhere += L")";
				}
			}

			CSQLiteFeatureClass *pFeatureClass = dynamic_cast<CSQLiteFeatureClass *>(pTable);
			if(pFeatureClass)
			{
				GisGeometry::IEnvelopePtr pEnvelope =  pFeatureClass->GetExtent();
				m_comp_params = pEnvelope->GetCompressParams();
			}

			sSQL += sWhere;
			m_pStmt = pDB->prepare_query(sSQL);
		}

		CSQLiteRowCursor::CSQLiteRowCursor(int64 nOId, IFieldSet *pFieldSet, ITable* pTable, SQLiteUtils::CSQLiteDB* pDB) :
			TBase(nOId, pFieldSet, pTable), m_pDB(pDB)
		{
			CommonLib::CString sSQL = "SELECT ";

			for (size_t i = 0, sz = m_vecActualFieldsIndexes.size(); i < sz; ++i)
			{
				IFieldPtr pField =  m_pSourceFields->GetField(m_vecActualFieldsIndexes[i]);
				assert(pField.get());
				if(i != 0)
					sSQL += L", ";
				sSQL += pField->GetName();
			}

			sSQL += L" from " + pTable->GetDatasetName();
			CommonLib::CString sWhere;

			sWhere.format(L" where %s = %I64d", pTable->HasOIDField() ? pTable->GetOIDFieldName().cwstr() : L"rowid", nOId);
			sSQL+= sWhere;

			CSQLiteFeatureClass *pFeatureClass = dynamic_cast<CSQLiteFeatureClass *>(pTable);
			if(pFeatureClass)
			{
				GisGeometry::IEnvelopePtr pEnvelope =  pFeatureClass->GetExtent();
				m_comp_params = pEnvelope->GetCompressParams();
			}

			m_pStmt = pDB->prepare_query(sSQL);
		}
		CSQLiteRowCursor::~CSQLiteRowCursor()
		{

		}

		// IRowCursor
		bool CSQLiteRowCursor::NextRow(IRowPtr* pRow)
		{
			if(!m_pStmt.get())
				return false;
			if(!m_pStmt->StepNext())
				return false;

			if(!m_pCurrentRow.get())
			{
				m_pCurrentRow = new  CFeature(m_pFieldSet.get(), m_pSourceFields.get());
				if(m_nShapeFieldIndex >= 0 /*&& IsFieldSelected(m_nShapeFieldIndex)*/)
				{
					IFeature* feature = (IFeature*)(m_pCurrentRow.get());
					if(feature)
					{
						m_pCacheShape = new CommonLib::CGeoShape();
						feature->SetShape(m_pCacheShape.get());
					}
				}
			}


			for(size_t i = 0, sz = m_vecFieldInfo.size(); i < sz ;++i)
			{
				const sFieldInfo& fi = m_vecFieldInfo[i];
			 
			 
				CommonLib::CVariant* pValue = m_pCurrentRow->GetValue(fi.m_nRowIndex);
				 switch(fi.m_nType)
				 {
				
					case dtBlob:
						{
							CommonLib::IBlobPtr pBlob;
							auto it = m_mapCacheBlob.find(fi.m_nRowIndex);
							if(it == m_mapCacheBlob.end())
							{
								pBlob = new CommonLib::CBlob();
								m_mapCacheBlob.insert(std::make_pair(fi.m_nRowIndex, pBlob));
							}
							else
								pBlob = it->second;

							m_pStmt->ColumnBlob(fi.m_nRowIndex, pBlob.get());
							*pValue = CommonLib::IRefObjectPtr((IRefCnt*)pBlob.get());
						}
						break;
					case dtGeometry:
						{
							m_pStmt->ColumnShape(fi.m_nRowIndex, m_pCacheShape.get(), &m_comp_params);
						}
						break;
					case dtUInteger8:
						*pValue = (byte)m_pStmt->ColumnInt(fi.m_nRowIndex);//TO DO fix
						break;
					case dtInteger8:
						*pValue = (int8)m_pStmt->ColumnInt(fi.m_nRowIndex);
						break;
					case dtUInteger16:
						*pValue = (uint16)m_pStmt->ColumnInt(fi.m_nRowIndex);//TO DO fix
						break;
					case dtInteger32:
						*pValue = (int32)m_pStmt->ColumnInt(fi.m_nRowIndex);
						break;
					case dtUInteger32:
						*pValue = (uint32)m_pStmt->ColumnInt(fi.m_nRowIndex);
						break;
					case dtInteger16:
						*pValue = (int16)m_pStmt->ColumnInt(fi.m_nRowIndex);
						break;
					case dtUInteger64:
						*pValue = (uint64)m_pStmt->ColumnInt64(fi.m_nRowIndex);
						break;
					case dtInteger64:
						*pValue = (int64)m_pStmt->ColumnInt64(fi.m_nRowIndex); 
						break;
					case dtString:
					case dtAnnotation:
						{
							CommonLib::CString str = m_pStmt->ColumnText(fi.m_nRowIndex); //TO DO set cache
							*pValue = str;
						}
						break;
					case dtOid32:
						*pValue =  m_pStmt->ColumnInt(fi.m_nRowIndex);//TO DO fix
						break;
					case dtOid64:
						*pValue =  (int64)m_pStmt->ColumnInt64(fi.m_nRowIndex);
						break;
				 }

			}
			*pRow = m_pCurrentRow;
			return true;
		}
	}
}