#include "stdafx.h"
#include "storage.h"
#include "DBMagicSymbol.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include <iostream>
namespace embDB
{
	CStorage::CStorage(CommonLib::alloc_t *pAlloc, int32 nCacheSize) :
		m_pAlloc(pAlloc)
		,m_nMaxPageBuf(nCacheSize) 
		,m_Chache(pAlloc)
		,m_nPageSize(DEFAULT_PAGE_SIZE)
		,m_FreePageManager(this, pAlloc)
		,m_nLastAddr(0)
		//,m_StorageStateInfo(this)
		, m_bDirty(false)
		, m_nBeginSize(0)
		, m_nStorageInfo(-1)
		, m_bCommitState(false)
		, m_nCalcFileSize(0)
	{
		
	}
	bool CStorage::open(const CommonLib::str_t& sDbName, bool bReadOnly, bool bNew, bool bCreate, bool bOpenAlways, size_t nPageSize)
	{
		CommonLib::enOpenFileMode nOpenMode;
		CommonLib::enAccesRights nReadWrite;
		if(bNew)
		{
			nOpenMode = CommonLib::ofmCreateNew;
		}
		else if(bCreate)
		{
			nOpenMode = CommonLib::ofmCreateAlways;
		}
		else if(bOpenAlways)
		{
			nOpenMode = CommonLib::ofmOpenAlways;
		}
		else
		{
			nOpenMode = CommonLib::ofmOpenExisting;
		}

		nReadWrite = bReadOnly ? CommonLib::arRead : CommonLib::aeReadWrite;
		m_nPageSize = nPageSize;
		bool bRet =  m_pFile.openFile(sDbName, nOpenMode, nReadWrite, CommonLib::smNoMode);
		if(bRet)
		{
			/*int64 nSize = m_pFile.getFileSize();
			int64 ntt = nSize%m_nPageSize;*/
		
			m_nCalcFileSize = m_pFile.getFileSize();
			assert(m_pFile.getFileSize() % m_nPageSize == 0);
			m_nLastAddr = m_nCalcFileSize/m_nPageSize;
		}
		return bRet;
	}
	void CStorage::setPageSize(size_t nPageSize)
	{
		m_nPageSize = nPageSize;
		//m_nLastAddr = m_pFile.getFileSize();
	}
	size_t CStorage::getPageSize() const
	{
		return m_nPageSize;
	}
	bool CStorage::close()
	{

		TNodesCache::TCacheSet::iterator it =	m_Chache.m_set.begin();
		while(!it.isNull())
		{
			CFilePage* pPage  = it.value().pListEl->obj_;
			delete pPage;
			it.next();
		}
		m_Chache.m_set.clear();
		m_nLastAddr = 0;
	 	return m_pFile.closeFile();
	}
	CStorage::~CStorage()
	{
		TNodesCache::TCacheSet::iterator it =	m_Chache.m_set.begin();
		while(!it.isNull())
		{
			CFilePage* pPage  = it.value().pListEl->obj_;
			delete pPage;
			it.next();
		}
		m_Chache.m_set.clear();
	}
	int64 CStorage::getFileSzie()
	{
		if(m_pFile.isValid())
			return m_pFile.getFileSize();
		return -1;
	}
	FilePagePtr CStorage::getFilePage(int64 nAddr, bool bRead){
		CFilePage* pPage = m_Chache.GetElem(nAddr);
		if(pPage)
			return FilePagePtr(pPage);
		
		pPage = new CFilePage(m_pAlloc, m_nPageSize, nAddr);
		if(bRead)
		{
			bool bRet = m_pFile.setFilePos64(nAddr * m_nPageSize, CommonLib::soFromBegin);
			assert(bRet);
			uint32 nWCnt = m_pFile.readFile((void*)pPage->getRowData(),  (uint32)m_nPageSize );
			assert(nWCnt != 0);
			if(nWCnt == 0)
			{
				std::cout <<"Error read page addr: " << nAddr << "  set file : " << nAddr * m_nPageSize << std::endl;
				return FilePagePtr(NULL);
			}
		}
		
		if(m_nLastAddr <= nAddr)
		{
			assert(false);
			m_nLastAddr =  nAddr + 1;
		}
		if(m_Chache.size() > (size_t)m_nMaxPageBuf)
		{
			CFilePage* pPage = m_Chache.remove_back();
			delete pPage;
		}
		m_Chache.AddElem(pPage->getAddr(), pPage);
		return FilePagePtr(pPage);

	}
	/*CFilePage*CStorage:: createPage(int64 nAddr)
	{
		return new CFilePage(m_pAlloc, m_nPageSize, nAddr);
	}*/
	bool CStorage::dropFilePage(FilePagePtr pPage)
	{
		if(!m_bCommitState)
			return false;

		 CFilePage* pFindPage = m_Chache.remove(pPage->getAddr());
		 int64 nAddr = pFindPage->getAddr();
		 if(pFindPage)
		 {
			 delete pFindPage;
		 }
		return m_FreePageManager.addPage(nAddr);
	}
	bool CStorage::removeFromFreePage(int64 nAddr)
	{
		if(!m_bCommitState)
			return false;
		return m_FreePageManager.removeFromFreePage(nAddr);
	}
	bool CStorage::dropFilePage(int64 nAddr)
	{		
 
		if(!m_bCommitState)
			return false;

		CFilePage* pPage = m_Chache.remove(nAddr);
		if(pPage)
		{
			delete pPage;
		}
		return m_FreePageManager.addPage(nAddr);
	}
	bool CStorage::isValid() const{
		return m_pFile.isValid();
	}
	FilePagePtr CStorage::getNewPage(bool bWrite)
	{
		int64 nAddr = m_FreePageManager.getFreePage();
		bool bFree = false;
		if(nAddr == -1)
		{
			nAddr = m_nLastAddr;
			m_nLastAddr += 1;
		}
		else
			bFree = true;
		CFilePage* pPage = new CFilePage(m_pAlloc, m_nPageSize, nAddr);
		pPage->setFlag(eFP_FROM_FREE_PAGES, bFree);
		/*bool bRet = m_pFile.setFilePos64(m_nLastAddr, CommonLib::soFromBegin);
		assert(bRet);
		CFilePage* pPage = new CFilePage(m_pAlloc, m_nPageSize, m_nLastAddr);
		uint32 nWCnt = m_pFile.writeFile((void*)pPage->getRowData(), (uint32)m_nPageSize );
		assert(nWCnt != 0);*/
		
		if(bWrite)
		{
			uint64 nFileAddr = pPage->getAddr() * m_nPageSize;

			bool bRet = m_pFile.setFilePos64(nFileAddr, CommonLib::soFromBegin);
			assert(bRet);
			uint32 nWCnt = m_pFile.writeFile((void*)pPage->getRowData(), (uint32)m_nPageSize );
			assert(nWCnt != 0);

			if(m_nCalcFileSize < (nFileAddr + m_nPageSize))
			{
				m_nCalcFileSize = (nFileAddr + m_nPageSize);
			}

		}
		if(m_Chache.size() > (size_t)m_nMaxPageBuf)
		{
			 CFilePage* pBackPage = m_Chache.remove_back();
			 if(pBackPage)
				 delete pBackPage;
		}
		m_Chache.AddElem(pPage->getAddr(), pPage);
		return FilePagePtr(pPage);
	}
	int64 CStorage::getNewPageAddr()
	{
		int64 nAddr = m_nLastAddr;
		m_nLastAddr += 1;
		return nAddr;
	}
	bool CStorage::saveFilePage(FilePagePtr pPage, size_t nDataSize,  bool bChandgeInCache)
	{
		return saveFilePage(pPage.get(), nDataSize, bChandgeInCache);
	}
	bool CStorage::saveFilePage(CFilePage* pPage, size_t nDataSize,  bool bChandgeInCache)
	{

		if(bChandgeInCache)
		{
			CFilePage* pCachePage = m_Chache.GetElem(pPage->getAddr());
			if(pCachePage)
			{
				pCachePage->copyFrom(pPage);
			}
		}
		uint64 nFileAddr = pPage->getAddr() * m_nPageSize;
		bool bRet = m_pFile.setFilePos64(pPage->getAddr() * m_nPageSize, CommonLib::soFromBegin);
		assert(bRet);
		if(!bRet)
			return false;

		uint32 nWriteSize = 0;
		if(m_nCalcFileSize < (nFileAddr + m_nPageSize) || nDataSize == 0)
			nWriteSize = (uint32)m_nPageSize;
		else
			nWriteSize = nDataSize;
	
		uint32 nCnt = m_pFile.writeFile((void*)pPage->getRowData(), nWriteSize );
		assert(nCnt == nWriteSize);
		if(m_nCalcFileSize < (nFileAddr + m_nPageSize))
		{
			m_nCalcFileSize = (nFileAddr + m_nPageSize);
		}
		

		return nCnt == nWriteSize;
		
	}
	bool CStorage::saveNewPage(FilePagePtr pPage)
	{
		assert(m_nPageSize == pPage->getPageSize());
		uint64 nFileAddr = pPage->getAddr() * m_nPageSize;
		bool bRet = m_pFile.setFilePos64(nFileAddr, CommonLib::soFromBegin);
		assert(bRet);
		if(!bRet)
			return false;
		uint32 nCnt = m_pFile.writeFile((void*)pPage->getRowData(),  (uint32)m_nPageSize );
		assert(nCnt != 0);
		if(m_nLastAddr < pPage->getAddr())
		{
			assert(0);
			m_nLastAddr =  pPage->getAddr() + 1;
		}
		if(m_nCalcFileSize < (nFileAddr + m_nPageSize))
		{
			m_nCalcFileSize = (nFileAddr + m_nPageSize);
		}
		return nCnt == (uint32)m_nPageSize;
	}


