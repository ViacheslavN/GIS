#include "stdafx.h"
#include "TranLogStateManager.h"
#include "TranStorage.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "embDBInternal.h"
namespace embDB
{
	CTranLogStateManager::CTranLogStateManager(CTranStorage *pStorage) : 
	m_pStorage(pStorage)
	,m_nPageAddr(-1)
	,m_nState(eTS_EMPTY)
	,m_nDbSize(0)
	{

	}
	CTranLogStateManager::~CTranLogStateManager()
	{}
	bool CTranLogStateManager::Init(int64 nPageAddr, bool bRead)
	{
		m_nPageAddr = nPageAddr;
		if(bRead)
		{
			CFilePage* pPage = m_pStorage->getFilePage(m_nPageAddr, COMMON_PAGE_SIZE);
			assert(pPage);
			if(!pPage)
				return false;
			CommonLib::FxMemoryReadStream stream;
			stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());
			stream.read(m_nState);
			stream.read(m_nDbSize);
			delete pPage; 
		}
		return true;
	}
	bool CTranLogStateManager::save()
	{
		CFilePage* pPage = m_pStorage->getFilePage(m_nPageAddr, false);
		assert(pPage);
		if(!pPage)
			return false;
		CommonLib::FxMemoryWriteStream stream;
		stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());
		stream.write(m_nState);
		stream.write(m_nDbSize);
		m_pStorage->saveFilePage(pPage, pPage->getAddr());
		delete pPage;
		return true;
	}
	bool  CTranLogStateManager::setState(eTransactionsState eTS)
	{
		m_nState = eTS;
		return true;
	}
	uint32  CTranLogStateManager::getState()
	{
		return m_nState;
	}
	void CTranLogStateManager::setDBSize(uint64 nSize)
	{
		m_nDbSize = nSize;
	}
};