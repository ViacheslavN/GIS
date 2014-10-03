#include "stdafx.h"
#include "TranUndoPageManager.h"
#include "FilePage.h"
#include "CommonLibrary/FixedMemoryStream.h"

namespace embDB
{
	CTranUndoPageManager::CTranUndoPageManager(IDBTransactions *pTran, CTranStorage *pStorage) :
		m_pTran(pTran)
		,m_pStorage(pStorage)
		,m_nCurPage(-1)
		,m_nLastPos(0)
		,m_undoPages(-1, pTran->getPageSize(), TRANSACTION_PAGE, UNDO_PAGES)
	{

	}
	CTranUndoPageManager::~CTranUndoPageManager()
	{

	}
	bool CTranUndoPageManager::add(const sUndoPageInfo& PageInfo)
	{
		return m_undoPages.push<CTranStorage, CFilePage*>(PageInfo, m_pStorage);
	}

	bool CTranUndoPageManager::save()
	{
		return m_undoPages.save<CTranStorage, CFilePage*>(m_pStorage);
	}

	/*bool CTranUndoPageManager::save(const TUndoDBPages& nPages)
	{
		uint32 nPos = nPages.size();
		uint32 nOffcet = 0;
		while(nPos != 0)
		{

			CFilePage *pFilePage = m_pStorage->getFilePage(m_nCurPage);
			if(!pFilePage)
			{
				m_pTran->error("TRAN: Can't get page");
				return false;
			}
			CommonLib::FxMemoryWriteStream stream;
			stream.attach(pFilePage->getRowData(), pFilePage->getPageSize());
			stream.seek(m_nLastPos ? m_nLastPos : sizeof(int64) + sizeof(size_t) , CommonLib::soFromBegin);

			uint32 nCap = stream.size() - stream.pos();
			uint32 nSizeCnt = nCap/(2 * sizeof(int64));
			if(nSizeCnt > nPos)
				nSizeCnt = nPos;

			for(size_t i = 0; i < nSizeCnt; ++i)
			{
				stream.write(nPages[i + nOffcet].nDBAddr);
				stream.write(nPages[i + nOffcet].nTranAddr);
			}
			nOffcet += nSizeCnt;
			m_nLastPos = stream.pos();
			nPos = nPos - nSizeCnt;
			stream.seek(0, CommonLib::soFromBegin);
			if(nPos > 0)
			{
				m_nCurPage = m_pStorage->getNewPageAddr();
				m_nLastPos = 0;
				stream.write(m_nCurPage);//next
			}
			else
				stream.write(-1);//next
			stream.write(nSizeCnt);
			m_pStorage->saveFilePage(pFilePage);
			delete pFilePage;

		}
	
		
		return true;
	}*/

	bool  CTranUndoPageManager::undo(CTranStorage *pTranStorage, IDBStorage* pDBStorage)
	{
		TUndoPageList::iterator<CTranStorage> it =  m_undoPages.begin(pTranStorage);
		TUndoDBPages undoPage;
		while(!it.isNull())
		{			
			sUndoPageInfo& pageInfo = it.value();
			CFilePage *pFilePage = pTranStorage->getFilePage(pageInfo.nTranAddr);
			if(!pFilePage)
			{
				m_pTran->error("TRAN: Can't get page from Tran");
				return false;
			}
			pFilePage->setAddr(pageInfo.nDBAddr);
			pDBStorage->saveFilePage(pFilePage);
			it.next();
		}


		/*int64 nPage = m_nCurPage;
		while(nPage != -1)
		{
			CFilePage *pFilePage = m_pStorage->getFilePage(nPage);
			if(!pFilePage)
			{
				m_pTran->error("TRAN: Can't get page");
				return false;
			}
			CommonLib::FxMemoryReadStream stream;
			stream.attach(pFilePage->getRowData(), pFilePage->getPageSize());
			nPage = stream.readInt64();
			uint32 nSizeCnt = stream.readInt32();

			sUndoPageInfo page;
			for(size_t i = 0; i < nSizeCnt; ++i)
			{
				page.nDBAddr =stream.readInt64();
				page.nTranAddr = stream.readInt64();
				CFilePage *pFilePage = pTranStorage->getFilePage(page.nTranAddr);
				if(!pFilePage)
				{
					m_pTran->error("TRAN: Can't get page from Tran");
					return false;
				}
				pFilePage->setAddr(page.nDBAddr);
				pDBStorage->saveFilePage(pFilePage);
			}

		}*/
		


		return true;
	}
}
