#include "stdafx.h"
#include "EmbDBRowCursor.h"
#include "../../EmbDB/EmbDB/FieldSet.h"
#include "FieldSet.h"
#include "GisGeometry/Envelope.h"
#include "Feature.h"

namespace GisEngine
{
	namespace GeoDatabase
	{
 
		CEmbDBRowCursor::CEmbDBRowCursor(IQueryFilter* pFilter, bool bRecycling, ITable *pTable, embDB::IDatabase *pDB) :
			TBase(pFilter, bRecycling, pTable), m_pDB(pDB)
		{
			 embDB::IFieldSetPtr pEmbFieldSet(new embDB::CFieldSet());

			 for (size_t i = 0, sz = m_vecFieldInfo.size(); i < sz; ++i)
			 {
				 IFieldPtr pField =  m_pSourceFields->GetField(m_vecFieldInfo[i].m_nDataSetIndex);
				 assert(pField.get());
				 pEmbFieldSet->Add(pField->GetName());
			 }


			 m_pTran = m_pDB->startTransaction(embDB::eTT_SELECT);
			 m_pTran->begin();
			 if(m_pExtentSource.get())
			 {
				 IFieldPtr pShapeField = m_pSourceFields->GetField(m_nShapeFieldIndex);
				  assert(pShapeField.get());

				m_pCursor =  m_pTran->executeSpatialQuery(m_pExtentSource->GetBoundingBox(), m_pTable->GetDatasetName().cwstr(), pShapeField->GetName().cwstr(), embDB::sqmIntersect, pEmbFieldSet.get());
				m_bInvalidCursor = false;
			 }
			 else
				 m_bInvalidCursor = true;
		}
			CEmbDBRowCursor::CEmbDBRowCursor(int64 nOId, IFieldSet *pFieldSet, ITable* pTable, embDB::IDatabase *pDB):
					TBase(nOId, pFieldSet, pTable), m_pDB(pDB)
		{
			m_bInvalidCursor = true;
		}

		CEmbDBRowCursor::~CEmbDBRowCursor()
		{

		}

		bool CEmbDBRowCursor::NextRow(IRowPtr* row)
		{

			if(!m_pCursor.get())
				return false;

			if(m_bInvalidCursor)
				return false;

			m_bInvalidCursor = !m_pCursor->NextRow();


			if(m_bInvalidCursor)
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

				if(fi.m_nType == dtGeometry) 
				{
					IFeature* pFeature = (IFeature*)(m_pCurrentRow.get());
					CommonLib::CVariant varian;
					m_pCursor->value(&varian, fi.m_nRowIndex);
				
					CommonLib::IGeoShapePtr pShape =  varian.Get<CommonLib::IGeoShapePtr >();
					pFeature->SetShape(pShape.get());

				}
				else
				{
					CommonLib::CVariant* pValue = m_pCurrentRow->GetValue(fi.m_nRowIndex);
					m_pCursor->value(pValue, fi.m_nRowIndex);
				}

			
			}
			*row = m_pCurrentRow;
		
			return true;
		}
	}
}