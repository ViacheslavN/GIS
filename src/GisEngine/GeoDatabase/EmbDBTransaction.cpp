#include "stdafx.h"
#include "EmbDBTransaction.h"
#include "EmbDBInsertCursor.h"
#include "embDBUtils.h"

namespace GisEngine
{
	namespace GeoDatabase
	{

		CEmbDBTransaction::CEmbDBTransaction(embDB::IConnection* pConnection,  eTransactionType type) :
			m_bCommit(false)
			,m_bEnd(false)
			,m_bBegin(false)
		{
			m_pConnection = pConnection;
			m_pTran = m_pConnection->startTransaction(embDBUtils::TranType2EmbDbTranType(type));
		}
		CEmbDBTransaction::~CEmbDBTransaction()
		{
			if(!m_bEnd)
			{
				m_pTran->rollback();
			}
		}


		bool CEmbDBTransaction::begin()
		{
			if(m_bBegin)
				return false;

			m_bBegin =  m_pTran->begin();
			return m_bBegin;
		}

		bool CEmbDBTransaction::commit()
		{
			if(!m_bBegin)
				return false;
			bool bRet = m_pTran->commit();

			m_bEnd = true;

			return bRet;
		}
		bool CEmbDBTransaction::rollback()
		{
			if(!m_bBegin)
				return false;

			m_bEnd = true;
			return m_pTran->rollback();
		}
		void CEmbDBTransaction::GetError(CommonLib::CString& sText)
		{

		}

		IInsertCursorPtr  CEmbDBTransaction::CreateInsertCusor(ITable *pTable, IFieldSet *pFileds)
		{
			CEmbDBInsertCursor* pCursor = new  CEmbDBInsertCursor(pTable, pFileds, m_pConnection.get(), m_pTran.get());
			return IInsertCursorPtr(pCursor);
		}
		IUpdateCursorPtr  CEmbDBTransaction::CreateUpdateCusor(ITable *pTable, IFieldSet *pFileds)
		{
			return IUpdateCursorPtr();
		}

		IDeleteCursorPtr  CEmbDBTransaction::CreateDeleteCusor(ITable *pTable, IFieldSet *pFileds)
		{
			return IDeleteCursorPtr();
		}
		

		

	}
}