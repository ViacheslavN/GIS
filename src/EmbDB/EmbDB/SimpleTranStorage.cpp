#include "stdafx.h"
#include "SimpleTranStorage.h"

namespace embDB
{
CSimpleTranStorage::CSimpleTranStorage(CommonLib::alloc_t *pAlloc) : m_pAlloc(pAlloc), m_nLastAddr(-1)
{

}
CSimpleTranStorage::~CSimpleTranStorage()
{

}
bool CSimpleTranStorage::open(const CommonLib::str_t& sDbName, size_t nPageSize)
{
	CommonLib::enOpenFileMode nOpenMode = CommonLib::ofmCreateAlways;
	CommonLib::enAccesRights nReadWrite = CommonLib::aeReadWrite;;
	m_nPageSize = nPageSize;
	bool bRet =  m_pFile.openFile(sDbName, nOpenMode, nReadWrite, CommonLib::smNoMode);
	if(bRet)
	{
		m_nLastAddr = m_pFile.getFileSize();
	}
	return bRet;
}
bool CSimpleTranStorage::close()
{
	return m_pFile.closeFile();
}
int64 CSimpleTranStorage::saveFilePage(CFilePage* pPage, int64 nAddr )
{
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
CFilePage* CSimpleTranStorage::getFilePage(int64 nAddr)
{
	bool bRet = m_pFile.setFilePos64(nAddr * m_nPageSize, CommonLib::soFromBegin);
	assert(bRet);
	CFilePage* pPage = new CFilePage(m_pAlloc, m_nPageSize, nAddr);
	uint32 nWCnt = m_pFile.readFile((void*)pPage->getRowData(),  (uint32)m_nPageSize );
	assert(nWCnt != 0);
	if(m_nLastAddr <= nAddr)
		m_nLastAddr =  nAddr + 1;
	return pPage;
}
}