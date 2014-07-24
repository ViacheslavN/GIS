#include "stdafx.h"
#include "ListDBPage.h"
#include "storage.h"

namespace embDB
{
	/*ListDBPage::ListDBPage(_TStorage *pStorage, int64 nKeySymbol) :
	m_pStorage(pStorage)
	,m_nKeySymbol(nKeySymbol)
	,m_nNext(-1)
	,m_nPrev(-1)
	,m_nAddr(-1)
	{

	}
	ListDBPage::~ListDBPage()
	{

	}
	bool ListDBPage::load(CommonLib::FxMemoryReadStream *pStream, int64 nAddr)
	{
		if(nAddr == -1)
			return false;


	

		CFilePage * pPage = m_pStorage->getFilePage(nAddr);
		if(!pPage)
			return false;
		pStream->attach(pPage->getRowData(), pPage->getPageSize());
		int64 nSymbol = pStream->readInt64();
		if(nSymbol != m_nKeySymbol)
			return false;
		m_nAddr = nAddr;
		m_nPrev = pStream->readInt64();
		m_nNext = pStream->readInt64();
		return true;
	}
	bool ListDBPage::next(CommonLib::FxMemoryReadStream *pStream)
	{
	 	return	load(pStream, m_nNext);
	}
	bool ListDBPage::prev(CommonLib::FxMemoryReadStream *pStream)
	{
		return	load(pStream, m_nPrev);
	}
	bool ListDBPage::getNext(CFilePage *pFilePage, int64& nNext)
	{
		CommonLib::FxMemoryReadStream stream;
		stream.attach(pFilePage->getRowData(), pFilePage->getPageSize());
		int64 nSymbol = stream.readInt64();
		if(nSymbol != m_nKeySymbol)
			return false;
		stream.readInt64();
		nNext = stream.readInt64();
		return true;
	}*/
}