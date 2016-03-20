#include "stdafx.h"
#include "FreePageManage.h"
#include "storage.h"
#include "DBMagicSymbol.h"

namespace embDB
{
	CFreePageManager::CFreePageManager(CStorage* pStorage, CommonLib::alloc_t * pAlloc) :
		m_nRootPage(-1)
		,m_pStorage(pStorage)
		,m_pAlloc(pAlloc)
		,m_ListFreeMaps(-1, 0, STORAGE_PAGE, STORAGE_LIST_FREEMAP_PAGE)
		, m_nPageSize(8192)
 
	{
		
	}
	CFreePageManager::~CFreePageManager()
	{

	}
	bool CFreePageManager::init(int64 nRootAddr, bool bNew)
	{
		m_nAddrLen = (m_nPageSize - sFilePageHeader::size() - sizeof(uint64)) * 8;
		//m_nAddrLen = 100;
		m_nRootPage = nRootAddr;
		m_ListFreeMaps.setPageSize(m_nPageSize);
		m_FreeMaps.clear();
		if(!bNew)
			return load();
	
		FilePagePtr pListFreeBitMapsPage(m_pStorage->getNewPage(m_nPageSize));
		
		if(!pListFreeBitMapsPage.get())
		{
			//TO DO Logs
			return false;
		}
		m_nFreeMapLists = pListFreeBitMapsPage->getAddr();

		/*CFilePage* pFirstFreeBitMap = m_pStorage->getNewPage();
		if(!pFirstFreeBitMap)
		{
			//TO DO Logs
			return false;
		}*/
		m_ListFreeMaps.setFirstPage(m_nFreeMapLists);
		if(!AddFreeMap(m_pStorage->getNewPageAddr(m_nPageSize), 0,  true))
			return false;

		{
			FileFreeMap* pFreeMaps = m_FreeMaps.begin()->second;
			m_ListFreeMaps.push(pFreeMaps->m_nAddr, m_pStorage);
			/*CommonLib::FxMemoryWriteStream stream;
			stream.attachBuffer(pListFreeBitMapsPage->getRowData(), pListFreeBitMapsPage->getPageSize());
			sFilePageHeader header(stream, STORAGE_PAGE, STORAGE_LIST_FREEMAP_PAGE);
			stream.write(int64(-1)); //next
			stream.write(uint32(1)); //next
			FileFreeMap* pFreeMaps = m_FreeMaps.begin()->second;
			stream.write(pFreeMaps->m_nAddr);
			header.writeCRC32(stream);
			m_pStorage->saveFilePage(pListFreeBitMapsPage);*/
		}
	


		FilePagePtr pRootPage(m_pStorage->getFilePage(m_nRootPage, m_nPageSize));
		if(!pRootPage.get())
		{
			//TO DO Logs
			return false;
		}
		{
			CommonLib::FxMemoryWriteStream stream;
			stream.attachBuffer(pRootPage->getRowData(), pRootPage->getPageSize());
			sFilePageHeader header(stream, STORAGE_PAGE, STORAGE_FREE_MAP_ROOT_PAGE, pRootPage->getPageSize());
			stream.write(m_nFreeMapLists); 
			header.writeCRC32(stream);
			m_pStorage->saveFilePage(pRootPage);
		}
		
		return true;
		
	}

