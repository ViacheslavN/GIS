#include "stdafx.h"
#include "SQLiteInsertCursor.h"
#include "sqlite3/sqlite3.h"

#include "FieldSet.h"
#include "SQLiteUtils.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
		SQLiteInsertCursor::SQLiteInsertCursor(ITable* pTable, IFieldSet *pFileds, sqlite3* pConn) :
			TBase(pTable, pFileds), m_pConn(pConn), m_bValidCursor(true), m_bInit(false), m_pStmt(NULL)
		{
			assert(m_pConn);


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

			int nRetVal = 0;
			IFieldSetPtr pFieldSet = pRow->GetFieldSet();
			if(!pFieldSet.get() || pFieldSet->GetCount() == 0)
				pFieldSet = m_pFieldSet;
			{
				for (int i = 0; i < pFieldSet->GetCount(); ++i)
				{
					
					const CommonLib::CString& sName = pFieldSet->Get(i);
					TFieldsInfo::const_iterator c_it = m_mapFieldInfo.find(sName);
					if(c_it == m_mapFieldInfo.end())
					{
						//TO DO Error
						return -1;
					}

					if(c_it->second.m_type == dtAnnotation || c_it->second.m_type == dtGeometry)
					{
						IFeature *pFeature = (IFeature *)pRow;
						if(!pFeature)
						{
							//TO DO Error
							return -1;
						}
						
						CommonLib::IGeoShapePtr pShape = pFeature->GetShape();
						if(pShape.get())
						{
							CommonLib::MemoryStream stream;
							pShape->write(&stream);
							nRetVal = sqlite3_bind_blob(m_pStmt, i + 1, stream.buffer(), stream.size(), SQLITE_TRANSIENT);
						}
						

					}
					else
					{
						nRetVal = SQLiteUtils::BindToField(m_pStmt, c_it->second.m_nCol + 1, c_it->second.m_type, pRow->GetValue(i));
					}
					if(nRetVal != SQLITE_OK)
					{
						//TO DO error
						 m_sErrorMessage = CommonLib::CString(sqlite3_errmsg(m_pConn));
						m_bValidCursor = false;
						return -1;

					}
				}

				nRetVal = sqlite3_step(m_pStmt);

				//if(m_pStmt)
				//	sqlite3_finalize(m_pStmt);
				if(nRetVal != SQLITE_DONE)
				{
					//TO DO error
					m_sErrorMessage = CommonLib::CString(sqlite3_errmsg(m_pConn));
					m_bValidCursor = false;
					return -1;
				}

				sqlite3_reset(m_pStmt);
			}
			

			int64 nLastRowID = sqlite3_last_insert_rowid(m_pConn);
			return nLastRowID;
		}
		void SQLiteInsertCursor::close()
		{
			if(m_pStmt)
				sqlite3_finalize(m_pStmt);
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
				m_mapFieldInfo.insert(std::make_pair(sFieldName, SFieldInfo(i, pField->GetType())));

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

			int retValue = sqlite3_prepare(m_pConn, sSQL.cstr(), -1, &m_pStmt, 0);
			if(retValue != SQLITE_OK)
			{
				 //TO DO error
				m_bValidCursor = false;
			 
			}
		}

	}
}