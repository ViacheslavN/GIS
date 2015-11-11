#include "stdafx.h"
#include "SQLiteTransaction.h"
#include "SQLiteDB.h"
#include "SQLiteInsertCursor.h"


namespace GisEngine
{
	namespace GeoDatabase
	{
		CSQLiteTransaction::CSQLiteTransaction(SQLiteUtils::CSQLiteDB *pDB) : m_pDB(pDB), m_bCommit(false), 
			m_bBegin(false), m_bEnd(false)
		{

		}
		CSQLiteTransaction::~CSQLiteTransaction()
		{
			if(!m_bEnd)
			{
				m_pDB->execute(L"ROLLBACK");
			}
		}
		bool CSQLiteTransaction::begin()
		{
			if(m_bBegin)
				return false;

			m_bBegin =  m_pDB->execute(L"BEGIN IMMEDIATE TRANSACTION");
			return m_bBegin;
		}
		bool CSQLiteTransaction::commit()
		{
			if(!m_bBegin)
				return false;
			bool bRet = m_pDB->execute(L"COMMIT");

			m_bEnd = true;

			return bRet;
		}
		bool CSQLiteTransaction::rollback()
		{
			if(!m_bBegin)
				return false;

			m_bEnd = true;
			return m_pDB->execute(L"ROLLBACK");
		}
		void CSQLiteTransaction::GetError(CommonLib::CString& sText)
		{
			sText = m_pDB->GetError();
		}

		IInsertCursorPtr CSQLiteTransaction::CreateInsertCusor(ITable *pTable, IFieldSet *pFileds)
		{
			return IInsertCursorPtr(new SQLiteInsertCursor(pTable, pFileds, m_pDB));
		}
		IUpdateCursorPtr CSQLiteTransaction::CreateUpdateCusor(ITable *pTable, IFieldSet *pFileds)
		{
			return IUpdateCursorPtr();
		}

		IDeleteCursorPtr CSQLiteTransaction::CreateDeleteCusor(ITable *pTable, IFieldSet *pFileds)
		{
			return IDeleteCursorPtr();
		}
	}
}