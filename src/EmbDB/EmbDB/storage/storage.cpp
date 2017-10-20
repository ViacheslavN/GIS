#include "stdafx.h"
#include "storage.h"
 
#include "CommonLibrary/FixedMemoryStream.h"
#include <iostream>
namespace embDB
{
	CStorage::CStorage(CommonLib::alloc_t *pAlloc, int32 nCacheSize, bool bCheckCRC,
		bool bMultiThread) :
		m_pAlloc(pAlloc)
		,m_nMaxPageBuf(nCacheSize) 
		,m_Chache(pAlloc, FilePagePtr())
		,m_nBasePageSize(PAGE_SIZE_8K)
#ifdef USE_FREE_PAGES
		,m_FreePageManager(this, pAlloc)
#endif
		,m_nLastAddr(0)
		//,m_StorageStateInfo(this)
		, m_bDirty(false)
		, m_nBeginSize(0)
		, m_nStorageInfo(-1)
		, m_bCommitState(false)
		, m_nCalcFileSize(0)
		//, m_MemCache(pAlloc)
		, m_pPageChiper(NULL)
		, m_nOffset(0)
		, m_bCheckCRC(bCheckCRC)
	 
	{
		if(bMultiThread)
		{
			m_pCommonLockObj = &m_ComLock;
			m_pWriteLockObj = &m_WriteLock;
		}
		else
		{
			m_pCommonLockObj = &m_ComEmptyLock;
			m_pWriteLockObj = &m_WriteEmptyLock;
		}
	}
	bool CStorage::open(const wchar_t* pszName, bool bReadOnly, bool bNew, bool bCreate, bool bOpenAlways/*, uint32 nPageSize*/)
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
		//m_nPageSize = nPageSize;
		bool bRet =  m_pFile.openFile(pszName , nOpenMode, nReadWrite, CommonLib::smNoMode);
		if(bRet)
		{
			/*int64 nSize = m_pFile.getFileSize();
			int64 ntt = nSize%m_nPageSize;*/
		
			//m_nCalcFileSize = m_pFile.getFileSize();
			//assert(m_pFile.getFileSize() % m_nPageSize == 0);
			//m_nLastAddr = m_nCalcFileSize/m_nPageSize;

		//	m_MemCache.init(m_nBasePageSize, m_nMaxPageBuf);
		}

		 
		return bRet;
	}
	/*void CStorage::setPageSize(uint32 nPageSize)
	{
		m_nBasePageSize = nPageSize;
		//m_nLastAddr = m_pFile.getFileSize();
	}
	uint32 CStorage::getPageSize() const
	{
		return m_nBasePageSize;
	}*/
	bool CStorage::close()
	{

		/*TNodesCache::TCacheSet::iterator it =	m_Chache.m_set.begin();
		while(!it.isNull())
		{
			CFilePage* pPage  = it.value().pListEl->obj_;
			delete pPage;
			it.next();
		}
		m_Chache.m_set.clear();*/
		/*TNodesCache::iterator it = m_Chache.begin();
		while(!it.isNull())
		{
			CFilePage* pPage  =  it.object();
			delete pPage;
			it.next();
		}*/
		m_Chache.clear();
		m_nLastAddr = 0;
	 	return m_pFile.closeFile();
	}
	CStorage::~CStorage()
	{

		/*TNodesCache::iterator it = m_Chache.begin();
		while(!it.isNull())
		{
			CFilePage* pPage  =  it.object();
			delete pPage;
			it.next();
		}*/
		//m_MemCache.clear();
	}

	void CStorage::SetOffset(int64 nOffset)
	{
		m_nOffset = nOffset;
	}
	int64 CStorage::GetOffset() const
	{
		return m_nOffset;
	}



	int64 CStorage::getFileSize()
	{
		if(m_pFile.isValid())
			return m_pFile.getFileSize();
		return -1;
	}
	FilePagePtr CStorage::getFilePage(int64 nAddr, uint32 nSize, bool bRead, bool bNeedDecrypt, bool bAddInCache )
	{
		CommonLib::ILockObject::scoped_lock lock(m_pCommonLockObj);
		FilePagePtr pPage = m_Chache.GetElem(nAddr);
		if (pPage.get())
		{
			//if (!bAddInCache)
			//	m_Chache.remove(nAddr);
			return pPage;
		}


	/*	if((nSize % m_nBasePageSize) != 0 )
			return FilePagePtr();
		if(nSize == 0)
			nSize = m_nBasePageSize;

		uint32 nCount = nSize/m_nBasePageSize;*/


		
		pPage = new CFilePage(m_pAlloc, m_nBasePageSize/*nSize*/, nAddr);
		if(bRead)
		{
			bool bRet = m_pFile.setFilePos64(m_nOffset + (nAddr * m_nBasePageSize), CommonLib::soFromBegin);
			assert(bRet);
			uint32 nWCnt = m_pFile.readFile((void*)pPage->getRowData(), m_nBasePageSize/*nSize*/);
			assert(nWCnt != 0);

			if(m_pPageChiper && bNeedDecrypt)
			{
				m_pPageChiper->decrypt(pPage.get());
			}
			if(nWCnt == 0)
			{
				//std::cout <<"Error read page addr: " << nAddr << "  set file : " << nAddr * nSize << std::endl;
				return FilePagePtr(NULL);
			}
		}
		
		/*if(m_nLastAddr < nAddr && nAddr != m_nStorageInfo)
		{
			assert(false);
			m_nLastAddr =  nAddr + nCount;
		}*/
		if (bAddInCache)
		{
			if (m_Chache.size() > (uint32)m_nMaxPageBuf)
			{
				m_Chache.remove_back();
			}
			m_Chache.AddElem(pPage->getAddr(), pPage);
		}
	
		return FilePagePtr(pPage);

	}
	/*CFilePage*CStorage:: createPage(int64 nAddr)
	{
		return new CFilePage(m_pAlloc, m_nPageSize, nAddr);
	}*/
	bool CStorage::dropFilePage(FilePagePtr pPage)
	{ 
		int64 nAddr = pPage->getRealAddr() != -1 ? pPage->getRealAddr() : pPage->getAddr();
		return dropFilePage(nAddr);

	}
	bool CStorage::removeFromFreePage(int64 nAddr)
	{

		if(!m_bCommitState)
			return false;
#ifdef USE_FREE_PAGES
		return m_FreePageManager.removeFromFreePage(nAddr);
#else
		return true;
#endif
	}
	bool CStorage::dropFilePage(int64 nAddr)
	{		
		CommonLib::ILockObject::scoped_lock lock(m_pCommonLockObj);
		if(!m_bCommitState)
			return false;

		m_Chache.remove(nAddr);
	
#ifdef USE_FREE_PAGES
		return m_FreePageManager.addPage(nAddr);
#else
		return true;
#endif
	}
	bool CStorage::isValid() const{
		return m_pFile.isValid();
	}
	FilePagePtr CStorage::getNewPage(uint32 nSize, bool bWrite, bool bAddInCache)
	{
		CommonLib::ILockObject::scoped_lock lock(m_pCommonLockObj);
		if((nSize % m_nBasePageSize) != 0 )
			return FilePagePtr();
		if(nSize == 0)
			nSize = m_nBasePageSize;

		uint32 nCount = nSize/m_nBasePageSize;

#ifdef USE_FREE_PAGES
		int64 nAddr = m_FreePageManager.getFreePage(m_bCommitState);
		bool bFree = false;
		if(nAddr == -1)
		{
			nAddr = m_nLastAddr;
			m_nLastAddr += 1;
		}
		else
		{
			bFree = true;
		}
#else
		int64 nAddr = m_nLastAddr;
		m_nLastAddr += nCount;
#endif
		
		CFilePage* pPage = new CFilePage(/*&m_MemCache*/m_pAlloc,nSize, nAddr);
#ifdef USE_FREE_PAGES
		pPage->setFlag(eFP_FROM_FREE_PAGES, bFree);
#endif
		/*bool bRet = m_pFile.setFilePos64(m_nLastAddr, CommonLib::soFromBegin);
		assert(bRet);
		CFilePage* pPage = new CFilePage(m_pAlloc, m_nPageSize, m_nLastAddr);
		uint32 nWCnt = m_pFile.writeFile((void*)pPage->getRowData(), (uint32)m_nPageSize );
		assert(nWCnt != 0);*/
		
		if(bWrite)
		{
			uint64 nFileAddr = pPage->getAddr() * nSize;

			bool bRet = m_pFile.setFilePos64(m_nOffset + nFileAddr, CommonLib::soFromBegin);
			assert(bRet);
			uint32 nWCnt = m_pFile.writeFile((void*)pPage->getRowData(), nSize);
			assert(nWCnt != 0);

			if(m_nCalcFileSize < (nFileAddr + nSize))
			{
				m_nCalcFileSize = (nFileAddr + nSize);
			}

		}
		if(bAddInCache && m_Chache.size() > (uint32)m_nMaxPageBuf)
		{
			m_Chache.remove_back();
		}
		FilePagePtr pPagePtr(pPage);
		if(bAddInCache)
			m_Chache.AddElem(pPage->getAddr(), pPagePtr);
		return pPagePtr;
	}
	int64 CStorage::getNewPageAddr(uint32 nSize)
	{
		CommonLib::ILockObject::scoped_lock lock(m_pCommonLockObj);
		if (nSize == 0)
			nSize = m_nBasePageSize;

		if((nSize % m_nBasePageSize) != 0 )
			return -1;

		uint32 nCount = nSize != 0 ? nSize/m_nBasePageSize : 1;
		 
#ifdef USE_FREE_PAGES
		int64 nAddr = m_FreePageManager.getFreePage(m_bCommitState);
		if(nAddr != -1)
		{
			if(nType)
				*nType |= eFP_FROM_FREE_PAGES;
		
			return nAddr;
		}
		nAddr = m_nLastAddr;
#else
		int64 nAddr = m_nLastAddr;
#endif

		m_nLastAddr += /*nCount*/1;
		return nAddr;
	}
	bool CStorage::saveFilePage(FilePagePtr pPage, bool bChandgeInCache)
	{
		return saveFilePage(pPage.get(),  bChandgeInCache);
	}
	bool CStorage::saveFilePage(CFilePage* pPage, bool bChandgeInCache)
	{
		CommonLib::ILockObject::scoped_lock lock(m_pCommonLockObj);

		int64 nAddr = pPage->getRealAddr() != -1 ? pPage->getRealAddr() : pPage->getAddr();

		if(bChandgeInCache)
		{
			FilePagePtr pCachePage = m_Chache.GetElem(nAddr);
			if(pCachePage.get())
			{
				pCachePage->copyFrom(pPage);
			}
		}
		uint64 nFileAddr = nAddr* pPage->getPageSize();
		bool bRet = m_pFile.setFilePos64(m_nOffset + nFileAddr, CommonLib::soFromBegin);
		assert(bRet);
		if(!bRet)
			return false;

		/*uint32 nWriteSize = 0;
		if (m_nCalcFileSize < (nFileAddr + m_nBasePageSize || nDataSize == 0))
			nWriteSize = (uint32)pPage->getPageSize();
		else
			nWriteSize = nDataSize;*/
	
		uint32 nCnt = 0;
		if(m_pPageChiper && pPage->isNeedEncrypt())
		{
			m_BufForChiper.reserve(pPage->getPageSize());
			m_pPageChiper->encrypt(pPage, m_BufForChiper.buffer(), pPage->getPageSize());
			nCnt = m_pFile.writeFile((void*) m_BufForChiper.buffer(), pPage->getPageSize());
		}
		else
			nCnt = m_pFile.writeFile((void*)pPage->getRowData(), pPage->getPageSize());
		assert(nCnt == pPage->getPageSize());
		if(m_nCalcFileSize < (nFileAddr + pPage->getPageSize()))
		{
			m_nCalcFileSize = (nFileAddr + pPage->getPageSize());
		}
		return nCnt == pPage->getPageSize();
		
	}

	bool CStorage::WriteRowData(const byte* pData, uint32 nSize, int64 nPos)
	{
		CommonLib::ILockObject::scoped_lock lock(m_pCommonLockObj);
		if (nPos != -1)
		{
			bool bRet = m_pFile.setFilePos64(nPos, CommonLib::soFromBegin);
			assert(bRet);
			if (!bRet)
				return false;
		}
		
		uint32 nCnt = m_pFile.writeFile((void*)pData, nSize);
		return nCnt == nSize;
	}
	bool CStorage::ReadRowData(const byte* pData, uint32 nSize, int64 nPos)
	{
		CommonLib::ILockObject::scoped_lock lock(m_pCommonLockObj);
		if (nPos != -1)
		{
			bool bRet = m_pFile.setFilePos64(nPos, CommonLib::soFromBegin);
			assert(bRet);
			if (!bRet)
				return false;
		}

		uint32 nCnt = m_pFile.readFile((void*)pData, nSize);
		return nCnt == nSize;
	}
	bool CStorage::saveNewPage(FilePagePtr pPage)
	{
		CommonLib::ILockObject::scoped_lock lock(m_pCommonLockObj);
 
		int64 nAddr = pPage->getRealAddr() != -1 ? pPage->getRealAddr() : pPage->getAddr();

		uint64 nFileAddr = nAddr * m_nBasePageSize;
		bool bRet = m_pFile.setFilePos64(m_nOffset + nFileAddr, CommonLib::soFromBegin);
		assert(bRet);
		if(!bRet)
			return false;


		uint32 nCnt = 0;
		if(m_pPageChiper && pPage->isNeedEncrypt())
		{
			m_BufForChiper.reserve(pPage->getPageSize());
			m_pPageChiper->encrypt(pPage.get(), m_BufForChiper.buffer(), pPage->getPageSize());
			nCnt = m_pFile.writeFile((void*) m_BufForChiper.buffer(), pPage->getPageSize());
		}
		else
			nCnt = m_pFile.writeFile((void*)pPage->getRowData(),  (uint32)pPage->getPageSize() );
 
		assert(nCnt != 0);
		if(m_nLastAddr < nAddr)
		{
			assert(0);
			m_nLastAddr = nAddr + 1;
		}
		if(m_nCalcFileSize < (nFileAddr + pPage->getPageSize()))
		{
			m_nCalcFileSize = (nFileAddr + pPage->getPageSize());
		}
		return nCnt == (uint32)pPage->getPageSize();
	}


	void CStorage::setStoragePageInfo(int64 nStorageInfo)
	{
		m_nStorageInfo = nStorageInfo;
	}
	bool  CStorage::initStorage(int64 nStorageInfo)
	{
		m_nStorageInfo = nStorageInfo;
#ifdef USE_FREE_PAGES
		return m_FreePageManager.init(getNewPageAddr(), true);
#else
		return true;
#endif

	}
	bool CStorage::saveStorageInfo()
	{
		FilePagePtr pPage = getFilePage(m_nStorageInfo, m_nBasePageSize);
		if(!pPage.get())
			return false;
#ifdef USE_FREE_PAGES
		int64 nFreeRootPage = m_FreePageManager.getRoot();
#else
		int64 nFreeRootPage = -1;
#endif
		CommonLib::FxMemoryWriteStream stream;
		stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());
		sFilePageHeader header(stream, STORAGE_PAGE, STORAGE_INFO_PAGE, pPage->getPageSize(), m_bCheckCRC);
		stream.write(nFreeRootPage);
		stream.write(m_nLastAddr);
		stream.write((int32)(m_bDirty ? 1 : 0));
		if(m_bDirty)
		{
			stream.write(m_nBeginSize);
			stream.write((uint32)m_sTranName.length());
			stream.write((byte*)m_sTranName.cwstr(), m_sTranName.length() * 2);
			stream.write((uint16)m_nTrantype);
		}
		if(m_bCheckCRC)
			header.writeCRC32(stream);
		return saveFilePage(pPage);
	}
	bool  CStorage::loadStorageInfo()
	{
		FilePagePtr pPage = getFilePage(m_nStorageInfo, m_nBasePageSize);
		if(!pPage.get())
			return false;
		CommonLib::FxMemoryReadStream stream;
		stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());
		sFilePageHeader header(stream, pPage->getPageSize(), m_bCheckCRC);
		if(m_bCheckCRC && !header.isValid())
		{
			//TO DO Log
			return false;
		}
		if(header.m_nObjectPageType != STORAGE_PAGE || header.m_nSubObjectPageType != STORAGE_INFO_PAGE)
		{
			//TO DO Log
			return false;
		}
		int64 nFreeRootPage = stream.readInt64();
		m_nLastAddr = stream.readInt64();
		int32 bDirty = stream.readInt32();
		m_bDirty = (bDirty == 1);
		if(m_bDirty)
		{
			m_nBeginSize = stream.readInt64();
			uint32 nlenStr = stream.readInt32();
			if(nlenStr <= 0 || nlenStr > uint32(stream.size() - stream.pos()))
				return false;
			std::vector<wchar_t> buf(nlenStr + 1, L'\0');
			stream.read((byte*)&buf[0], nlenStr * 2);
			m_sTranName = CommonLib::CString(&buf[0]);

			m_nTrantype = (eDBTransationType)stream.readintu16();
		}
		//m_MemCache.init(m_nBasePageSize, m_nMaxPageBuf);