	bool CFreePageManager::AddFreeMap(int64 nAddr, uint64 nBlockNum, bool bNew)
	{
		if(bNew)
		{


			FileFreeMap* pFreeMap =  new FileFreeMap(m_nAddrLen);
			pFreeMap->m_nAddr = nAddr;
			pFreeMap->m_nBlockNum = nBlockNum;
			pFreeMap->m_nBeginAddr = pFreeMap->m_nBlockNum *  m_nAddrLen;
			pFreeMap->m_nEndAddr = pFreeMap->m_nBeginAddr  +  m_nAddrLen - 1;
			pFreeMap->m_BitMap.init(m_nAddrLen/8);
			m_FreeMaps.insert(TMapFreeMaps::value_type(pFreeMap->m_nBlockNum, pFreeMap));

			FilePagePtr pPage(m_pStorage->getFilePage(nAddr, false));
			if(!pPage.get())
			{
				//TO DO Logs
				return false;
			}
			memset(pPage->getRowData(), 0, pPage->getPageSize());
			pFreeMap->save(pPage.get());
			/*memset(pPage->getRowData(), 0, pPage->getPageSize());
			CommonLib::FxMemoryWriteStream stream;
			stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());
			sFilePageHeader header(stream, STORAGE_PAGE, STORAGE_FREE_MAP_PAGE);
			header.writeCRC32(stream);
			pFreeMap->m_BitMap.setBits(pPage->getRowData(), m_nAddrLen/8);*/
			m_pStorage->saveFilePage(pPage);
		}
		else
		{
			FileFreeMap* pFreeMap =  new FileFreeMap(m_nAddrLen);
			FilePagePtr pPage(m_pStorage->getFilePage(nAddr, m_nPageSize));
			if(!pPage.get())
			{
				//TO DO Logs
				return false;
			}
			if(!pFreeMap->load(pPage.get()))
				return false;
			m_FreeMaps.insert(TMapFreeMaps::value_type(pFreeMap->m_nBlockNum, pFreeMap));
			return true;
		}
 
		return true;
	}
	bool CFreePageManager::load()
	{
		FilePagePtr pRootPage(m_pStorage->getFilePage(m_nRootPage, m_nPageSize));
		if(!pRootPage.get())
		{
			//TO DO Logs
			return false;
		}
		{
			CommonLib::FxMemoryReadStream stream;
			stream.attachBuffer(pRootPage->getRowData(), pRootPage->getPageSize());
			sFilePageHeader header(stream, pRootPage->getPageSize());
			if(!header.isValid())
			{
				//TO DO Log
				return false;
			}
			if(header.m_nObjectPageType != STORAGE_PAGE || header.m_nSubObjectPageType != STORAGE_FREE_MAP_ROOT_PAGE )
			{
				//TO DO Log
				return false;
			}

			m_nFreeMapLists = stream.readInt64();
			m_ListFreeMaps.setFirstPage(m_nFreeMapLists);
			if(!m_ListFreeMaps.load(m_pStorage))
			{
				//TO DO Log
				return false;
			}
		}
		TFreeMapLists::iterator it =  m_ListFreeMaps.begin();
		while(!it.isNull())
		{
			FileFreeMap* pFreeMap =  new FileFreeMap(m_nAddrLen);
			FilePagePtr pPage(m_pStorage->getFilePage(it.value(), m_nPageSize));
			if(!pPage.get())
			{
				//TO DO Logs
				return false;
			}
			if(!pFreeMap->load(pPage.get()))
				return false;
			m_FreeMaps.insert(TMapFreeMaps::value_type(pFreeMap->m_nBlockNum, pFreeMap));
			it.next();
		}

		/*FilePagePtr pListFreeBitMapsPage(m_pStorage->getFilePage(m_nFreeMapLists));
		if(!pListFreeBitMapsPage.get())
		{
			//TO DO Logs
			return false;
		}

		{
			CommonLib::FxMemoryReadStream stream;
			stream.attachBuffer(pListFreeBitMapsPage->getRowData(), pListFreeBitMapsPage->getPageSize());
			sFilePageHeader header(stream);
			if(!header.isValid())
			{
				//TO DO Log
				return false;
			}
			if(header.m_nObjectPageType != STORAGE_PAGE || header.m_nSubObjectPageType != STORAGE_LIST_FREEMAP_PAGE )
			{
				//TO DO Log
				return false;
			}
			int64 nNext = stream.readInt64();
			uint32 nNum = stream.readInt32();
			for (uint32 i = 0; i < nNum; ++i)
			{
				int64 nAddr = stream.readInt64();
				FileFreeMap* pFreeMap =  new FileFreeMap(m_nAddrLen);
				FilePagePtr pPage(m_pStorage->getFilePage(nAddr));
				if(!pPage.get())
				{
					//TO DO Logs
					return false;
				}
				if(!pFreeMap->load(pPage.get()))
					return false;
				m_FreeMaps.insert(TMapFreeMaps::value_type(pFreeMap->m_nBlockNum, pFreeMap));

			}

		}*/

		return true;
	}
	bool CFreePageManager::save()
	{


		TMapFreeMaps::iterator it = m_FreeMaps.begin();
		TMapFreeMaps::iterator end = m_FreeMaps.end();

		for (; it != end; ++it)
		{
			FileFreeMap* pFreeMap = it->second;
			if(pFreeMap->m_bChange)
			{
				FilePagePtr pPage(m_pStorage->getFilePage(pFreeMap->m_nAddr, m_nPageSize, false));
				if(!pPage.get())
				{
					//TO DO Logs
					return false;
				}
				pFreeMap->save(pPage.get());
				pFreeMap->m_bChange = false;
				m_pStorage->saveFilePage(pPage.get());
			}
		}

		if(m_vecNewFreeMaps.size())
		{
			for (uint32 i = 0, sz = m_vecNewFreeMaps.size(); i < sz; ++i)
			{
				m_ListFreeMaps.push(m_vecNewFreeMaps[i], m_pStorage);	
			}
		/*	FilePagePtr pFreeMapListsPage(m_pStorage->getFilePage(m_nFreeMapLists));

			CommonLib::FxMemoryWriteStream stream;
			stream.attachBuffer(pFreeMapListsPage->getRowData(), pFreeMapListsPage->getPageSize());
			sFilePageHeader header(stream, STORAGE_PAGE, STORAGE_LIST_FREEMAP_PAGE);
			stream.write(int64(-1)); //next
			stream.write(uint32(m_FreeMaps.size())); 
			it = m_FreeMaps.begin();
			end = m_FreeMaps.end();
			for (; it != end; ++it)
			{
				FileFreeMap* pFreeMap = it->second;
				stream.write(pFreeMap->m_nAddr);
			}

			header.writeCRC32(stream);
			m_pStorage->saveFilePage(pFreeMapListsPage);*/
		}

			
		return true;
		
	}
	bool CFreePageManager::addPage(int64 nAddr)
	{

		 uint64 nBlocNum = nAddr / m_nAddrLen;
		TMapFreeMaps::iterator it = m_FreeMaps.find(nBlocNum);
		if(it == m_FreeMaps.end())
		{
			if(!AddFreeMap(m_pStorage->getNewPageAddr(m_nPageSize), nBlocNum, true))
			{
				//TO DO Log
				return false;
			}
			it = m_FreeMaps.find(nBlocNum);
			if(it == m_FreeMaps.end())
			{
				//TO DO Log
				return false;
			}
			m_vecNewFreeMaps.push_back(it->second->m_nAddr);
		}
	
		FileFreeMap* pFreeMap = it->second;

		assert(!pFreeMap->getBit(nAddr));
		pFreeMap->setBit(nAddr, true);
		pFreeMap->m_FreePages.push(nAddr);
		pFreeMap->m_bChange = true;
		return true;
	}

