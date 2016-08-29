#include "stdafx.h"
#include "EmbDBInsertCursor.h"
#include "FieldSet.h"
#include "EmbDBFeatureClass.h"
#include "../../EmbDB/EmbDB/FieldSet.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
		CEmbDBInsertCursor::CEmbDBInsertCursor(ITable* pTable, IFieldSet *pFileds,  embDB::IConnection *pConnection,
			embDB::ITransaction* pTran) :
				TBase(pTable, pFileds), m_bValidCursor(true), m_bInit(false)
		{
			m_pConnection = pConnection;
			m_pTran = pTran;
		}
		CEmbDBInsertCursor::~CEmbDBInsertCursor()
		{

		}
		void CEmbDBInsertCursor::init()
		{
			if(!m_pFieldSet.get())
			{
				m_pFieldSet = new CFieldSet();
				for (int i = 0, sz = m_pSourceFields->GetFieldCount(); i < sz; ++i)
				{
					IFieldPtr pField = m_pSourceFields->GetField(i);
					m_pFieldSet->Add(pField->GetName());
				}
			}


		
			embDB::IFieldSetPtr pFieldSet(new embDB::CFieldSet());
			for (int i = 0; i < m_pFieldSet->GetCount(); ++i)
			{

				const CommonLib::CString& sFieldName = m_pFieldSet->Get(i);
				IFieldPtr pField = m_pSourceFields->GetField(sFieldName);

				if(!pField.get())
				{
					//TO DO error
					m_bValidCursor = false;
				}
				pFieldSet->Add(sFieldName);
			}

			m_pCursor = m_pTran->createInsertCursor(m_pTable->GetDatasetName().cwstr(), pFieldSet.get());
			if(m_pCursor.get())
				m_pDBRow = m_pCursor->createRow();
		}
		int64 CEmbDBInsertCursor::InsertRow(IRow* pRow)
		{
			if(!m_bInit)
			{
				init();
				m_bInit = true;
			}
			if(!m_bValidCursor || !m_pCursor.get())
				return -1;


			for (size_t i = 0, sz = m_pFieldSet->GetCount(); i < sz; ++i)
			{
				m_pDBRow->set(*pRow->GetValue(i), i);
			}
			 
			return m_pCursor->insert(m_pDBRow.get());

		}

	}
}