#include "stdafx.h"
#include "BPTreeInfoPage.h"

namespace embDB
{

	/*BPNewPageStorage::BPNewPageStorage(CTranStorage *pTranStorage, CommonLib::alloc_t *pAlloc) :
		m_pTranStorage(pTranStorage)
		,m_nMaxPageBuf(100) 
		,m_Chache(pAlloc)
	{

	}
	BPNewPageStorage::~BPNewPageStorage()
	{
		TNodesCache::TCacheSet::iterator it =	m_Chache.m_set.begin();
		while(!it.isNull())
		{
			CFilePage* pPage  = it.value().pListEl->obj_;
			delete pPage;
			it.next();
		}
	}
	int64 BPNewPageStorage::saveFilePage(FilePagePtr pPage, uint32 pos)
	{
		return m_pTranStorage->saveFilePage(pPage.get(), pPage->getAddr());
	}
	FilePagePtr BPNewPageStorage::getFilePage(int64 nAddr, bool bRead)
	{
		CFilePage* pPage = m_Chache.GetElem(nAddr);
		if(pPage)
			return FilePagePtr(pPage);
		pPage = m_pTranStorage->getFilePage(nAddr, bRead);
		if(m_Chache.size() > m_nMaxPageBuf)
		{
			CFilePage* pRemPage = m_Chache.remove_back();
			delete pRemPage;
		}
		m_Chache.AddElem(pPage->getAddr(), pPage);
			return FilePagePtr(pPage);
	}
	FilePagePtr BPNewPageStorage::getNewPage()
	{
		CFilePage*	pPage = m_pTranStorage->getNewPage();
		if(m_Chache.size() > m_nMaxPageBuf){
			CFilePage* pRemPage = m_Chache.remove_back();
			delete pRemPage;
		}
		m_Chache.AddElem(pPage->getAddr(), pPage);
		return FilePagePtr(pPage);
	}*/

}