	bool CFreePageManager::removeFromFreePage(int64 nAddr)
	{

		 uint64 nBlocNum = nAddr / m_nAddrLen;
		TMapFreeMaps::iterator it = m_FreeMaps.find(nBlocNum);

		if(it == m_FreeMaps.end())
		{
			AddFreeMap(m_pStorage->getNewPageAddr(m_nPageSize), nBlocNum, true);
			it = m_FreeMaps.find(nBlocNum);
			if(it == m_FreeMaps.end())
			{
				//TO DO Log
				return false;
			}
			m_vecNewFreeMaps.push_back(it->second->m_nAddr);
		}
			
		FileFreeMap* pFreeMap = it->second;

		assert(pFreeMap->getBit(nAddr));
		pFreeMap->setBit(nAddr, false);
		pFreeMap->m_bChange = true;
		return true;
	
	}
	
	
	int64 CFreePageManager::getFreePage(bool bSave)
	{

		TMapFreeMaps::iterator it =  m_FreeMaps.begin();
		TMapFreeMaps::iterator end =  m_FreeMaps.end();
		for (; it != end; ++it)
		{
			FileFreeMap* pFreeMap = it->second;
			if(pFreeMap->m_FreePages.empty())
				continue;

		
			int64 nAddr = pFreeMap->m_FreePages.top();
			if(bSave)
			{
				pFreeMap->setBit(nAddr, false);
				pFreeMap->m_bChange = true;
			}
			return nAddr;
		}
		return -1;
	}

