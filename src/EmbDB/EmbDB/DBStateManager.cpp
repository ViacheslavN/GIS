#include "stdafx.h"
#include "DBStateManager.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "FilePage.h"
namespace embDB
{
	CDBStateManager::CDBStateManager()
	{

	}
	CDBStateManager::~CDBStateManager()
	{

	}
	bool CDBStateManager::Init(int64 nPageID, IDBStorage* pDBStorage, bool bRead)
	{
		m_pDBStorage = pDBStorage;
		m_nPageID = nPageID;
		if(bRead)
		{
			FilePagePtr pPage(m_pDBStorage->getFilePage(m_nPageID, true));
			if(!pPage.get())
				return false;
			CommonLib::FxMemoryReadStream stream;
			stream.attach(pPage->getRowData(), pPage->getPageSize());

		}
		
		return true;
	}
}