	void CStorage::setStoragePageInfo(int64 nStorageInfo)
	{
		m_nStorageInfo = nStorageInfo;
	}
	bool  CStorage::initStorage(int64 nStorageInfo)
	{
		m_nStorageInfo = nStorageInfo;
		return m_FreePageManager.init(getNewPageAddr(), true);

	}
	bool CStorage::saveStorageInfo()
	{
		FilePagePtr pPage = getFilePage(m_nStorageInfo);
		if(!pPage.get())
			return false;

		int64 nFreeRootPage = m_FreePageManager.getRoot();
		CommonLib::FxMemoryWriteStream stream;
		stream.attach(pPage->getRowData(), pPage->getPageSize());
		sFilePageHeader header(stream, STORAGE_PAGE, STORAGE_INFO_PAGE);
		stream.write(nFreeRootPage);
		stream.write((int32)(m_bDirty ? 1 : 0));
		if(m_bDirty)
		{
			stream.write(m_nBeginSize);
			stream.write((uint32)m_sTranName.length());
			stream.write((byte*)m_sTranName.cwstr(), m_sTranName.length() * 2);
		}
		header.writeCRC32(stream);
		return saveFilePage(pPage);
	}
	bool  CStorage::loadStorageInfo()
	{
		FilePagePtr pPage = getFilePage(m_nStorageInfo);
		if(!pPage.get())
			return false;
		CommonLib::FxMemoryReadStream stream;
		stream.attach(pPage->getRowData(), pPage->getPageSize());
		sFilePageHeader header(stream);
		if(!header.isValid())
		{
			//TO DO Log
			return false;
		}
		if(header.m_nObjectPageType != STORAGE_PAGE || header.m_nSubObjectPageType != STORAGE_INFO_PAGE)
		{
			//TO DO Log
			return false;
		}
		int64 nFreeRootPage =stream.readInt64();
		int32 bDirty = stream.readInt32();
		m_bDirty = (bDirty == 1);
		if(m_bDirty)
		{
			m_nBeginSize = stream.readInt64();
			size_t nlenStr = stream.readInt32();
			if(nlenStr <= 0 || nlenStr > size_t(stream.size() - stream.pos()))
				return false;
			std::vector<wchar_t> buf(nlenStr + 1, L'\0');
			stream.read((byte*)&buf[0], nlenStr * 2);
			m_sTranName = CommonLib::str_t(&buf[0]);
		}
		return m_FreePageManager.init(nFreeRootPage, false);
	}
	bool CStorage::commit()
	{
		m_FreePageManager.save();
		return m_pFile.Flush();
	}