#ifdef USE_FREE_PAGES
		return m_FreePageManager.init(nFreeRootPage, false);
#else
		return true;
#endif
	}
	bool CStorage::commit()
	{
#ifdef USE_FREE_PAGES
		m_FreePageManager.save();
#endif
		return m_pFile.Flush();
	}
	bool CStorage::setFileSize(int64 nSize)
	{
		if(nSize == -1)
		{
			int64 nSize = m_pFile.getFileSize();
			assert(nSize % m_nBasePageSize == 0);
			m_nLastAddr = nSize/m_nBasePageSize;
			m_nCalcFileSize = m_nLastAddr;
		}
		else
		{
			assert(nSize % m_nBasePageSize == 0);
			m_nLastAddr = (nSize / m_nBasePageSize) + 1;
			
		}
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

	bool CStorage::lockWrite(IDBTransaction *pTran)
	{
	   m_pWriteLockObj->lock();
		if(!pTran)
			return true;
	
		m_bDirty = true;
		m_sTranName = pTran->getFileTranName();
		m_nTrantype = pTran->getDBTransationType();
		//m_StorageStateInfo.BeginWriteTransaction(pTran->getFileTranName());
		m_bCommitState = true;
		return saveStorageInfo();
	}
	bool CStorage::unlockWrite(IDBTransaction *pTran)
	{
		
		if(!pTran)
			return true;
		//m_StorageStateInfo.EndWriteTransaction(pTran->getFileTranName());
		m_bDirty = false;
		assert(m_sTranName == pTran->getFileTranName());
		m_bCommitState = false;
		m_nTrantype = eTTUndefined;
		saveStorageInfo();
		m_pWriteLockObj->unlock();
		return true;
	}
	int64 CStorage::getBeginFileSize() const
	{
		return -1;
	}
	bool CStorage::isDirty() const
	{
		return m_bDirty;
	}
	const CommonLib::CString & CStorage::getTranFileName() const
	{
		return m_sTranName; //m_StorageStateInfo.getWriteTransaction();
	}

	eDBTransationType CStorage::getTranDBType() const
	{
		return m_nTrantype;
	}


	 void CStorage::clearDirty()
	{
		m_bDirty = false;
		saveStorageInfo();
	}

	bool  CStorage::saveForUndoState(IDBTransaction *pTran)
	{
#ifdef USE_FREE_PAGES		
		return m_FreePageManager.saveForUndoState(pTran);
#else
		return true;
#endif
	}
	bool  CStorage::undo(IDBTransaction *pTran)
	{
		return true;
		//return m_FreePageManager.undo(pTran, nPageBegin);
	}
	bool CStorage::reload()
	{
		return loadStorageInfo();
	}
}