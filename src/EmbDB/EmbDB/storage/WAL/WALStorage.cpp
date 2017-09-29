#include "stdafx.h"
#include "WALStorage.h"

namespace embDB
{

	FilePagePtr CWALStorage::getFilePage(int64 nAddr, uint32 nSize, bool bRead, bool bNeedDecrypt)
	{
		stopCopy();

		// TO DO Lock
		auto it = m_PageAddrs.find(nAddr);
		if (it == m_PageAddrs.end())
			return m_pDBStorage->getFilePage(nAddr, nSize, bRead, bNeedDecrypt);
		else
		{
			assert(it->second.second == nSize);
			auto pPage = m_pDBLogStorage->getFilePage(it->second.first, nSize, bRead, bNeedDecrypt);
			if(pPage.get())
				pPage->setAddr(nAddr);

			return pPage;
		}
	}

	bool CWALStorage::saveFilePage(CFilePage* pPage, uint32 nDataSize = 0, bool ChandgeInCache = false)
	{
		stopCopy();

		// TO DO Lock


	}


	void CWALStorage::stopCopy()
	{

	}

	void CWALStorage::copyPage()
	{
		while (true)
		{
			//TO DO lock

			if (!IsCopy())
				break;

		
			if (m_PageAddrs.empty())
				break;
			bool bNeedSave = false;

			for (auto it = m_PageAddrs.begin(); it != m_PageAddrs.end(); )
			{
				if (!IsCopy())
					break;

				auto pPage = m_pDBLogStorage->getFilePage(it->first, it->second.second, true, false);
				pPage->setAddr(it->second.first);
				m_pDBStorage->saveFilePage(pPage, it->second.second);
				it = m_PageAddrs.erase(it);
				bNeedSave = true;
			}
			if(bNeedSave)
				m_pDBStorage->commit();
		}


		
	}

	bool IsCopy()
	{
		return false;
	}
}