	int64 CStorage::getFileSize()  
	{
		return m_pFile.getFileSize();
	}
	bool CStorage::setFileSize(int64 nSize)
	{
		assert(m_pFile.getFileSize() % m_nPageSize == 0);
		m_nLastAddr = m_pFile.getFileSize()/m_nPageSize;
		return true;
		//return m_pFile.setFileSize();
	}
	/*bool CStorage::saveChache()
	{

		TNodesCache::TCacheSet::iterator it =	m_Chache.m_set.begin();
		while(!it.isNull())
		{
			CFilePage* pPage  = it.value().pListEl->obj_;
			//if(pPage->getFlag() & eFP_CHANGE) 
			//{
				if(!saveFilePage(pPage, false))
					return false;
					delete pPage;
			//	pPage->setFlag(eFP_CHANGE, false);
			//}

			it.next();
		}
		m_Chache.m_set.destroyTree();
		m_Chache.clear();
		return true;
	}*/
	bool CStorage::saveState()
	{
		return true;
	}

	bool CStorage::lockWrite(IDBTransactions *pTran)
	{
	 //TO-DO lock
		if(!pTran)
			return true;
	
		m_bDirty = true;
		m_sTranName = pTran->getFileTranName();
		//m_StorageStateInfo.BeginWriteTransaction(pTran->getFileTranName());
		m_bCommitState = true;
		return saveStorageInfo();
	}
	bool CStorage::unlockWrite(IDBTransactions *pTran)
	{
		//TO-DO lock
		if(!pTran)
			return true;
		//m_StorageStateInfo.EndWriteTransaction(pTran->getFileTranName());
		m_bDirty = false;
		assert(m_sTranName == pTran->getFileTranName());
		m_bCommitState = false;
		return saveStorageInfo();
	}
	int64 CStorage::getBeginFileSize() const
	{
		return -1;
	}
	bool CStorage::isDirty() const
	{
		return m_bDirty;
	}
	const CommonLib::str_t & CStorage::getTranFileName() const
	{
		return m_sTranName; //m_StorageStateInfo.getWriteTransaction();
	}
	 void CStorage::clearDirty()
	{
		m_bDirty = true;
		saveStorageInfo();
	}

	bool  CStorage::saveForUndoState(IDBTransactions *pTran, int64 nPageBegin)
	{

	}
	bool  CStorage::undo(IDBTransactions *pTran, int64 nPageBegin)
	{

	}
}