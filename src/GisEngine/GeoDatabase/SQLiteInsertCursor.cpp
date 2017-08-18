#include "stdafx.h"
#include "SQLiteInsertCursor.h"

extern "C" {
#include "sqlite3/sqlite3.h"
}
#include "FieldSet.h"
#include "SQLiteUtils.h"
#include "SQLiteFeatureClass.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
		SQLiteInsertCursor::SQLiteInsertCursor(ITable* pTable, IFieldSet *pFileds, SQLiteUtils::CSQLiteDB *pDB) :
			TBase(pTable, pFileds), m_pDB(pDB), m_bValidCursor(true), m_bInit(false)
		{
			assert(m_pDB);


		}
		SQLiteInsertCursor::~SQLiteInsertCursor()
		{
	 
			close();
		}

		int64 SQLiteInsertCursor::InsertRow(IRow* pRow)
		{
			if(!m_bInit)
			{
				init();
				m_bInit = true;
			}
			if(!m_bValidCursor)
				return -1;
			IFeature *pFeature = NULL;
			//IFieldSetPtr pFieldSet = pRow->GetFieldSet();
			//if(!pFieldSet.get() || pFieldSet->GetCount() == 0)
			//	pFieldSet = m_pFieldSet;
			{
				//for (int i = 0; i < pFieldSet->GetCount(); ++i)
				for (int i = 0, sz = (int)m_vecTypes.size(); i < sz; ++i)
				
				{
					
					/*const CommonLib::CString& sName = pFieldSet->Get(i);
					TFieldsInfo::const_iterator c_it = m_mapFieldInfo.find(sName);
					if(c_it == m_mapFieldInfo.end())
					{
						//TO DO Error
						return -1;
					}*/

					if(m_vecTypes[i] == dtGeometry)
					{
						pFeature = (IFeature *)pRow;
						if(!pFeature)
						{
							//TO DO Error
							return -1;
						}
						
						CommonLib::IGeoShapePtr pShape = pFeature->GetShape();
						if(pShape.get())
						{
						 
							pShape->compress(&m_WriteShapeStream, &m_comp_params);
							m_pStmt->ColumnBindBlob(i + 1, m_WriteShapeStream.buffer(), m_WriteShapeStream.pos());
							m_WriteShapeStream.seek(0, CommonLib::soFromBegin);
						}
						

					}
					else
					{
						m_pStmt->ColumnBind(i + 1, m_vecTypes[i], pRow->GetValue(i));
						 
					}
					if(m_pStmt->IsError())
					{
						//TO DO error
						m_bValidCursor = false;
						return -1;

					}
				}

				m_pStmt->StepNext();
				if(m_pStmt->IsError())
				{
					//TO DO error
					m_bValidCursor = false;
					return -1;
				}

				m_pStmt->reset();
			}
			
			int64 nLastRowID = m_pDB->GetLastInsertRowID();
			if(pFeature)
			{
				IFieldPtr pOIDField = pRow->GetSourceFields()->GetField(m_pTable->GetOIDFieldName());

				int64 nOID = -1;
				if(m_pTable->HasOIDField())
					nOID = pRow->GetOID();
				if(nOID == -1)
					nOID = nLastRowID;

				if(pOIDField.get() && pOIDField->GetType() == dtOid32)
					m_pStmtSpatial->ColumnBindInt(1,  (int32)nOID);
				else
					m_pStmtSpatial->ColumnBindInt64(1,  nOID);
				if(m_pStmtSpatial->IsError())
				{
					//TO DO error
					m_bValidCursor = false;
					return -1;

				}

				CommonLib::IGeoShapePtr pShape = pFeature->GetShape();
				const CommonLib::bbox& bb = pShape->getBB();

				m_pStmtSpatial->ColumnBindDouble(2, bb.xMin);
				m_pStmtSpatial->ColumnBindDouble(3, bb.xMax);
				m_pStmtSpatial->ColumnBindDouble(4, bb.yMin);
				m_pStmtSpatial->ColumnBindDouble(5, bb.yMax);

				if(m_pStmtSpatial->IsError())
				{
					//TO DO error
					m_bValidCursor = false;
					return -1;

				}

				m_pStmtSpatial->StepNext();

				//if(m_pStmt)
				//	sqlite3_finalize(m_pStmt);
				if(m_pStmtSpatial->IsError())
				{
					//TO DO error
					m_bValidCursor = false;
					return -1;
				}

				m_pStmtSpatial->reset();

			}
			pRow->SetRowID(nLastRowID);
			return nLastRowID;
		}
		void SQLiteInsertCursor::close()
		{
		 
		}
		void SQLiteInsertCursor::init()
		{
		
			CommonLib::CString sSQL = L"INSERT INTO  " + m_pTable->GetDatasetName() + L" ( ";
			CommonLib::CString sValues = " VALUES ( ";


			if(!m_pFieldSet.get())
			{
				m_pFieldSet = new CFieldSet();
				for (int i = 0, sz = m_pSourceFields->GetFieldCount(); i < sz; ++i)
				{
					IFieldPtr pField = m_pSourceFields->GetField(i);
					m_pFieldSet->Add(pField->GetName());
				}
			}

			for (int i = 0; i < m_pFieldSet->GetCount(); ++i)
			{

				const CommonLib::CString& sFieldName = m_pFieldSet->Get(i);
				IFieldPtr pField = m_pSourceFields->GetField(sFieldName);

				if(!pField.get())
				{
					//TO DO error
					m_bValidCursor = false;
				}
				//m_mapFieldInfo.insert(std::make_pair(sFieldName, SFieldInfo(i, pField->GetType())));

				m_vecTypes.push_back(pField->GetType());
				if(i != 0)
				{
					sSQL += L", ";
					sValues += L", ";
				}

				sSQL += m_pFieldSet->Get(i);
				sValues += "?";
				
			}
			
			sValues  += L" )";
			sSQL += L" )";

			sSQL += sValues;

			m_pStmt = m_pDB->prepare_query(sSQL);
			if(!m_pStmt.get())
			{
				 //TO DO error
				m_bValidCursor = false;
				return;
			 
			}

			
			if(m_pTable->GetDatasetType() == dtFeatureClass)
			{
				CSQLiteFeatureClass *pFC = dynamic_cast<CSQLiteFeatureClass*>(m_pTable.get());
				assert(pFC);
				const CommonLib::CString& sRTreeIndex = pFC->GetRTReeIndexName();
				CommonLib::CString sSQL = L"INSERT INTO  " + sRTreeIndex + L"(feature_id, minX, maxX, minY, maxY)" + 
					" VALUES (?, ?, ?, ?, ?)";
				m_pStmtSpatial = m_pDB->prepare_query(sSQL);
				if(!m_pStmtSpatial.get())
				{
					m_bValidCursor = false;
				}
							
				GisGeometry::IEnvelopePtr pEnvelope =  pFC->GetExtent();
				m_comp_params = pEnvelope->GetCompressParams();
				
			}
			
		}

	}
}