	bool CFreePageManager::saveForUndoState(IDBTransaction *pTran)
	{

		TMapFreeMaps::iterator it =  m_FreeMaps.begin();
		TMapFreeMaps::iterator end =  m_FreeMaps.end();
		pTran->addUndoPage(m_pStorage->getFilePage(m_nFreeMapLists, m_nPageSize));
		for (; it != end; ++it)
		{
			FileFreeMap* pFreeMap = it->second;
			pTran->addUndoPage(m_pStorage->getFilePage(pFreeMap->m_nAddr, m_nPageSize));

		}
		return true;
	}
	bool CFreePageManager::undo(IDBTransaction *pTran, int64 nPageBegin)
	{

		FilePagePtr pRootPage = pTran->getTranFilePage(nPageBegin, false);
		if(!pRootPage.get())
		{
			//TO DO LOG
			return false;
		}
		CommonLib::FxMemoryReadStream stream;
		stream.attachBuffer(pRootPage->getRowData(), pRootPage->getPageSize());
		sFilePageHeader header(stream, pRootPage->getPageSize());
		if(!header.isValid())
		{
			//TO DO Log
			return false;
		}
		if(header.m_nObjectPageType != TRANSACTION_PAGE || header.m_nSubObjectPageType != UNDO_FREEMAP_PAGES_ROOT )
		{
			//TO DO Log
			return false;
		}

		int64 nPageList = stream.readInt64();
		
		TUndoVector UndoVector(nPageList, m_nPageSize/*pTran->getPageSize()*/, TRANSACTION_PAGE, UNDO_FREEMAP_PAGES_LIST);
		TUndoVector::iterator<IDBTransaction> it = UndoVector.begin(pTran);
		while(!it.isNull())
		{
			const sUndoPageInfo& undoPageInfo = it.value();
			FilePagePtr pTranPage = pTran->getFilePage(undoPageInfo.m_nBitMapAddInTran, m_nPageSize);
			pTranPage->setAddr(undoPageInfo.m_BitMapAddr);
			m_pStorage->saveFilePage(pTranPage);
		}
		return true;
	}


	bool  CFreePageManager::SaveUndoPage(int64 nPageAddr, IDBTransaction *pTran, TUndoVector& UndoVector)
	{
		sUndoPageInfo undoPageInfo;
		undoPageInfo.m_BitMapAddr = nPageAddr;
		FilePagePtr pTranPage = pTran->getTranNewPage(m_nPageSize);
		FilePagePtr pStoragePage = m_pStorage->getFilePage(nPageAddr, m_nPageSize);

		if(!pTranPage.get())
		{
			//TO DO LOGs
			return false;
		}
		if(!pStoragePage.get())
		{
			//TO DO LOGs
			return false;
		}
		undoPageInfo.m_nBitMapAddInTran = pTranPage->getAddr();
		if(!UndoVector.push<IDBTransaction, FilePagePtr>(undoPageInfo, pTran))
		{
			//TO DO LOGs
			return false;
		}
		pTranPage->copyFrom(pStoragePage.get());
		pTran->saveFilePage(pTranPage);
		return true;
	}
}