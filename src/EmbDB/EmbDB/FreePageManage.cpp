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
		
		if(!AddFreeMap(m_pStorage->getNewPageAddr(), true))
			return false;

		CommonLib::FxMemoryWriteStream stream;
		stream.attach(pListFreeBitMapsPage->getRowData(), pListFreeBitMapsPage->getPageSize());
		sFilePageHeader header(stream, STORAGE_PAGE, STORAGE_LIST_FREEMAP_PAGE);
		stream.write(int64(-1)); //next
		stream.write(uint32(1));
		stream.write(m_FreeMaps[0].m_nAddr);
		header.writeCRC32(stream);
		m_pStorage->saveFilePage(pListFreeBitMapsPage);

		return true;
		
	}

	bool CFreePageManager::AddFreeMap(int64 nAddr, bool bNew)
	{
		{
			FileFreeMap freeMap;
			freeMap.m_nAddr = nAddr;
			freeMap.m_nBeginAddr = m_FreeMaps.empty() ? 0 : m_FreeMaps.back().m_nAddr + 1;
			freeMap.m_nEndAddr = freeMap.m_nBeginAddr + (m_pStorage->getPageSize() - sFilePageHeader::size()) * 8;
			m_FreeMaps.push_back(freeMap);
		}
		FileFreeMap& freeMap = m_FreeMaps.back();
		if(bNew)
		{
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
			CFilePage* pPage = m_pStorage->getFilePage(nAddr);
			if(!pPage)
			{
				//TO DO Logs
				return false;
			}
			return freeMap.load(pPage);
		}
 
		return true;
	}
	bool CFreePageManager::load()
	{
		for (size_t i = 0, sz = m_FreeMaps.size(); i < sz; ++i)
		{
			FileFreeMap& freeMap = m_FreeMaps[i];
			if(freeMap.m_bChange)
			{
				CFilePage* pPage = m_pStorage->getFilePage(freeMap.m_nAddr, false);
				if(!pPage)
				{
					//TO DO Logs
					return false;
				}
				freeMap.save(pPage);
				freeMap.m_bChange = false;
				m_pStorage->saveFilePage(pPage);
			}
		}
		return true;
	}
	bool CFreePageManager::save()
	{
		return true;
		
	}
	bool CFreePageManager::addPage(__int64 nAddr)
	{
		for (size_t i = 0, sz = m_FreeMaps.size(); i < sz; ++i)
		{
			FileFreeMap& freeMap = m_FreeMaps[i];
			if(freeMap.m_nBeginAddr <= nAddr && freeMap.m_nEndAddr >= nAddr)
			{
				freeMap.setBit(nAddr, true);
				freeMap.m_bChange = true;
				return true;
			}
		}
		if(!AddFreeMap(m_pStorage->getNewPageAddr(), true))
			return false;
		return addPage(nAddr);
	}

	bool CFreePageManager::removeFromFreePage(int64 nAddr)
	{
		for (size_t i = 0, sz = m_FreeMaps.size(); i < sz; ++i)
		{
			FileFreeMap& freeMap = m_FreeMaps[i];
			if(freeMap.m_nBeginAddr <= nAddr && freeMap.m_nEndAddr >= nAddr)
			{
				assert(freeMap.getBit(nAddr));
				freeMap.setBit(nAddr, false);
				freeMap.m_bChange = true;
				return true;
			}
		}
		return false;
	}
	
	
	int64 CFreePageManager::getFreePage()
	{
		for (size_t i = 0, sz = m_FreeMaps.size(); i < sz; ++i)
		{
			if(m_FreeMaps[i].m_FreePages.empty())
				continue;

			FileFreeMap& freeMap = m_FreeMaps[i];
			int64 nAddr = freeMap.m_FreePages.top();
			//freeMap.m_BitMap.setBit(nAddr, false);
			return nAddr;
		}
		return -1;
	}
}