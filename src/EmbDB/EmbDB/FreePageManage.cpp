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
	{
		m_nAddrLen = m_pStorage->getPageSize() - sFilePageHeader::size() - sizeof(uint32);
	}
	CFreePageManager::~CFreePageManager()
	{

	}
	bool CFreePageManager::init(int64 nRootAddr, bool bNew)
	{

		m_nRootPage = nRootAddr;
		if(!bNew)
			return load();
	
		CFilePage* pListFreeBitMapsPage = m_pStorage->getNewPage();
		
		if(!pListFreeBitMapsPage)
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
		
		if(!AddFreeMap(m_pStorage->getNewPageAddr(), 0,  true))
			return false;

		CommonLib::FxMemoryWriteStream stream;
		stream.attach(pListFreeBitMapsPage->getRowData(), pListFreeBitMapsPage->getPageSize());
		sFilePageHeader header(stream, STORAGE_PAGE, STORAGE_LIST_FREEMAP_PAGE);
		stream.write(int64(-1)); //next
		stream.write(uint32(1));
		stream.write(m_FreeMaps.begin()->m_nAddr);
		header.writeCRC32(stream);
		m_pStorage->saveFilePage(pListFreeBitMapsPage);

		return true;
		
	}

	bool CFreePageManager::AddFreeMap(int64 nAddr, uint32 nBlockNum, bool bNew)
	{

		if(bNew)
		{


			FileFreeMap* pFreeMap =  new FileFreeMap(m_nAddrLen);
			pFreeMap->m_nAddr = nAddr;
			pFreeMap->m_nBlockNum = nBlockNum;
			pFreeMap->m_nBeginAddr = pFreeMap->m_nBlockNum * (1 + m_nAddrLen);
			pFreeMap->m_nEndAddr = pFreeMap->m_nBeginAddr  +  m_nAddrLen;
			m_FreeMaps.insert(pFreeMap->nBlockNum, pFreeMap);

			CFilePage* pPage = m_pStorage->getFilePage(nAddr, false);
			if(!pPage)
			{
				//TO DO Logs
				return false;
			}
			memset(pPage->getRowData(), 0, pPage->getPageSize());
			CommonLib::FxMemoryWriteStream stream;
			stream.attach(pPage->getRowData(), pPage->getPageSize());
			sFilePageHeader header(stream, STORAGE_PAGE, STORAGE_FREE_MAP_PAGE);
			header.writeCRC32(stream);
			m_pStorage->saveFilePage(pPage);
		}
		else
		{
			FileFreeMap* pFreeMap =  new FileFreeMap(m_nAddrLen);
			CFilePage* pPage = m_pStorage->getFilePage(nAddr);
			if(!pPage)
			{
				//TO DO Logs
				return false;
			}
			return pFreeMap->load(pPage);
		}
 
		return true;
	}
	bool CFreePageManager::load()
	{
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
				CFilePage* pPage = m_pStorage->getFilePage(pFreeMap->m_nAddr, false);
				if(!pPage)
				{
					//TO DO Logs
					return false;
				}
				pFreeMap->save(pPage);
				pFreeMap->m_bChange = false;
				m_pStorage->saveFilePage(pPage);
			}
		}
		return true;
		
	}
	bool CFreePageManager::addPage(__int64 nAddr)
	{
		 
		if(!AddFreeMap(m_pStorage->getNewPageAddr(), 1, true))
			return false;
		return addPage(nAddr);
	}

	bool CFreePageManager::removeFromFreePage(int64 nAddr)
	{

		TMapFreeMaps::iterator it = m_FreeMaps.find(nAddr/(1 + m_nAddrLen));

		if(it == m_FreeMaps.end())
		{
			AddFreeMap(m_pStorage->getNewPageAddr(), nAddr/(1 + m_nAddrLen), true);
			it = m_FreeMaps.find(nAddr);
			if(it == m_FreeMaps.end())
			{
				//TO DO LOg
				return false;
			}
		}
			
		FileFreeMap* pFreeMap = it->second;

		assert(pFreeMap->getBit(nAddr));
		pFreeMap->setBit(nAddr, false);
		pFreeMap->m_FreePages.push(nAddr);
		pFreeMap->m_bChange = true;
		return true;
	
	}
	
	
	int64 CFreePageManager::getFreePage()
	{
		for (size_t i = 0, sz = m_FreeMaps.size(); i < sz; ++i)
		{
			if(m_FreeMaps[i]->m_FreePages.empty())
				continue;

			FileFreeMap* pFreeMap = m_FreeMaps[i];
			int64 nAddr = pFreeMap->m_FreePages.top();
			//pFreeMap->m_BitMap.setBit(nAddr, false);
			return nAddr;
		}
		return -1;
	}
}