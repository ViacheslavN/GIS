#include "stdafx.h"
#include "TranStorage.h"
namespace embDB
{
	CTranStorage::CTranStorage(CommonLib::alloc_t *pAlloc) : m_pAlloc(pAlloc), m_nLastAddr(-1)
	{

	}
	CTranStorage::~CTranStorage()
	{

	}
	bool CTranStorage::open(const CommonLib::str_t& sTranName, size_t nPageSize, bool bNew)
	{
		CommonLib::enOpenFileMode nOpenMode = bNew ? CommonLib::ofmCreateAlways : CommonLib::ofmOpenExisting ;
		CommonLib::enAccesRights nReadWrite = CommonLib::aeReadWrite;;
		m_nPageSize = nPageSize;
		bool bRet =  m_pFile.openFile(sTranName, nOpenMode, nReadWrite, CommonLib::smNoMode);
		m_sTranName = sTranName;
		if(bRet)
		{
			m_nLastAddr = m_pFile.getFileSize();
		}
		return bRet;
	}
	bool CTranStorage::close(bool bDelete)
	{
		bool bRet =  m_pFile.closeFile();
		if(!bRet)
			return false;
		if(bDelete)
			return CommonLib::FileSystem::deleteFile(m_sTranName);
		return true;
	}
	int64 CTranStorage::saveFilePage(CFilePage* pPage, int64 nAddr )
	{
		pPage->setFlag(eFP_CHANGE, true);
		if(nAddr == -1)
		{
			nAddr = m_nLastAddr;
			m_nLastAddr += 1;
		}
		bool bRet = m_pFile.setFilePos64(nAddr * m_nPageSize, CommonLib::soFromBegin);
		assert(bRet);
		uint32 nCnt = m_pFile.writeFile((void*)pPage->getRowData(),  (uint32)m_nPageSize );
		assert(nCnt != 0);
		return nAddr;
	}
	CFilePage* CTranStorage::getFilePage(int64 nAddr, bool bRead)
	{
		CFilePage* pPage = new CFilePage(m_pAlloc, m_nPageSize, nAddr);
		if(bRead)
		{
			bool bRet = m_pFile.setFilePos64(nAddr * m_nPageSize, CommonLib::soFromBegin);
			assert(bRet);
			uint32 nWCnt = m_pFile.readFile((void*)pPage->getRowData(),  (uint32)m_nPageSize );
			assert(nWCnt != 0);
		}
	
		if(m_nLastAddr <= nAddr)
			m_nLastAddr =  nAddr + 1;
		return pPage;
	}
	CFilePage* CTranStorage::getNewPage()
	{
		CFilePage* pPage = new CFilePage(m_pAlloc, m_nPageSize, m_nLastAddr);
		m_nLastAddr += 1;
		return pPage;
	}
	int64 CTranStorage::getNewPageAddr()
	{
		int64 nAddr = m_nLastAddr;
		m_nLastAddr += 1;
		return nAddr;
	}
	bool CTranStorage::Flush()
	{
		return m_pFile.Flush();
	}
}